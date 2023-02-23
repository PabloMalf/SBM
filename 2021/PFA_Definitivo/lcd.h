#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define MSGQUEUE_OBJECTS_lcd 16

typedef struct {
  char linea_1 [30];
  char linea_2 [30];
}MSGQUEUE_OBJ_lcd_t;

extern osThreadId_t tid_Th_lcd;
extern osMessageQueueId_t mid_MsgQueue_lcd;

int Init_Th_lcd(void);
void Th_lcd (void *argument);
int Init_MsgQueue_lcd (void);



void LCD_reset(void);
void LCD_init(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char data);
void LCD_update(void);
void symbolToLocalBuffer_L1(uint8_t symbol, int reset);
void symbolToLocalBuffer_L2(uint8_t symbol, int reset);
void symbolToLocalBuffer(uint8_t line, uint8_t symbol, int reset);
void limpiarPantalla(void);
void cargarMSG(MSGQUEUE_OBJ_lcd_t);





#endif
