#ifndef MP3_H
#define MP3_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "Driver_USART.h"

#define MSGQUEUE_OBJECTS_mp3 16

typedef struct {
  uint8_t Command;
} MSGQUEUE_OBJ_mp3_t;

extern osThreadId_t tid_Th_mp3;                        // thread id
extern osMessageQueueId_t mid_mp3;

extern ARM_DRIVER_USART Driver_USART3;

int Init_Th_mp3 (void);
void Th_mp3 (void *argument);
void Init_USART (void);
void myUSART_callback(uint32_t event);
#endif
