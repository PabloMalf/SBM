#include "principal.h"
#include "cmsis_os2.h"
#include <stdio.h>

typedef float buf_info[BUFFER_SIZE];

typedef struct{
	buf_info info;
	uint8_t head;
	uint8_t size;
}buffer_t;

typedef enum {reposo, manual, memori, prog_h} estados_t;

static buffer_t buf;
static	estados_t estado;
static	uint8_t hora, min, seg, prev_sec;
static	osStatus_t status_temp;
static	MSGQUEUE_OBJ_COM_MISO msg_com_miso;
static	MSGQUEUE_OBJ_COM_MOSI msg_com_mosi;
static	MSGQUEUE_OBJ_JOY msg_joy;
static	MSGQUEUE_OBJ_LCD msg_lcd;
static	MSGQUEUE_OBJ_RDA_MOSI msg_rda_mosi;
static	MSGQUEUE_OBJ_RDA_MISO msg_rda_miso;
static	MSGQUEUE_OBJ_TEMP msg_temp;
static	MSGQUEUE_OBJ_VOL msg_vol;
static	MSGQUEUE_OBJ_RGB msg_rgb;

extern uint32_t sec;

static void gestion (void);
static void f_reposo(void);
static void f_manual(void);
static void f_memori(void);
static void f_prog_h(void);
static void shift_array(void);
static void parsec_com(void);

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
	estado = reposo;
	prev_sec = 79; //Numero aleatorio para que no sea igual al estado inicial del reloj
	buf.head = 0;
	buf.size = 0;
	osThreadYield();
	while(1){
		gestion();
		switch(estado){
			case reposo:
				f_reposo();
			break;
			
			case manual:
				f_manual();
			break;
			
			case memori:
				f_memori();
			break;
			
			case prog_h:
				f_prog_h();
			break;
		}
		osThreadYield();
	}
}
static void gestion(){
	if(osOK == osMessageQueueGet(get_id_MsgQueue_com_miso(), &msg_com_miso, NULL, 0U)){
		static uint8_t a,b,c;
		sec_to_multiple(msg_com_miso.hora, &a, &b, &c);
		set_clock(a, b, c);
	}
	if(osOK == osMessageQueueGet(get_id_MsgQueue_vol(), &msg_vol, NULL, osWaitForever)){
		osMessageQueueGet(get_id_MsgQueue_temp(), &msg_temp, NULL, osWaitForever);
		
		msg_rgb.pulse = msg_vol.volume_lvl;
		osMessageQueuePut(get_id_MsgQueue_rgb(), &msg_rgb, NULL, 0U);
		
		msg_rda_mosi.comando = cmd_set_vol;
		msg_rda_mosi.vol = msg_vol.volume_lvl;
		osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
	}
}

static void f_reposo(){
	if((prev_sec != sec) | (status_temp == osOK)){
		prev_sec = sec;
		sec_to_multiple(sec, &hora, &min, &seg);
		sprintf(msg_lcd.data_L1, " SBM 2022  T:%.1fºC", msg_temp.temperature);
		sprintf(msg_lcd.data_L2, "       %.2u:%.2u:%.2u", hora, min, seg);
		osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd, NULL, 0U);
	}
	if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &msg_joy, NULL, 0U)){
		osThreadFlagsSet(get_id_Th_pwm(), FLAG_SPK);
		if((msg_joy.tecla == Centro) && (msg_joy.duracion == Larga)){
			msg_rda_mosi.comando = cmd_power_on;
			osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
			
			msg_rda_mosi.comando = cmd_set_vol;
			msg_rda_mosi.vol = msg_vol.volume_lvl;
			osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
			
			msg_rda_mosi.comando = cmd_get_info;
			osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);

			estado = manual;
		}
	}
}

