/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_H
#define __PWM_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h" 
/* Exported functions ------------------------------------------------------- */
void Init_PWM(void);
static void Timer_zumb_Callback (void const *arg);
int Init_Timer_zumb(void);

#endif /* __PWM_H */

