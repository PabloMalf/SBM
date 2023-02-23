#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "principal.h" 

osThreadId_t tid_Th_principal;                        // thread id
 
void Th_principal (void *argument);                   // thread function
 
int Init_Th_principal (void) {
  tid_Th_principal = osThreadNew(Th_principal, NULL, NULL);
  if (tid_Th_principal == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Th_principal (void *argument) {
 
	MSGQUEUE_OBJ_lcd_t msg_out_lcd;
	strcpy(msg_out_lcd.data, " prueba ");
	msg_out_lcd.lane = 1;
	osMessageQueuePut(mid_lcd, &msg_out_lcd, 0U, 0U); 
	
	
  while (1) {
		;
    osThreadYield();                            // suspend thread
  }
}
