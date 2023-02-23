#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

extern osThreadId_t tid_Th_Leds;                        // thread id

int Init_Th_Leds (void);
void Th_Leds (void *argument);
