#include "main.h"
#include "stm32f4xx_it.h"

#ifdef _RTE_
#include "RTE_Components.h"
#endif

/* IRQs Perso */
extern TIM_HandleTypeDef tim2;

void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch(GPIO_Pin){
		case GPIO_PIN_10: //Down
			HAL_TIM_OC_Stop(&tim2, TIM_CHANNEL_4);
			tim2.Init.Period = (tim2.Init.Period == 16) ? 1 : tim2.Init.Period * 2;
			HAL_TIM_OC_Init(&tim2);
			HAL_TIM_OC_Start(&tim2, TIM_CHANNEL_4);
		break;
		
		case GPIO_PIN_12: //Right
			
		break;
		
		case GPIO_PIN_13: //User
			HAL_TIM_OC_Stop(&tim2, TIM_CHANNEL_4);
			tim2.Init.Period = (tim2.Init.Period == 1) ? 4 : 1;
			HAL_TIM_OC_Init(&tim2);
			HAL_TIM_OC_Start(&tim2, TIM_CHANNEL_4);
		break;
		
		case GPIO_PIN_14: //Up
			HAL_TIM_OC_Stop(&tim2, TIM_CHANNEL_4);
			tim2.Init.Period = (tim2.Init.Period == 1) ? 16 : tim2.Init.Period / 2;
			HAL_TIM_OC_Init(&tim2);
			HAL_TIM_OC_Start(&tim2, TIM_CHANNEL_4);
		break;
		
		case GPIO_PIN_15: //Center
			
		break;
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
