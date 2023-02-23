#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

extern osThreadId_t tid_Th_Leds;                        // thread id

int Init_Th_Leds (void);
void Th_Leds (void *argument);


	/**
	
	LEYENDA DE FLAGS
	
		1 Led Verde (LED1)				3 Led Verde + Azul (LED1 + LED2)			7 Led Verde + Azul + Rojo (LED1 + LED2 + LED3)
		2 Led Azul  (LED2)				5 Led Verde + Rojo (LED1 + LED3)
		4 Led Rojo  (LED3)				6 Led Azul  + Rojo (LED2 + LED3)
		
	**/

