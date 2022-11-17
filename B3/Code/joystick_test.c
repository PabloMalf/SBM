#include "joystick_test.h"
#include "cmsis_os2.h"
#include "joystick.h"

static osThreadId_t tid_joystick_test;
static MSGQUEUE_OBJ_JOY msg_joy;

void Th_joystick_test(void *argument);

int Init_Th_joystick_test(void){
  tid_joystick_test = osThreadNew(Th_joystick_test, NULL, NULL);
  if(tid_joystick_test == NULL)
    return(-1);
  return(0);
}

void Th_joystick_test(void *argument) {
	while(1){
		if(osMessageQueueGet(id_MsgQueue_joystick(), &msg_joy, NULL, 0U) == osOK){
			;
		}
	}
}

