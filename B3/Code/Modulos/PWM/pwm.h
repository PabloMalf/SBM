#ifndef __PWM_H
#define __PWM_H

#include "cmsis_os2.h"

#define FLAG_SPK 0x01U

osThreadId_t get_id_Th_pwm(void);

int Init_Th_pwm(void);
int Init_Th_pwm_test(void);

#endif
