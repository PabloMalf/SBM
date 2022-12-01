#include "rda5807m.h"
#include "stm32f4xx_hal.h"

static osThreadId_t id_rda;
static osThreadId_t id_rda_test;
static osMessageQueueId_t id_MsgQueue_rda;
static MSGQUEUE_OBJ_RDA msg;

static void Th_rda(void *argument);
static void Th_rda_test(void *argument);

osThreadId_t get_id_Th_rda(void){
	return id_rda;
}

osThreadId_t get_id_Th_rda_test(void){
	return id_rda_test;
}

osMessageQueueId_t get_id_MsgQueue_rda(void){
	return id_MsgQueue_rda;
}

static int Init_MsgQueue_rda(void){
  id_MsgQueue_rda = osMessageQueueNew(MSGQUEUE_OBJECTS_RDA, sizeof(MSGQUEUE_OBJ_RDA), NULL);
  if(id_MsgQueue_rda == NULL)
    return (-1); 
  return(0);
}

int Init_Th_rda(void){
  id_rda = osThreadNew(Th_rda, NULL, NULL);
  if(id_rda == NULL)
    return(-1);
  return(Init_MsgQueue_rda());
}

static void Th_rda(void *argument){
	while(1){

	}
}

/*TEST*/
int Init_Th_rda_test(void){
  id_rda_test = osThreadNew(Th_rda_test, NULL, NULL);
  if(id_rda_test == NULL)
    return(-1);
  return(0);
}

static void Th_rda_test(void *argument){
	while(1){

	}
}
