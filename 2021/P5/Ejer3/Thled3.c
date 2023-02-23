#include "Thled1.h"
#include "Thled2.h"
#include "Thled3.h"

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread_LED_3;                        // thread id
 
 
int Init_Thread_LED_3 (void) {
 
  tid_Thread_LED_3 = osThreadNew(Thread_LED_3, NULL, NULL);
  if (tid_Thread_LED_3 == NULL) {
    return(-1);
  }
 
  return(0);
}
 

void Thread_LED_3 (void *argument) {
	
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
	int ciclos = 1;
	osThreadFlagsWait(0x001U, osFlagsWaitAll, osWaitForever);
	
  while (1) {
    ; // Insert thread code here...
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
		osDelay(287);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
		osDelay(287);
		ciclos ++;
		if (ciclos == 25){
			osThreadFlagsSet(tid_Thread_LED_2, 0x001U);		
		}
		if (ciclos == 30){
			osThreadFlagsWait(0x001U, osFlagsWaitAll, osWaitForever);
			ciclos = 1;
		}
		
    osThreadYield();                            // suspend thread
 }
}

