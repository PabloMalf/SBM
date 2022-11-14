#include "Thled3.h"

osThreadId_t tid_Thled3;

static void Th_Thled3(void *argument);
static void led_Init(void);
 
int Init_Th_Thled3(void){
  tid_Thled3 = osThreadNew(Th_Thled3, NULL, NULL);
  if (tid_Thled3 == NULL) {
    return(-1);
  }
  return(0);
}
 
static void Th_Thled3(void *argument){
	led_Init();
  while (1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
    osDelay(287);
  }
}

static void led_Init(void){
	static GPIO_InitTypeDef GPIO_InitStruct;
	
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}
