#include "com.h"

osThreadId_t tid_Th_com;                        // thread id

void Th_com (void *argument);                   // thread function

int Init_Th_com(void) {

  tid_Th_com = osThreadNew(Th_com, NULL, NULL);
  if (tid_Th_com == NULL) {
    return(-1);
  }

  return(0);
}

void Th_com (void *argument) {

  while (1) {
    ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}
