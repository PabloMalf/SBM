#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "principal.h" 

MSGQUEUE_OBJ_lcd_t msg_lcd;

MSGQUEUE_OBJ_joystick_t msg_joystick;


static osStatus_t status;
int cntArriba = 0;
int cntAbajo = 0;
int cntCentro = 0;
int cntCentro_L = 0;
int cntIzquierda = 0;
int cntDerecha = 0;

osThreadId_t tid_Th_principal;
 
void Th_principal (void *argument);
 
int Init_Th_principal (void) {
  tid_Th_principal = osThreadNew(Th_principal, NULL, NULL);
  if (tid_Th_principal == NULL) {
    return(-1);
  }
	
  return(0);
}
 
void Th_principal (void *argument) {
	
	while (Init_Th_lcd()){};
	while (Init_Th_joystick()){};
		
  while (1) {	
		
		
		
		
		/*PRUEBA JOYSTICK*/
		status = osMessageQueueGet(mid_MsgQueue_joystick, &msg_joystick, NULL, 0U);
    if (status == osOK) {
			if(msg_joystick.puls == Arriba){
				cntArriba ++;
			}
			if (msg_joystick.puls == Abajo){
				cntAbajo ++;
			}
			if(msg_joystick.puls == Izquierda){
				cntIzquierda ++;
			}
			if (msg_joystick.puls == Derecha){
				cntDerecha ++;
			}
			if (msg_joystick.puls == Centro){
				cntCentro ++;
			}
			if (msg_joystick.puls == Centro_L){
				cntCentro_L ++;
			}
		}
		/**/
		
		
		
		/*Prueba LCD
		strcpy(msg_lcd.linea_1, "12345678901234567");
		strcpy(msg_lcd.linea_2, "abcdefghijklmnopq");
		osMessageQueuePut(mid_MsgQueue_lcd, &msg_lcd, 0U, 0U);
		osDelay(1000);
		
		strcpy(msg_lcd.linea_1, "");
		strcpy(msg_lcd.linea_2, "");
		osMessageQueuePut(mid_MsgQueue_lcd, &msg_lcd, 0U, 0U);
		osDelay(1000);
		*/
    osThreadYield();                            // suspend thread
  }
}
