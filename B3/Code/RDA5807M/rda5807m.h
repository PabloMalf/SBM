#ifndef __RDA_H
#define __RDA_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_RDA 4

typedef struct {
	int a;
} MSGQUEUE_OBJ_RDA;

int Init_Th_rda(void);
int Init_Th_rda_test(void);

osThreadId_t get_id_Th_rda(void);
osMessageQueueId_t get_id_MsgQueue_rda(void);

#endif
