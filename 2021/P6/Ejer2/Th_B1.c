#include "Th_B1.h"
#include "Th_Leds.h"
 
osThreadId_t tid_Th_B1;                        // thread id
 
void Th_B1 (void *argument);                   // thread function

extern uint8_t cnt;
 
int Init_Th_B1(void) {
 
  tid_Th_B1 = osThreadNew(Th_B1, NULL, NULL);
  if (tid_Th_B1 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Th_B1 (void *argument) {
	//BOTON INIT
 	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();
  	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	cnt = 0;
	
  while (1) {
    ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}


