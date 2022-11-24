#include "temp.h"
#include "stm32f4xx_hal.h"

static osThreadId_t id_temp;
static osMessageQueueId_t id_MsgQueue_temp;
static MSGQUEUE_OBJ_TEMP msg;

void Th_temp(void *argument);

osThreadId_t get_id_Th_temp(void){
	return id_temp;
}

osMessageQueueId_t get_id_MsgQueue_temp(void){
	return id_MsgQueue_temp;
}

static int Init_MsgQueue_temp(void){
  id_MsgQueue_temp = osMessageQueueNew(MSGQUEUE_OBJECTS_TEMP, sizeof(MSGQUEUE_OBJ_TEMP), NULL);
  if(id_MsgQueue_temp == NULL)
    return (-1); 
  return(0);
}

int Init_Th_temp(void){
  id_temp = osThreadNew(Th_temp, NULL, NULL);
  if(id_temp == NULL)
    return(-1);
  return(Init_MsgQueue_temp());
}

void Th_temp(void *argument) {
	while(1){

	}
}
