#include "main.h"
#include "stm32f4xx_it.h"

#ifdef _RTE_
#include "RTE_Components.h"
#endif

/* IRQs Perso */

extern uint16_t delay;

void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch(GPIO_Pin){
		case GPIO_PIN_13:
			delay = ((delay == 250) ? 1000 : (delay/2));
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

