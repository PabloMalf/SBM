#ifndef LCD_H
#define LCD_H

#include "Driver_SPI.h"  
#include "main.h"
#include "stdio.h"
#include "string.h"

void LCD_init(void);
void update_data(char data_L1[32], char data_L2[32]);

#endif
