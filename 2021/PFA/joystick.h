#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

#define MSGQUEUE_OBJECTS 16

extern osThreadId_t tid_Th_joystick;
extern osMessageQueueId_t mid_joystick;

int Init_Th_joystick (void);
void Th_joystick (void *argument);

void EXTI15_10_IRQHandler (void);
void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin);
void Init_joystick (void);
static void timer_one_shot_50ms_Callback (void* argument);
static void timer_one_shot_950ms_Callback (void* argument);
#endif
