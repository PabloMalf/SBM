#include "principal.h"
#include "cmsis_os2.h"
#include <stdio.h>

typedef enum {reposo, manual, memori, prog_h} estados_t;

typedef struct{
	estados_t estado;
	uint8_t hora, min, seg, prev_sec;
	
	osStatus_t state_vol;
	osStatus_t state_tem;
	osStatus_t state_rda_miso;
	
	MSGQUEUE_OBJ_COM msg_com;
	MSGQUEUE_OBJ_JOY msg_joy;
	MSGQUEUE_OBJ_LCD msg_lcd;
	MSGQUEUE_OBJ_RDA_MOSI msg_rda_mosi;
	MSGQUEUE_OBJ_RDA_MISO msg_rda_miso;
	MSGQUEUE_OBJ_TEMP msg_temp;
	MSGQUEUE_OBJ_VOL msg_vol;
	MSGQUEUE_OBJ_RGB msg_rgb;
}info_principal_t;

extern uint32_t sec;

static void f_reposo(info_principal_t*);
static void f_manual(info_principal_t*);
static void f_memori(info_principal_t*);
static void f_prog_h(info_principal_t*);

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
					//Init_Th_pwm() | 
					Init_Th_rda() | 
					Init_Th_temp() | 
					Init_Th_vol() |
					Init_Th_rgb());
}

static void Th_principal(void *argument){
	static info_principal_t info;
	info.estado = reposo;
	info.prev_sec = 79; //Numero aleatorio para que no sea igual al estado inicial del reloj

	while(1){
		info.state_vol = osMessageQueueGet(get_id_MsgQueue_vol(), &info.msg_vol, NULL, 0U);
		info.state_tem = osMessageQueueGet(get_id_MsgQueue_temp(), &info.msg_temp, NULL, 0U);
		info.state_rda_miso = osMessageQueueGet(get_id_MsgQueue_rda_miso(), &info.msg_rda_miso, NULL, 0U);
		switch(info.estado){
			case reposo:
				f_reposo(&info);
			break;
			
			case manual:
				f_manual(&info);
			break;
			
			case memori:
				f_memori(&info);
			break;
			
			case prog_h:
				f_prog_h(&info);
			break;
		}
		osThreadYield();
	}
}

static void f_reposo(info_principal_t* i){
	if((i->prev_sec != sec) | (i->state_tem == osOK)){
		i->prev_sec = sec;
		sec_to_multiple(sec, &i->hora, &i->min, &i->seg);
		sprintf(i->msg_lcd.data_L1, " SBM 2022  T:%.1fºC", i->msg_temp.temperature);
		sprintf(i->msg_lcd.data_L2, "      %.2u:%.2u:%.2u", i->hora, i->min, i->seg);
		osMessageQueuePut(get_id_MsgQueue_lcd(), &i->msg_lcd, NULL, 0U);
	}
	if(i->state_vol == osOK){
		i->msg_rgb.pulse = i->msg_vol.volume_lvl;
		osMessageQueuePut(get_id_MsgQueue_rgb(), &i->msg_rgb, NULL, 0U);
		
		i->msg_rda_mosi.comando = cmd_set_vol;
		i->msg_rda_mosi.data = i->msg_vol.volume_lvl;
		osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &i->msg_rda_mosi, NULL, 0U);
	}

	if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &i->msg_joy, NULL, 0U)){
		osThreadFlagsSet(get_id_Th_pwm(), FLAG_SPK);
		if((i->msg_joy.tecla == Centro) && (i->msg_joy.duracion == Larga)){
			i->msg_rda_mosi.comando = cmd_power_on;
			osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &i->msg_rda_mosi, NULL, 0U);
			//osMessageQueueGet(get_id_MsgQueue_rda_miso(), &<i->msg_rda_miso, NULL, osWaitForever);
			
//			i->msg_rda_mosi.comando = cmd_set_vol;
//			i->msg_rda_mosi.data = i->msg_vol.volume_lvl;
//			osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &i->msg_rda_mosi, NULL, 0U);
			//osMessageQueueGet(get_id_MsgQueue_rda_miso(), &i->msg_rda_miso, NULL, osWaitForever);
			
			i->msg_rda_mosi.comando = cmd_set_freq;
			i->msg_rda_mosi.data = 980;
			osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &i->msg_rda_mosi, NULL, 0U);
			//osStatus_t b= osMessageQueueGet(get_id_MsgQueue_rda_miso(), &i->msg_rda_miso, NULL, osWaitForever);
			i->estado = manual;
		}
	}
}

