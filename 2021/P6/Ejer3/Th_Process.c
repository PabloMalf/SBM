#include "Th_Process.h"
#include "Th_Leds.h"

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
	
	int8_t to = 0;
	
	osThreadFlagsSet(tid_Th_Leds, 0x01U); //Flag Verde
	
  while (1) {
		if(to == 0){
			if (osFlagsErrorTimeout == osThreadFlagsWait(0x01U, osFlagsWaitAny, 3000U)){
				osThreadFlagsSet(tid_Th_Leds, 0x05U); //Flag Verde + Rojo
				to = 1;
			} else {
				osThreadFlagsSet(tid_Th_Leds, 0x02U);
			}
		}
    osThreadYield();                            // suspend thread
  }
}
