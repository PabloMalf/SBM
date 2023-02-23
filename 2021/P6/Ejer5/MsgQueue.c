#include "cmsis_os2.h"                          // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Message Queue creation & usage
 *---------------------------------------------------------------------------*/
 
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
 
typedef struct {                                // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t;

MSGQUEUE_OBJ_t msg;

osMessageQueueId_t mid_MsgQueue;                // message queue id

int Init_MsgQueue (void) {
 
  mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
  if (mid_MsgQueue == NULL) {
    ; // Message Queue object not created, handle failure
	}
 
  return(0);
}

 


 