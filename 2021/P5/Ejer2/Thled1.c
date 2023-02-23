#include "Thled1.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'LED 1': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread_LED_1;                        // thread id
 
 
int Init_Thread_LED_1 (void) {
	
  tid_Thread_LED_1 = osThreadNew(Thread_LED_1, NULL, NULL);
  if (tid_Thread_LED_1 == NULL) {
    return(-1);
  }
 
  return(0);
}
 

void Thread_LED_1 (void *argument) {
	
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
  while (1) {
    ; // Insert thread code here...
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		osDelay(200);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		osDelay(800);
    osThreadYield();                            // suspend thread
  }
}
