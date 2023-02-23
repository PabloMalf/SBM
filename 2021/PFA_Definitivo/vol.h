#ifndef VOL_H
#define VOL_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

#define MSGQUEUE_OBJECTS_vol 16

typedef struct {
  uint8_t Vol;
} MSGQUEUE_OBJ_vol_t;

extern osThreadId_t tid_Th_vol;                        // thread id
extern osMessageQueueId_t mid_vol;

int Init_Th_vol (void);
void Th_vol (void *argument);

void ADC1_pins_F429ZI_config(void);
int ADC_Init_Single_Conversion(ADC_HandleTypeDef *, ADC_TypeDef *);
float ADC_getVoltage(ADC_HandleTypeDef * , uint32_t );
#endif
