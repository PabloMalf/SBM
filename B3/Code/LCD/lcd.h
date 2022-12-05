#ifndef __LCD_H
#define __LCD_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_LCD 4

typedef enum{sup, inf} linea_t;

typedef struct {
	char data[32];
	linea_t linea;
} MSGQUEUE_OBJ_LCD;

int Init_Th_lcd(void);
int Init_Th_lcd_test(void);

osThreadId_t get_id_Th_lcd(void);
osMessageQueueId_t get_id_MsgQueue_lcd(void);

#endif
