#ifndef __LCD_H
#define __LCD_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_LCD 4

typedef struct {
	char data_L1[32];
	uint16_t init_L1;
	char data_L2[32];
	uint16_t init_L2;
} MSGQUEUE_OBJ_LCD;

int Init_Th_lcd(void);
osThreadId_t get_id_Th_lcd(void);
osMessageQueueId_t get_id_MsgQueue_lcd(void);

#endif
