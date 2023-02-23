/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VOL_H
#define __VOL_H

/* Exported constants --------------------------------------------------------*/
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
#define RESOLUTION_12B 4096U
#define VREF 3.3f

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"



/* Exported functions ------------------------------------------------------- */

void ADC1_pins_F429ZI_config(void);
int ADC_Init_Single_Conversion(ADC_HandleTypeDef *, ADC_TypeDef  *);
float ADC_getVoltage(ADC_HandleTypeDef * , uint32_t );
int Init_MsgQueue_vol(void);
int Init_Thread_vol (void);
void Thread_vol (void *argument);




#endif /* __VOL_H */


