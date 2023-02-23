#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "Thled1.h"
#include "Thled2.h"

osThreadId_t tid_Thled1;

static void Th_led1(void *argument);
static void led_Init(void);

static uint8_t cnt = 1;
 
int Init_Th_led1(void){
  tid_Thled1 = osThreadNew(Th_led1, NULL, NULL);
  if (tid_Thled1 == NULL) {
    return(-1);
  }
  return(0);
}
 
static void Th_led1(void *argument){
	led_Init();
  while(1){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		osDelay(200);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		osDelay(800);
		if (cnt == 5){
			osThreadFlagsSet(id_Th_led2(), 0x01);			
		}
		cnt++;
  }
}

static void led_Init(void){
	static GPIO_InitTypeDef GPIO_InitStruct;
	
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}
