#include "com.h"
#include "stm32f4xx_hal.h"

static osThreadId_t id_com;
static osThreadId_t id_com_test;
static osMessageQueueId_t id_MsgQueue_com;
static MSGQUEUE_OBJ_COM msg;

static void Th_com(void *argument);
static void Th_com_test(void *argument);

osThreadId_t get_id_Th_com(void){
	return id_com;
}

osThreadId_t get_id_Th_com_test(void){
	return id_com_test;
}

osMessageQueueId_t get_id_MsgQueue_com(void){
	return id_MsgQueue_com;
}

static int Init_MsgQueue_com(void){
  id_MsgQueue_com = osMessageQueueNew(MSGQUEUE_OBJECTS_COM, sizeof(MSGQUEUE_OBJ_COM), NULL);
  if(id_MsgQueue_com == NULL)
    return (-1); 
  return(0);
}

int Init_Th_com(void){
  id_com = osThreadNew(Th_com, NULL, NULL);
  if(id_com == NULL)
    return(-1);
  return(Init_MsgQueue_com());
}

static void Th_com(void *argument){
	while(1){

	}
}

/*TEST*/
int Init_Th_com_test(void){
  id_com_test = osThreadNew(Th_com_test, NULL, NULL);
  if(id_com_test == NULL)
    return(-1);
  return(0);
}

static void Th_com_test(void *argument){
	while(1){

	}
}
