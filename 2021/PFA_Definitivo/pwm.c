#include "pwm.h"

osThreadId_t tid_Th_pwm;                        // thread id

void Th_pwm (void *argument);                   // thread function

int Init_Th_pwm(void) {

  tid_Th_pwm = osThreadNew(Th_pwm, NULL, NULL);
  if (tid_Th_pwm == NULL) {
    return(-1);
  }

  return(0);
}

void Th_pwm (void *argument) {

  while (1) {
			
    osThreadYield();                            // suspend thread
  }
}
