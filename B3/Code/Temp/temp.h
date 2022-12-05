#ifndef __TEMP_H
#define __TEMP_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_TEMP 4

typedef struct {
	float temperature;
} MSGQUEUE_OBJ_TEMP;

int Init_Th_temp(void);
int Init_Th_temp_test(void);
osThreadId_t get_id_Th_temp(void);
osMessageQueueId_t get_id_MsgQueue_temp(void);

#endif