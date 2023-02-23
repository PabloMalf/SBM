#include "Th_NOMBRE.h"

osThreadId_t tid_Th_NOMBRE;                        // thread id

void Th_NOMBRE (void *argument);                   // thread function

int Init_Th_NOMBRE(void) {

  tid_Th_NOMBRE = osThreadNew(Th_NOMBRE, NULL, NULL);
  if (tid_Th_NOMBRE == NULL) {
    return(-1);
  }

  return(0);
}

void Th_NOMBRE (void *argument) {

  while (1) {
    ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}
