#ifndef __COM_H
#define __COM_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_COM 4

typedef struct {
	char frame_Tx[12];	// in: traba de 12bytes de la radio	
	uint32_t hora;			// in
} MSGQUEUE_OBJ_COM_MOSI;

typedef struct{
	uint32_t hora;
} MSGQUEUE_OBJ_COM_MISO;

int Init_Th_com(void);
osMessageQueueId_t get_id_MsgQueue_com_mosi(void);
osMessageQueueId_t get_id_MsgQueue_com_miso(void);

/*TEST*/
int Init_Th_com_test(void);

#endif
