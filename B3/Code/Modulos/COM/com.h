#ifndef __COM_H
#define __COM_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_COM 4

typedef enum{CMD_RDA, SET_TIME, LCD_to_PC, PC_CNTRL}comPC_t;

typedef struct {
	comPC_t comPC;
	char frame_Tx[12];
	uint32_t frame_time;
	uint32_t hora;
} MSGQUEUE_OBJ_COM;

int Init_Th_com(void);
osThreadId_t get_id_Th_com(void);
osMessageQueueId_t get_id_MsgQueue_com(void);

/*TEST*/
int Init_Th_com_test(void);

#endif
