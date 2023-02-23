#ifndef __COM_H
#define __COM_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_COM 4

typedef enum{CMD_RDA, SET_TIME, LCD_to_PC, PC_CNTRL}comPC_t;

typedef struct {
	comPC_t comPC; 	// tipo para gestion del update 
	char frame_Tx[12];	// in: traba de 12bytes de la radio	
	uint32_t hora;			// in/out 
} MSGQUEUE_OBJ_COM;

int Init_Th_com(void);
osMessageQueueId_t get_id_MsgQueue_com(void);

/*TEST*/
int Init_Th_com_test(void);

#endif
