/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"  

/* Exported constants --------------------------------------------------------*/
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects

/* Exported functions ------------------------------------------------------- */
int Init_Thread_led (void) ;
void Thread_led (void *argument) ;
static void Timer_led_Callback (void const *arg);
int Init_Timer_led(void) ;
void Init_LEDS(void);
void RGB_rojo(void);
void RGB_verde(void);
void RGB_azul(void);


#endif /* __LED_H */

