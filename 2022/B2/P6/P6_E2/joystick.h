#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "cmsis_os2.h"

int Init_Th_joystick(void);
osThreadId_t id_Th_joystick(void);

#endif
