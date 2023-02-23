#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "joystick.h"
#include "clock.h"
#include "temp.h"
#include "lcd.h"
#include "vol.h"
#include "mp3.h"
#include "com.h"
#include "pwm.h"


extern osThreadId_t tid_principal;                        // thread id

int Init_Th_principal (void);
void Th_principal (void *argument);

#endif
