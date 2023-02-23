#include "Thled2.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread_LED_2;                        // thread id
 
 
int Init_Thread_LED_2 (void) {
 
  tid_Thread_LED_2 = osThreadNew(Thread_LED_2, NULL, NULL);
  if (tid_Thread_LED_2 == NULL) {
    return(-1);
  }
 
  return(0);
}
 

void Thread_LED_2 (void *argument) {
	
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
  while (1) {
    ; // Insert thread code here...
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		osDelay(137);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		osDelay(137);
    osThreadYield();                            // suspend thread
  }
}
