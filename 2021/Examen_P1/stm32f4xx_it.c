#include "main.h"
#include "stm32f4xx_it.h"

#ifdef _RTE_
#include "RTE_Components.h"             /* Component selection */
#endif



/****************************************/


extern int i;
extern TIM_HandleTypeDef htim7;

void TIM7_IRQHandler(void){
	HAL_TIM_IRQHandler(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIM7){
		i++;
		if(i == 3){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
			i = 0;
		} 
		else if(i == 1){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		}
	}
}



/****************************************/



void NMI_Handler(void){}

void HardFault_Handler(void){while (1){}}

void MemManage_Handler(void){while (1){}}

void BusFault_Handler(void){while (1){}}

void UsageFault_Handler(void){while (1){}}

#ifndef RTE_CMSIS_RTOS2_RTX5
void SVC_Handler(void){}
#endif

void DebugMon_Handler(void){}

#ifndef RTE_CMSIS_RTOS2_RTX5
void PendSV_Handler(void){}
#endif

#ifndef RTE_CMSIS_RTOS2_RTX5
void SysTick_Handler(void){HAL_IncTick();}
#endif
