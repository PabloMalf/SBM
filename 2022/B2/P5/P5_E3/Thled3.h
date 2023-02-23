#ifndef __THLED3_H
#define __THLED3_H

#include "cmsis_os2.h"

#define FLAG_START_LED3 0x01

int Init_Th_led3(void);
osThreadId_t id_Th_led3(void);

#endif
