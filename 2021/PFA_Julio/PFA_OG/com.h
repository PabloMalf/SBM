/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COM_H
#define __COM_H

/* Exported constants --------------------------------------------------------*/
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects


/* Includes ------------------------------------------------------------------*/
#include "Driver_USART.h"
#include "cmsis_os2.h"                   /* ARM::CMSIS:RTOS:Keil RTX */
#include <stdio.h>
#include <string.h>

/* Exported functions ------------------------------------------------------- */
int Init_MsgQueue_pc(void) ;
void myUSART3_callback(uint32_t event);
int Init_myUSART3_Thread (void) ;//Init thread del pc
void myUSART3_Thread(void *argument);//Thread del pc
void pc(int h, int m, int s);
void Thread_cvol (void *argument) ;
int Init_Thread_cvol (void);

#endif /* __COM_H */

