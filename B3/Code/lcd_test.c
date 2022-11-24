#include "lcd_test.h"
#include "cmsis_os2.h"
#include "lcd.h"
#include "stdio.h"

static osThreadId_t tid_lcd_test;
static MSGQUEUE_OBJ_LCD msg_lcd;

void Th_lcd_test(void *argument);

int Init_Th_lcd_test(void){
  tid_lcd_test = osThreadNew(Th_lcd_test, NULL, NULL);
  if(tid_lcd_test == NULL)
    return(-1);
  return(Init_Th_lcd());
}

void Th_lcd_test(void *argument) {
	uint8_t cnt = 0;
	while(1){
		msg_lcd.init_L1 = 7;
		msg_lcd.init_L2 = 15;
		sprintf(msg_lcd.data_L1, "Patatitas con queso");
		sprintf(msg_lcd.data_L2, "Hola Segismundo %d", cnt);
		osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd, NULL, 0U);
		cnt++;
		osDelay(1000);
		
		msg_lcd.init_L1 = 7;
		msg_lcd.init_L2 = 10;
		sprintf(msg_lcd.data_L1, "Patatitas con bacon");
		sprintf(msg_lcd.data_L2, "Adios Segismundo %d", cnt);
		osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd, NULL, 0U);
		cnt++;
		osDelay(1000);
	}
}
