/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __JOYSTICK_H
#define __JOYSTICK_H

/* Exported constants --------------------------------------------------------*/
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "cmsis_os2.h" 
#include "stm32f4xx_hal.h"

#ifdef _RTE_
#include "RTE_Components.h"             /* Component selection */
#endif

/* Exported functions ------------------------------------------------------- */
int Init_MsgQueue_joy (void);
static void Timer_joy_Callback (void const *arg);
static void Timer2_joy_Callback (void const *arg);
int Init_Timers_joy (void);
int Init_Thread_joy (void);
void Thread_joy (void *argument);
void Init_Joystick(void);
void Thread_rep (void *argument);
int Init_Thread_rep (void);

#endif /* __JOYSTICK_H */
