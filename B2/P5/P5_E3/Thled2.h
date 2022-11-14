#ifndef __THLED2_H
#define __THLED2_H

#include "cmsis_os2.h"

#define FLAG_START_LED2 0x01

int Init_Th_led2(void);
osThreadId_t id_Th_led2(void);

#endif
