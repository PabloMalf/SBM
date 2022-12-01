#ifndef __COM_H
#define __COM_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_COM 4

typedef struct {
	int a;
} MSGQUEUE_OBJ_COM;

int Init_Th_com(void);
int Init_Th_com_test(void);

osThreadId_t get_id_Th_com(void);
osMessageQueueId_t get_id_MsgQueue_com(void);

#endif
