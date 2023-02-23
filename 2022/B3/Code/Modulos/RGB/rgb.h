#ifndef __RGB_H
#define __RGB_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_RGB 4

typedef struct {
	uint8_t pulse;
} MSGQUEUE_OBJ_RGB;

int Init_Th_rgb(void);
int Init_Th_rgb_test(void);
osMessageQueueId_t get_id_MsgQueue_rgb(void);

#endif
