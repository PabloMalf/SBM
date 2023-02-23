#ifndef __XXX_H
#define __XXX_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_XXX 4

typedef struct {
	;
} MSGQUEUE_OBJ_XXX;

int Init_Th_XXX(void);
osThreadId_t get_id_Th_XXX(void);
osMessageQueueId_t get_id_MsgQueue_XXX(void);

#endif
