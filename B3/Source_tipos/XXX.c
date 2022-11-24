#include "XXX.h"
#include "stm32f4xx_hal.h"

static osThreadId_t id_XXX;
static osMessageQueueId_t id_MsgQueue_XXX;
static MSGQUEUE_OBJ_XXX msg;

void Th_XXX(void *argument);

osThreadId_t get_id_Th_XXX(void){
	return id_XXX;
}

osMessageQueueId_t get_id_MsgQueue_XXX(void){
	return id_MsgQueue_XXX;
}

static int Init_MsgQueue_XXX(void){
  id_MsgQueue_XXX = osMessageQueueNew(MSGQUEUE_OBJECTS_XXX, sizeof(MSGQUEUE_OBJ_XXX), NULL);
  if(id_MsgQueue_XXX == NULL)
    return (-1); 
  return(0);
}

int Init_Th_XXX(void){
  id_XXX = osThreadNew(Th_XXX, NULL, NULL);
  if(id_XXX == NULL)
    return(-1);
  return(Init_MsgQueue_XXX());
}

void Th_XXX(void *argument) {
	while(1){

	}
}
