#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

extern osThreadId_t tid_Th_Leds;                        // thread id

int Init_Th_Leds (void);
void Th_Leds (void *argument);


	/**
	
	LEYENDA DE FLAGS
	
		1 Led Verde (LED1)
		2 Led Azul  (LED2)
		4 Led Rojo  (LED3)
		
	**/

