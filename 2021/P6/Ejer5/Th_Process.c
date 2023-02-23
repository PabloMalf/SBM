#include "Th_Process.h"
#include "Th_Leds.h"

typedef struct {                                // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t;

osMessageQueueId_t mid_Joystick;

osThreadId_t tid_Th_Process;                        // thread id

void Th_Process (void *argument);                   // thread function

int Init_Th_Process(void) {

  tid_Th_Process = osThreadNew(Th_Process, NULL, NULL);
  if (tid_Th_Process == NULL) {
    return(-1);
  }

  return(0);
}

void Th_Process (void *argument) {
	MSGQUEUE_OBJ_t msg;
  osStatus_t status;
  while (1) {
		osThreadFlagsWait(0x01U, osFlagsWaitAny, osWaitForever);
		status = osMessageQueueGet(mid_Joystick, &msg, NULL, osWaitForever);   // wait for message
    if (status == osOK) {
	if(msg.Idx = 1U){
		
      osThreadFlagsSet(tid_Th_Leds, 0x01U);
    }
    osThreadYield();                            // suspend thread
  }
}
