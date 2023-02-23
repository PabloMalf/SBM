#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

extern osThreadId_t tid_Th_temp;                        // thread id

int Init_Th_temp (void);
void Th_temp (void *argument);
