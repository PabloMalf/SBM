#define LCD_H
#ifdef LCD_H

#include "stm32f4xx_hal.h"

void LCD_reset(void);
void LCD_init(void);
void update_data(char data_L1[32], char data_L2[32]);

static void LCD_wr_data(unsigned char data);
static void LCD_wr_cmd(unsigned char cmd);
static void LCD_update(void);
static void delay(uint32_t n_microsegundos);
static void symbolToLocalBuffer(uint8_t lane, uint8_t symbol);
static void symbolToLocalBuffer_L1(uint8_t symbol);
static void symbolToLocalBuffer_L2(uint8_t symbol);


#endif
