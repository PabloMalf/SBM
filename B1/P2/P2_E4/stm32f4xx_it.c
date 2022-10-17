#include "main.h"
#include "stm32f4xx_it.h"

#ifdef _RTE_
#include "RTE_Components.h"
#endif

//static uint32_t t_0;
//static uint32_t t_x;
uint8_t ov = 0;
//Definicion de los registros y las variables
uint32_t reg_1, reg_2, galleta, freq;
//0 -> si es el primer flag; 1 -> si es el segundo flag.
uint8_t flag = 0;
int freq_tim3;

/* IRQs Perso */

extern TIM_HandleTypeDef tim3;
static uint8_t cnt_overflow;

typedef enum{first, second}state_t;
static state_t measures;

static uint32_t value1, value2, value3, diff, freq, a;
	
void TIM3_IRQHandler(void){
	HAL_TIM_IRQHandler(&tim3);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM3){
		cnt_overflow++;
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *timX){
	if((timX->Instance == TIM3) && (timX->Channel == HAL_TIM_ACTIVE_CHANNEL_3)){
		switch(measures){
			case first:
				cnt_overflow = 0;
				value1 = HAL_TIM_ReadCapturedValue(timX, TIM_CHANNEL_3);
				measures = second;
//				ov = 0;
//				reg_1 = HAL_TIM_ReadCapturedValue(&tim3, TIM_CHANNEL_3);
//				flag = 1;
			break;
			
			case second:
				value2 = HAL_TIM_ReadCapturedValue(timX, TIM_CHANNEL_3);
				value3 = value2 + (cnt_overflow * timX->Init.Period); 
				diff = value3 - value1;
				a = HAL_RCC_GetPCLK2Freq();
				freq = a / diff;
				measures = first;
			
//				reg_2 = HAL_TIM_ReadCapturedValue(&tim3, TIM_CHANNEL_3);
//				galleta = ((reg_2 + (ov*tim3.Init.Period)) - reg_1);
//				flag = 0;	
//				freq_tim3 = HAL_RCC_GetPCLK2Freq();
//				freq = freq_tim3 / galleta;
			break;
		}
	}
}
	

/* IRQs Perso */

void NMI_Handler(void){}

void HardFault_Handler(void){
  while(1){}
}

void MemManage_Handler(void){
  while(1){}
}

void BusFault_Handler(void){
  while(1){}
}

void UsageFault_Handler(void){  
  while(1){}
}

#ifndef RTE_CMSIS_RTOS2_RTX5
void SVC_Handler(void){}
#endif

void DebugMon_Handler(void){}

#ifndef RTE_CMSIS_RTOS2_RTX5
void PendSV_Handler(void){}
#endif

#ifndef RTE_CMSIS_RTOS2_RTX5
void SysTick_Handler(void){
  HAL_IncTick();
}
#endif
