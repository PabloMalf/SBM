#ifndef __PRINCIPAL_H
#define __PRINCIPAL_H

#include "rda5807m.h"
#include "clock.h"
#include "com.h"
#include "joystick.h"
#include "lcd.h"
#include "pwm.h"
#include "temp.h"
#include "vol.h"
#include "rgb.h"

#define BUFFER_SIZE 16
#define TO_ERROR  3000 //(ms)

int Init_Th_principal(void);

#endif
