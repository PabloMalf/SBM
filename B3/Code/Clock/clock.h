#ifndef __CLOCK_H
#define __CLOCK_H

#include "cmsis_os2.h"

uint8_t hours;
uint8_t minutes;
uint8_t seconds;

int Init_Th_clock(void);
int Init_Th_clock_test(void);
osThreadId_t get_id_Th_clock(void);

void set_clock(uint8_t hour, uint8_t minutes, uint8_t seconds);

#endif
