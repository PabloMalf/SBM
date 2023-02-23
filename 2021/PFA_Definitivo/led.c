#include "led.h"

osThreadId_t tid_Th_led;                        // thread id
osMessageQueueId_t mid_MsgQueue_led;
static osStatus_t status;
static MSGQUEUE_OBJ_led_t msg;


void Th_led (void *argument);                   // thread function

int Init_Msg_led (void) {
  mid_MsgQueue_led = osMessageQueueNew(MSGQUEUE_OBJECTS_led, sizeof(MSGQUEUE_OBJ_led_t), NULL);
  if (mid_MsgQueue_led== NULL) {
    return (-1);
	}
  return(0);
}

int Init_Th_led(void) {

  tid_Th_led = osThreadNew(Th_led, NULL, NULL);
  if (tid_Th_led == NULL) {
    return(-1);
  }
	Init_Msg_led();
  return(0);
}

void Th_led (void *argument) {

  while (1) {
    status = osMessageQueueGet(mid_MsgQueue_led, &msg, NULL, 0U);
    if (status == osOK) {
			
		}
    osThreadYield();                            // suspend thread
  }
}