static void f_manual(void){
	if(prev_sec != sec){
		prev_sec = sec;
		sec_to_multiple(sec, &hora, &min, &seg);
		
		osMessageQueueGet(get_id_MsgQueue_rda_miso(), &msg_rda_miso, NULL, 0U);
		parsec_com();
		msg_com_mosi.hora = sec;
		osMessageQueuePut(get_id_MsgQueue_com_mosi(), &msg_com_mosi, NULL, 0U);
		
		sprintf(msg_lcd.data_L1, "  %.2u:%.2u:%.2u - T:%.1fºC", hora, min, seg, msg_temp.temperature);
		sprintf(msg_lcd.data_L2, "    F:%.1lf  Vol:%d%d", msg_rda_miso.freq/1000, ((msg_vol.volume_lvl+1) / 10), ((msg_vol.volume_lvl+1) % 10));
		osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd, NULL, 0U);
	}
	if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &msg_joy, NULL, 0U)){
		osThreadFlagsSet(get_id_Th_pwm(), FLAG_SPK);
		if(msg_joy.duracion == Corta){
			switch (msg_joy.tecla){
				case Arriba:
					msg_rda_mosi.comando = cmd_seek_up;
					osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
				break;
				
				case Derecha:
					msg_rda_mosi.comando = cmd_next100kHz;
					osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
				break;
				
				case Abajo:
					msg_rda_mosi.comando = cmd_seek_down;
					osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
				break;
				
				case Izquierda:
					msg_rda_mosi.comando = cmd_prev100kHz;
					osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
				break;
				
				case Centro:
					//NADA
				break;
			}
		}
		else if((msg_joy.duracion == Larga) && (msg_joy.tecla == Centro)){
			estado = memori;
		}
	}	
}

static void shift_array(void){
	static int j;
	for(j = 0; j < ((buf.size - 1) - buf.head); j++){
		buf.info[buf.size - j] = buf.info[buf.size - j - 1];
	}
}

static void f_memori(void){
	if(prev_sec != sec){
		prev_sec = sec;
		sec_to_multiple(sec, &hora, &min, &seg);
		
		osMessageQueueGet(get_id_MsgQueue_rda_miso(), &msg_rda_miso, NULL, 0U);
		parsec_com();
		msg_com_mosi.hora = sec;
		osMessageQueuePut(get_id_MsgQueue_com_mosi(), &msg_com_mosi, NULL, 0U);
		
		sprintf(msg_lcd.data_L1, "  %.2u:%.2u:%.2u - T:%.1fºC", hora, min, seg, msg_temp.temperature);
		if((msg_rda_miso.freq / 1000) > 99.9){
			if(buf.size == 0){
				sprintf(msg_lcd.data_L2, ".M:%d%d/%d%d  F:%.1lf  Vol:%d%d", buf.head / 10, buf.head % 10, buf.size / 10, buf.size % 10, msg_rda_miso.freq / 1000, ((msg_vol.volume_lvl+1) / 10), ((msg_vol.volume_lvl+1) % 10));
			}
			else{
				sprintf(msg_lcd.data_L2, ".M:%d%d/%d%d  F:%.1lf  Vol:%d%d", (buf.head + 1) / 10, (buf.head + 1) % 10, buf.size / 10, buf.size % 10, msg_rda_miso.freq / 1000, ((msg_vol.volume_lvl+1) / 10), ((msg_vol.volume_lvl+1) % 10));
			}
		}
		else{
			if(buf.size == 0){
				sprintf(msg_lcd.data_L2, ".M:%d%d/%d%d  F: %.1lf  Vol:%d%d", buf.head / 10, buf.head % 10, buf.size / 10, buf.size % 10, msg_rda_miso.freq / 1000, ((msg_vol.volume_lvl+1) / 10), ((msg_vol.volume_lvl+1) % 10));
			}
			else{
				sprintf(msg_lcd.data_L2, ".M:%d%d/%d%d  F: %.1lf  Vol:%d%d", (buf.head + 1) / 10, (buf.head + 1) % 10, buf.size / 10, buf.size % 10, msg_rda_miso.freq / 1000, ((msg_vol.volume_lvl+1) / 10), ((msg_vol.volume_lvl+1) % 10));
			}
		}
		osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd, NULL, 0U);
	}
	
	if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &msg_joy, NULL, 0U)){
		osThreadFlagsSet(get_id_Th_pwm(), FLAG_SPK);
		if(msg_joy.duracion == Corta){
			switch (msg_joy.tecla){
				case Arriba:
					msg_rda_mosi.comando = cmd_seek_up;
					osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
				break;
				
				case Abajo:
					if(buf.size == 0){
						buf.info[buf.head] = msg_rda_miso.freq;
						buf.size++;
					}
					else if(buf.size == BUFFER_SIZE){
						if(buf.head == (BUFFER_SIZE - 1)){
							buf.head = 0;
						}
						else{
							buf.head++;
						}
						buf.info[buf.head] = msg_rda_miso.freq;
					}
					else if(buf.head < (buf.size - 1)){
						shift_array();
						buf.head++;
						buf.info[buf.head] = msg_rda_miso.freq;
						buf.size++;
					}
					else{
						buf.head++;
						buf.info[buf.head] = msg_rda_miso.freq;
						buf.size++;
					}
				break;
				
				case Derecha:
					if(buf.size != 0){
						if(buf.head < (buf.size - 1)){
							buf.head++;
						}
						else{
							buf.head = 0;
						}
						msg_rda_mosi.comando = cmd_set_freq;
						msg_rda_mosi.freq = ((buf.info[buf.head] / 1000) + 0.05);
						osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
					}
				break;

				case Izquierda:
					if(buf.size != 0){
						if(buf.head == 0){
							buf.head = (buf.size - 1);
						}
						else{
							buf.head--;
						}
						msg_rda_mosi.comando = cmd_set_freq;
						msg_rda_mosi.freq = ((buf.info[buf.head] / 1000) + 0.05);
						osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
					}
				break;
				
				case Centro:
					//NADA
				break;
			}
		}
		else if((msg_joy.duracion == Larga) && (msg_joy.tecla == Centro)){
			estado = prog_h;
		}
	}	
}

