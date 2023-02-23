#ifndef __CLOCK_H
#define __CLOCK_H

#include "cmsis_os2.h"

extern uint32_t sec;

int Init_Th_clock(void);
int Init_Th_clock_test(void);

void set_clock(uint8_t hour, uint8_t minutes, uint8_t seconds);
void multiple_to_sec(uint32_t* sec, uint8_t  hour, uint8_t  minutes, uint8_t  seconds);
void sec_to_multiple(uint32_t  sec, uint8_t* hour, uint8_t* minutes, uint8_t* seconds);
#endif
