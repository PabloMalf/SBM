#include "temp.h"

osThreadId_t tid_Th_temp;                        // thread id

void Th_temp (void *argument);                   // thread function

int Init_Th_temp(void) {

  tid_Th_temp = osThreadNew(Th_temp, NULL, NULL);
  if (tid_Th_temp == NULL) {
    return(-1);
  }

  return(0);
}

void Th_temp (void *argument) {

  while (1) {
    ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}
