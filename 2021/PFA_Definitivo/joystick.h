#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

#define MSGQUEUE_OBJECTS_joystick 16

typedef enum {Arriba = 0, Derecha, 
							Abajo, Izquierda,
							Centro, Centro_L
} pulsaciones_t;


typedef struct {
  pulsaciones_t puls;
} MSGQUEUE_OBJ_joystick_t;

extern osThreadId_t tid_Th_joystick;
extern osMessageQueueId_t mid_MsgQueue_joystick;

int Init_Th_joystick (void);
void Th_joystick (void *argument);
int Init_Msg_joystick (void);

void EXTI15_10_IRQHandler (void);
void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin);
void Init_joystick (void);
static void timer_one_shot_50ms_Callback (void* argument);
static void timer_one_shot_950ms_Callback (void* argument);
#endif
