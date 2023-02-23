#define LCD_H
#ifdef LCD_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>

extern osThreadId_t tid_Th_lcd;

int Init_Th_lcd(void);
void Th_lcd (void *argument);

void LCD_reset(void);
void LCD_init(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char data);
void LCD_update(void);
void symbolToLocalBuffer_L1(uint8_t symbol, int reset);
void symbolToLocalBuffer_L2(uint8_t symbol, int reset);
void symbolToLocalBuffer(uint8_t line, uint8_t symbol, int reset);
void escribirEntero(int entero);
void escribirFloat(float flotante);

extern unsigned char buffer[512];
// ------------------ Extras ------------------
void limpiarPantalla(void);
void escribirLCD(void);
void escribirFrase(char frase[]);
void moverCaracter(char symbol);


#endif
