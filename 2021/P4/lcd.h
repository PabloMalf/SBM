#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"
#include "Driver_SPI.h"
#include "Arial12x12.h"
#include <string.h>
#include <stdio.h>

extern TIM_HandleTypeDef htim7;

extern GPIO_InitTypeDef GPIO_InitStruct;

extern ARM_DRIVER_SPI Driver_SPI1;
extern ARM_DRIVER_SPI* SPIdrv;
extern ARM_SPI_STATUS estado;

extern uint16_t posL1;
extern uint16_t posL2;
extern char cadena[80];

void delay (uint32_t);

void LCD_init(void);
void LCD_wr_cmd(unsigned char);
void LCD_wr_data(unsigned char);
void symbolToLocalBuffer(uint8_t, uint8_t);
void symbolToLocalBuffer_L1(uint8_t);
void symbolToLocalBuffer_L2(uint8_t);
void escribirEntero(int);
void escribirFloat(float);

void LCD_reset(void);
void LCD_update(void);

#endif
