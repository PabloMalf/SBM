#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

extern osThreadId_t tid_Th_Joystick;                        // thread id

int Init_Th_Joystick (void); 
void Th_Joystick (void *argument);


	/**
	
	LEYENDA DE FLAGS
	
		 1 Arriba
		 2 Derecha
		 4 Abajo
		 8 Izquierda
		16 Centro
		
	**/