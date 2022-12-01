#include "vol.h"
#include "stm32f4xx_hal.h"

static osThreadId_t id_vol;
static osThreadId_t id_vol_test;
static osMessageQueueId_t id_MsgQueue_vol;
static MSGQUEUE_OBJ_VOL msg;

static void Th_vol(void *argument);
static void Th_vol_test(void *argument);

osThreadId_t get_id_Th_vol(void){
	return id_vol;
}

osThreadId_t get_id_Th_vol_test(void){
	return id_vol_test;
}

osMessageQueueId_t get_id_MsgQueue_vol(void){
	return id_MsgQueue_vol;
}

static int Init_MsgQueue_vol(void){
  id_MsgQueue_vol = osMessageQueueNew(MSGQUEUE_OBJECTS_VOL, sizeof(MSGQUEUE_OBJ_VOL), NULL);
  if(id_MsgQueue_vol == NULL)
    return (-1); 
  return(0);
}

int Init_Th_vol(void){
  id_vol = osThreadNew(Th_vol, NULL, NULL);
  if(id_vol == NULL)
    return(-1);
  return(Init_MsgQueue_vol());
}

static void Th_vol(void *argument){
	while(1){

	}
}

/*TEST*/
int Init_Th_vol_test(void){
  id_vol_test = osThreadNew(Th_vol_test, NULL, NULL);
  if(id_vol_test == NULL)
    return(-1);
  return(0);
}

static void Th_vol_test(void *argument){
	while(1){

	}
}
