#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

extern osThreadId_t tid_Th_B1;                        // thread id

int Init_Th_B1 (void); 
void Th_B1 (void *argument);