static void f_manual(info_principal_t* i){
	if((i->prev_sec != sec)  | (i->state_vol == osOK)){
		i->prev_sec = sec;
		sec_to_multiple(sec, &i->hora, &i->min, &i->seg);
		sprintf(i->msg_lcd.data_L1, "  %.2u:%.2u:%.2u - T:%.1fºC", i->hora, i->min, i->seg, i->msg_temp.temperature);
		sprintf(i->msg_lcd.data_L2, "    F:%d.%d  Vol:%d", (i->msg_rda_miso.frequency / 10), (i->msg_rda_miso.frequency % 10), i->msg_vol.volume_lvl);
		//sprintf(i->msg_lcd.data_L2, "    F:%.1lf  Vol:%d", i->msg_rda_miso.frequency/1000, i->msg_vol.volume_lvl);
		osMessageQueuePut(get_id_MsgQueue_lcd(), &i->msg_lcd, NULL, 0U);
	}
	if(i->state_rda_miso == osOK){
		//MENSAJE COM
		; 
	}
	if(i->state_vol == osOK){
		i->msg_rgb.pulse = i->msg_vol.volume_lvl;
		osMessageQueuePut(get_id_MsgQueue_rgb(), &i->msg_rgb, NULL, 0U);
		i->msg_rda_mosi.comando = cmd_set_vol;
		i->msg_rda_mosi.data = i->msg_vol.volume_lvl;
		osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &i->msg_rda_mosi, NULL, 0U);
	}
	if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &i->msg_joy, NULL, 0U)){
		osThreadFlagsSet(get_id_Th_pwm(), FLAG_SPK);
		if(i->msg_joy.duracion == Corta){
			switch (i->msg_joy.tecla){
				case Arriba:
					i->msg_rda_mosi.comando = cmd_seek_up;
					osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &i->msg_rda_mosi, NULL, 0U);
				break;
				
				case Derecha:
					i->msg_rda_mosi.comando = cmd_next100kHz;
					osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &i->msg_rda_mosi, NULL, 0U);
				break;
				
				case Abajo:
					i->msg_rda_mosi.comando = cmd_seek_down;
					osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &i->msg_rda_mosi, NULL, 0U);
				break;
				
				case Izquierda:
					i->msg_rda_mosi.comando = cmd_prev100kHz;
					osMessageQueuePut(get_id_MsgQueue_rda_mosi(), &i->msg_rda_mosi, NULL, 0U);
				break;
				
				case Centro:
					//NADA
				break;
			}
			osMessageQueueGet(get_id_MsgQueue_rda_miso(), &i->msg_rda_miso, NULL, osWaitForever);
		}
		else if((i->msg_joy.duracion == Larga) && (i->msg_joy.tecla == Centro)){
			i->estado = memori;
		}
	}
}

static void f_memori(info_principal_t* i){
	while(i->estado == memori){
		if(i->prev_sec != sec){
			i->prev_sec = sec;
			sec_to_multiple(sec, &i->hora, &i->min, &i->seg);
			
			osMessageQueueGet(get_id_MsgQueue_temp(), &i->msg_temp, NULL, 0U);
			
			sprintf(i->msg_lcd.data_L1, "  %.2u:%.2u:%.2u - T:%.1fºC", i->hora, i->min, i->seg, i->msg_temp.temperature);
			sprintf(i->msg_lcd.data_L2, "Mem:%d  F:%d.%d  Vol:%d", 16, (i->msg_rda_miso.frequency / 10), (i->msg_rda_miso.frequency % 10), i->msg_rda_miso.volume);
			osMessageQueuePut(get_id_MsgQueue_lcd(), &i->msg_lcd, NULL, 0U);
		}
		if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &i->msg_joy, NULL, 0U)){
			osThreadFlagsSet(get_id_Th_pwm(), FLAG_SPK);
			if(i->msg_joy.duracion == Corta){
				switch (i->msg_joy.tecla){
					case Arriba:
						
					break;
					
					case Derecha:
					
					break;
					
					case Abajo:
					
					break;
					
					case Izquierda:
					
					break;
					
					case Centro:
						//NADA
					break;
				}
			}
			else if((i->msg_joy.duracion == Larga) && (i->msg_joy.tecla == Centro)){
				i->estado = prog_h;
			}
		}	
	}
}

