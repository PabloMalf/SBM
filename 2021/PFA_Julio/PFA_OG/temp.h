/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEMP_H
#define __TEMP_H


/* Includes ------------------------------------------------------------------*/
#include "Driver_I2C.h"
#include "cmsis_os2.h"  

/* Exported constants --------------------------------------------------------*/
#define I2C_ADDR       0x48      /*  I2C address */

/* Exported functions ------------------------------------------------------- */
int Init_Thread_temp (void);
void Thread_temp (void *argument);
static void Timer_temp_Callback (void const *arg);
int Init_Timer_temp(void) ;
void init_temp(void);
	

#endif /* __TEMP_H */

