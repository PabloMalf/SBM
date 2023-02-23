#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

extern osThreadId_t tid_Th_NOMBRE;                        // thread id

int Init_Th_NOMBRE (void);
void Th_NOMBRE (void *argument);
