#ifndef __VOL_H
#define __VOL_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_VOL 4

typedef struct {
	uint8_t volume_lvl; //0-15
} MSGQUEUE_OBJ_VOL;

osMessageQueueId_t get_id_MsgQueue_vol(void);

int Init_Th_vol(void);
int Init_Th_vol_test(void);

#endif

