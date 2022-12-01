#ifndef __pwm_H
#define __pwm_H

#include "cmsis_os2.h"

#define FLAG_PWM 0x01

int Init_Th_pwm(void);
int Init_Th_pwm_test(void);

osThreadId_t get_id_Th_pwm(void);
osThreadId_t get_id_Th_pwm_test(void);

#endif