static void f_prog_h(info_principal_t* i){
	typedef enum{hor_d, hor_u, min_d, min_u, seg_d, seg_u, bor}selector_t;
	static selector_t selec;
	
	uint8_t on = 0;
	uint8_t borrado = 0;
	
	sec_to_multiple(sec, &i->hora, &i->min, &i->seg);
	
	uint8_t d_hor_u = i->seg % 10;
	uint8_t d_hor_d = i->seg / 10;
	uint8_t d_min_u = i->seg % 10;
	uint8_t d_min_d = i->seg / 10;
	uint8_t d_seg_u = i->seg % 10;
	uint8_t d_seg_d = i->seg / 10;
	
	sprintf(i->msg_lcd.data_L1, "   Programacion Hora");
	
	while(i->estado == prog_h){
		if(osOK == osMessageQueueGet(get_id_MsgQueue_joystick(), &i->msg_joy, NULL, 0U)){
			osThreadFlagsSet(get_id_Th_pwm(), FLAG_SPK);
			if((i->msg_joy.duracion == Larga) && (i->msg_joy.tecla == Centro)){
				set_clock ((d_hor_d * 10 + d_hor_u), (d_min_d * 10 + d_min_u), (d_seg_d * 10 + d_seg_u));
				if(borrado){
					//PENDIENTE BORRAR
				}
				i->estado = reposo;
			}
			if(i->msg_joy.duracion == Corta){
				switch (i->msg_joy.tecla){
					case Centro: 
						//NADA
					break;
					
					case Derecha:
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
			sprintf(i->msg_lcd.data_L2, "     %d%d:%d%d:%d%d - B:%d", d_hor_d, d_hor_u, d_min_d, d_min_u, d_seg_d, d_seg_u, borrado);
			on = 0;
		}
		else{
			switch(selec){
				case hor_d:
					sprintf(i->msg_lcd.data_L2, "      %d:%d%d:%d%d - B:%d", d_hor_u, d_min_d, d_min_u, d_seg_d, d_seg_u, borrado);
				break;
				
				case hor_u:
					sprintf(i->msg_lcd.data_L2, "     %d :%d%d:%d%d - B:%d", d_hor_d, d_min_d, d_min_u, d_seg_d, d_seg_u, borrado);
				break;
				
				case min_d:
					sprintf(i->msg_lcd.data_L2, "     %d%d: %d:%d%d - B:%d", d_hor_d, d_hor_u, d_min_u, d_seg_d, d_seg_u, borrado);
				break;
				
				case min_u:
					sprintf(i->msg_lcd.data_L2, "     %d%d:%d :%d%d - B:%d", d_hor_d, d_hor_u, d_min_d, d_seg_d, d_seg_u, borrado);
				break;
				
				case seg_d:
					sprintf(i->msg_lcd.data_L2, "     %d%d:%d%d: %d - B:%d", d_hor_d, d_hor_u, d_min_d, d_min_u, d_seg_u, borrado);
				break;
				
				case seg_u:
					sprintf(i->msg_lcd.data_L2, "     %d%d:%d%d:%d  - B:%d", d_hor_d, d_hor_u, d_min_d, d_min_u, d_seg_d, borrado);
				break;
				
				case bor:
					sprintf(i->msg_lcd.data_L2, "     %d%d:%d%d:%d%d - B: ", d_hor_d, d_hor_u, d_min_d, d_min_u, d_seg_d, d_seg_u);
				break;
			}
			on = 1;
		}
		osMessageQueuePut(get_id_MsgQueue_lcd(), &i->msg_lcd, NULL, 0U);
		osDelay(500);
	}
}
