#include "principal.h"
#include "cmsis_os2.h"
#include <stdio.h>

extern uint32_t sec;

typedef enum{reposo, manual, memoria, prog_hora} estados_t;

static int muestreo_temp(MSGQUEUE_OBJ_TEMP*, float*);
static int muestreo_hora(uint8_t*, uint8_t*, uint8_t*, uint8_t*);
static void gestion_vol(void);

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
					Init_Th_vol());
}

static void Th_principal(void *argument){
	static estados_t estado = reposo;
	static uint8_t hora, min, seg, prev_sec;
	static float prev_temp;
	
	MSGQUEUE_OBJ_COM msg_com;
	MSGQUEUE_OBJ_JOY msg_joy;
	MSGQUEUE_OBJ_LCD msg_lcd;
	MSGQUEUE_OBJ_RDA_MOSI msg_rda_mosi;
	MSGQUEUE_OBJ_RDA_MISO msg_rda_miso;
	MSGQUEUE_OBJ_TEMP msg_temp;
	MSGQUEUE_OBJ_VOL msg_vol;
	
	while(1){
//		muestreo_temp(&msg_temp, &prev_temp);
//		muestreo_hora(&hora, &min, &seg, &prev_sec);
		gestion_vol();
		switch(estado){
			case reposo:
				if(muestreo_hora(&hora, &min, &seg, &prev_sec) || muestreo_temp(&msg_temp, &prev_temp)){
					sprintf(msg_lcd.data_L1, " SBM 2022  T:%.1fºC", msg_temp.temperature);
					sprintf(msg_lcd.data_L2, "      %.2u:%.2u:%.2u", hora, min, seg);
					osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd, NULL, 0U);
				}
				if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &msg_joy, NULL, 0U)){
					if((msg_joy.tecla == Centro) && (msg_joy.duracion == Larga)){
						msg_rda_mosi.comando = cmd_power_on;
						osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
						estado = manual;
					}
				}
			break;
			
			case manual:
				if(muestreo_hora(&hora, &min, &seg, &prev_sec) || muestreo_temp(&msg_temp, &prev_temp)){
					msg_rda_mosi.comando = cmd_get_info;
					osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
					osMessageQueueGet(get_id_MsgQueue_rda_miso(), &msg_rda_miso, NULL, 0U);
					
					sprintf(msg_lcd.data_L1, "%.2u:%.2u:%.2u - T:%.1fºC", hora, min, seg, msg_temp.temperature);
					sprintf(msg_lcd.data_L2, "F: %d.%d Vol: %d", (msg_rda_miso.frequency / 10), (msg_rda_miso.frequency % 10), msg_rda_miso.volume);
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
								
							break;
							
							case Izquierda:
						
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

static int muestreo_temp(MSGQUEUE_OBJ_TEMP* msg, float* prev_temp){
	if(osOK == osMessageQueueGet(get_id_MsgQueue_temp(), msg, NULL, 0U)){
		if(*prev_temp != msg->temperature){
			*prev_temp = msg->temperature;
			return(1);
		}
	}
	return(0);
}

static int muestreo_hora(uint8_t* h, uint8_t* m, uint8_t* s, uint8_t* last_sec){
	if(last_sec != s){
		last_sec = s;
		sec_to_multiple(sec, h, m, s);
		return(1);
	}
	return(0);
}

static void gestion_vol(void){
	MSGQUEUE_OBJ_VOL msg_vol;
	MSGQUEUE_OBJ_RDA_MOSI msg_rda_mosi;
	osMessageQueueGet(get_id_MsgQueue_vol(), &msg_vol, NULL, 0U);
	msg_rda_mosi.comando = cmd_set_vol;
	msg_rda_mosi.data = msg_vol.volume_lvl;
	osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
}
