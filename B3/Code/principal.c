#include "principal.h"
#include "cmsis_os2.h"
#include <stdio.h>

/** BUFFER **/
typedef enum{FALSE = 0, TRUE = 1}bool_t;

typedef struct{
	uint16_t frequency;
	uint8_t  freq_rssi;
}buffer_data_t;

typedef struct {
	buffer_data_t* const buffer;
	uint8_t primer_elemento;
	uint8_t ultimo_elemento;
	const uint8_t maxlen;
}circ_bbuf_t;

int buf_in(circ_bbuf_t* buf, buffer_data_t data){
	static uint8_t next_elemento;

	next_elemento = buf->primer_elemento + 1;

	if(next_elemento >= buf->maxlen){
		next_elemento = 0;
	}
	if(next_elemento == buf->ultimo_elemento){
		return -1;
	}
	buf->buffer[buf->primer_elemento] = data;
	buf->primer_elemento = next_elemento;
	return 0;
}

int buf_out(circ_bbuf_t* buf, buffer_data_t* data){
	static uint8_t next_elemento;

	if(buf->primer_elemento == buf->ultimo_elemento){
		return -1;
	}
	next_elemento = buf->ultimo_elemento + 1;
	if(next_elemento >= buf->maxlen){
		next_elemento = 0;
	}
	*data = buf->buffer[buf->ultimo_elemento];
	buf->ultimo_elemento = next_elemento;
	return 0;
}

extern uint32_t sec;

typedef enum {reposo, manual, memoria, prog_hora} estados_t;

//static int muestreo_hora(uint8_t*, uint8_t*, uint8_t*, uint8_t*);
static void gestion_vol(estados_t* estado, MSGQUEUE_OBJ_RDA_MISO* msg_rda_miso);

static osThreadId_t id_Th_principal;
static void Th_principal(void *argument);

int Init_Th_principal(void){
  id_Th_principal = osThreadNew(Th_principal, NULL, NULL);
  if(id_Th_principal == NULL)
    return(-1);
  return(Init_Th_clock() | 
					Init_Th_com() | 
					Init_Th_joystick() | 
					Init_Th_lcd() | 
					Init_Th_pwm() | 
					Init_Th_rda() | 
					Init_Th_temp() | 
					Init_Th_vol() |
					Init_Th_rgb());
}

static void Th_principal(void *argument){
	static estados_t estado = reposo;
	static uint8_t hora, min, seg, prev_sec = 1;
	
	static MSGQUEUE_OBJ_COM msg_com;
	static MSGQUEUE_OBJ_JOY msg_joy;
	static MSGQUEUE_OBJ_LCD msg_lcd;
	static MSGQUEUE_OBJ_RDA_MOSI msg_rda_mosi;
	static MSGQUEUE_OBJ_RDA_MISO msg_rda_miso;
	static MSGQUEUE_OBJ_TEMP msg_temp;
	static MSGQUEUE_OBJ_VOL msg_vol;
	
	while(1){
		gestion_vol(&estado, &msg_rda_miso);
		switch(estado){
			case reposo:
				if(prev_sec != sec){
					prev_sec = sec;
					sec_to_multiple(sec, &hora, &min, &seg);
					
					osMessageQueueGet(get_id_MsgQueue_temp(), &msg_temp, NULL, osWaitForever);
					sprintf(msg_lcd.data_L1, " SBM 2022  T:%.1fºC", msg_temp.temperature);
					sprintf(msg_lcd.data_L2, "      %.2u:%.2u:%.2u", hora, min, seg);
					osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd, NULL, 0U);
				}
				if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &msg_joy, NULL, 0U)){
					if((msg_joy.tecla == Centro) && (msg_joy.duracion == Larga)){
						msg_rda_mosi.comando = cmd_power_on;
						osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
						
						msg_rda_mosi.comando = cmd_set_freq;
						msg_rda_mosi.data = 980;
						osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
						osMessageQueueGet(get_id_MsgQueue_rda_miso(), &msg_rda_miso, NULL, osWaitForever);
						estado = manual;
					}
				}
			break;
			
			case manual:
				if(prev_sec != sec){
					prev_sec = sec;
					sec_to_multiple(sec, &hora, &min, &seg);
					
					osMessageQueueGet(get_id_MsgQueue_temp(), &msg_temp, NULL, 0U);
					
					sprintf(msg_lcd.data_L1, "  %.2u:%.2u:%.2u - T:%.1fºC", hora, min, seg, msg_temp.temperature);
					sprintf(msg_lcd.data_L2, "    F: %d.%d Vol: %d", (msg_rda_miso.frequency / 10), (msg_rda_miso.frequency % 10), msg_rda_miso.volume);
					osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd, NULL, 0U);
				}
				if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &msg_joy, NULL, 0U)){
					if(msg_joy.duracion == Corta){
						switch (msg_joy.tecla){
							case Arriba:
								msg_rda_mosi.comando = cmd_seek_up;
								osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
							break;
							
							case Derecha:
								msg_rda_mosi.comando = cmd_get_info;
								osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
								osMessageQueueGet(get_id_MsgQueue_rda_miso(), &msg_rda_miso, NULL, 0U);
								//memorizar msg_rda_miso.frequency
							break;
							
							case Abajo:
								msg_rda_mosi.comando = cmd_set_freq;
								msg_rda_mosi.data = 980;
								osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
							break;
							
							case Izquierda:
								msg_rda_mosi.comando = cmd_set_freq;
								msg_rda_mosi.data = 1017;
								osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
							break;
							
							case Centro:
								
							break;
						}
					}
				}
				
			break;
			
			case memoria:
				
			break;
			
			case prog_hora:
				
			break;
		}
		osThreadYield();
	}
}

static void gestion_vol(estados_t* estado, MSGQUEUE_OBJ_RDA_MISO* msg_rda_miso){
	static MSGQUEUE_OBJ_VOL msg_vol;
	static MSGQUEUE_OBJ_RGB msg_rgb;
	static MSGQUEUE_OBJ_RDA_MOSI msg_rda_mosi;
	static osStatus_t a;
	
	a = osMessageQueueGet(get_id_MsgQueue_vol(), &msg_vol, NULL, 0U);
	
	if(osOK == a){
		msg_rgb.pulse = msg_vol.volume_lvl;
		osMessageQueuePut(get_id_MsgQueue_rgb(), &msg_rgb, NULL, 0U);
		if((*estado == memoria) | (*estado == manual)){
			msg_rda_mosi.comando = cmd_set_vol;
			msg_rda_mosi.data = msg_vol.volume_lvl;
			osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
			osMessageQueueGet(get_id_MsgQueue_rda_miso(), &msg_rda_miso, NULL, 0U);
		}
	}
}