static void f_prog_h(void){
	typedef enum{hor_d, hor_u, min_d, min_u, seg_d, seg_u, bor}selector_t;
	static selector_t selec;
	
	uint8_t on = 0;
	uint8_t borrado = 0;
	
	uint8_t d_hor_u = hora % 10;
	uint8_t d_hor_d = hora / 10;
	uint8_t d_min_u = min  % 10;
	uint8_t d_min_d = min  / 10;
	uint8_t d_seg_u = seg  % 10;
	uint8_t d_seg_d = seg  / 10;
	
	sprintf(msg_lcd.data_L1, "   Programacion Hora");
	
	while(estado == prog_h){
		if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &msg_joy, NULL, 0U)){
			osThreadFlagsSet(get_id_Th_pwm(), FLAG_SPK);
			if((msg_joy.duracion == Larga) && (msg_joy.tecla == Centro)){
				set_clock ((d_hor_d * 10 + d_hor_u), (d_min_d * 10 + d_min_u), (d_seg_d * 10 + d_seg_u));
				if(borrado){
						buf.head = 0;
						buf.size = 0;
				}
				msg_rda_mosi.comando = cmd_power_off;
				osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &msg_rda_mosi, NULL, 0U);
				estado = reposo;
			}
			if(msg_joy.duracion == Corta){
				switch (msg_joy.tecla){	
					case Derecha: case Centro:
						selec += (selec < bor) ? 1 : 0;
					break;
					
					case Izquierda:
						selec -= (selec > hor_d) ? 1 : 0;
					break;
					
					case Arriba: 
						switch(selec){
							case hor_d:
								d_hor_d += (d_hor_d < 2) ? 1 : -d_hor_d; 
							break;
							
							case hor_u:
								d_hor_u += (d_hor_u < 3) ? 1 : -d_hor_u;
							break;
							
							case min_d:
								d_min_d += (d_min_d < 5) ? 1 : -d_min_d;
							break;
							
							case min_u:
								d_min_u += (d_min_u < 9) ? 1 : -d_min_u;
							break;
							
							case seg_d:
								d_seg_d += (d_min_d < 5) ? 1 : -d_seg_d;
							break;
							
							case seg_u:
								d_seg_u += (d_seg_u < 9) ? 1 : -d_seg_u;
							break;
							
							case bor:
								borrado = (borrado) ? 0 : 1;
							break;
						}
					break;
					
					case Abajo:
						switch(selec){
							case hor_d:
								d_hor_d -= (d_hor_d > 0) ? 1 : -2; 
							break;
							
							case hor_u:
								d_hor_u -= (d_hor_u > 0) ? 1 : -3;
							break;
							
							case min_d:
								d_min_d -= (d_min_d > 0) ? 1 : -5;
							break;
							
							case min_u:
								d_min_u -= (d_min_u > 0) ? 1 : -9;
							break;
							
							case seg_d:
								d_seg_d -= (d_min_d > 0) ? 1 : -5;
							break;
							
							case seg_u:
								d_seg_u -= (d_seg_u > 0) ? 1 : -9;
							break;
							
							case bor:
								borrado = (borrado) ? 0 : 1;
							break;
						}
					break;
				}
			}
		}
		if(on){
			sprintf(msg_lcd.data_L2, "     %d%d:%d%d:%d%d - B:%d", d_hor_d, d_hor_u, d_min_d, d_min_u, d_seg_d, d_seg_u, borrado);
			on = 0;
		}
		else{
			switch(selec){
				case hor_d:
					sprintf(msg_lcd.data_L2, "      %d:%d%d:%d%d - B:%d", d_hor_u, d_min_d, d_min_u, d_seg_d, d_seg_u, borrado);
				break;
				
				case hor_u:
					sprintf(msg_lcd.data_L2, "     %d :%d%d:%d%d - B:%d", d_hor_d, d_min_d, d_min_u, d_seg_d, d_seg_u, borrado);
				break;
				
				case min_d:
					sprintf(msg_lcd.data_L2, "     %d%d: %d:%d%d - B:%d", d_hor_d, d_hor_u, d_min_u, d_seg_d, d_seg_u, borrado);
				break;
				
				case min_u:
					sprintf(msg_lcd.data_L2, "     %d%d:%d :%d%d - B:%d", d_hor_d, d_hor_u, d_min_d, d_seg_d, d_seg_u, borrado);
				break;
				
				case seg_d:
					sprintf(msg_lcd.data_L2, "     %d%d:%d%d: %d - B:%d", d_hor_d, d_hor_u, d_min_d, d_min_u, d_seg_u, borrado);
				break;
				
				case seg_u:
					sprintf(msg_lcd.data_L2, "     %d%d:%d%d:%d  - B:%d", d_hor_d, d_hor_u, d_min_d, d_min_u, d_seg_d, borrado);
				break;
				
				case bor:
					sprintf(msg_lcd.data_L2, "     %d%d:%d%d:%d%d - B: ", d_hor_d, d_hor_u, d_min_d, d_min_u, d_seg_d, d_seg_u);
				break;
			}
			on = 1;
		}
		osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd, NULL, 0U);
		osDelay(500);
	}
}

static void parsec_com(void){
	msg_com_mosi.frame_Tx[0] = msg_rda_miso.reg[0] >> 8;
	msg_com_mosi.frame_Tx[1] = msg_rda_miso.reg[0] & 0x00FF;
	msg_com_mosi.frame_Tx[2] = msg_rda_miso.reg[1] >> 8;
	msg_com_mosi.frame_Tx[3] = msg_rda_miso.reg[1] & 0x00FF;
	msg_com_mosi.frame_Tx[4] = msg_rda_miso.reg[2] >> 8;
	msg_com_mosi.frame_Tx[5] = msg_rda_miso.reg[2] & 0x00FF;
	msg_com_mosi.frame_Tx[6] = msg_rda_miso.reg[3] >> 8;
	msg_com_mosi.frame_Tx[7] = msg_rda_miso.reg[3] & 0x00FF;
	msg_com_mosi.frame_Tx[8] = msg_rda_miso.reg[4] >> 8;
	msg_com_mosi.frame_Tx[9] = msg_rda_miso.reg[4] & 0x00FF;
	msg_com_mosi.frame_Tx[10]= msg_rda_miso.reg[5] >> 8;
	msg_com_mosi.frame_Tx[11]= msg_rda_miso.reg[5] & 0x00FF;
}
