#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

extern osThreadId_t tid_Thread_LED_3;                        // thread id

int Init_Thread_LED_3 (void); 
void Thread_LED_3 (void *argument);
