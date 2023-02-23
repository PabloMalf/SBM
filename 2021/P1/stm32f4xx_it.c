#include "main.h"
#include "stm32f4xx_it.h"

#ifdef _RTE_
#include "RTE_Components.h"             /* Component selection */
#endif

extern uint32_t delay;
	
void EXTI15_10_IRQHandler (void);

void EXTI15_10_IRQHandler (void) {
	HAL_GPIO_EXTI_IRQHandler (GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin);

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin){
	if (GPIO_Pin == GPIO_PIN_13){
		delay = ((delay > 125) ? delay / 2 : 500);
		if (delay>125)
			delay = delay/2;
		else
			delay = 500;
		(delay>125)? (delay = delay / 2) :  (delay = 500);
	}
}
	
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
#ifndef RTE_CMSIS_RTOS2_RTX5
void SVC_Handler(void)
{
}
#endif

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
#ifndef RTE_CMSIS_RTOS2_RTX5
void PendSV_Handler(void)
{
}
#endif

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
#ifndef RTE_CMSIS_RTOS2_RTX5
void SysTick_Handler(void)
{
  HAL_IncTick();
}
#endif
