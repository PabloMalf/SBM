#ifndef LCD_H
#define LCD_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "Driver_SPI.h"
#include <string.h>
#include <stdio.h>

typedef struct {
	char data [20];
  uint8_t lane;//Num linea
} MSGQUEUE_OBJ_lcd_t;

#define MSGQUEUE_OBJECTS_lcd 16

extern osThreadId_t tid_Th_lcd;
extern osMessageQueueId_t mid_lcd;

int Init_Th_lcd (void);
void Th_lcd (void *argument);
#endif
