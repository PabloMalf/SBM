#ifndef __VOL_H
#define __VOL_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_VOL 4

typedef struct {
	float voltage_level;
} MSGQUEUE_OBJ_VOL;

osThreadId_t get_id_Th_vol(void);
osMessageQueueId_t get_id_MsgQueue_vol(void);
int Init_Th_vol(void);

/*TEST*/
int Init_Th_vol_test(void);

#endif

