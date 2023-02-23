/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

/* Exported constants --------------------------------------------------------*/
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects

/* Includes ------------------------------------------------------------------*/
#include "Driver_SPI.h"  
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "cmsis_os2.h" 

/* Exported functions ------------------------------------------------------- */
void delay (uint32_t n_microsegundos);
void PIN_LCD(void);
void LCD_reset(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void LCD_init(void);
void LCD_update(void);
void symbolToLocalBuffer_L1(uint8_t symbol);
void symbolToLocalBuffer_L2(uint8_t symbol);
void Clean_LCD(void);
void symbolToLocalBuffer(uint8_t line,uint8_t symbol);
int Init_MsgQueue_lcd(void);
int Init_Thread_lcd (void);
void Thread_lcd (void *argument);

	
#endif /* __LCD_H */
