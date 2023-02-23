#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

extern osThreadId_t tid_Th_pwm;                        // thread id

int Init_Th_pwm (void);
void Th_pwm (void *argument);
