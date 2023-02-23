#include "stm32f4xx_hal.h"
#include "Thled2.h"
#include "Thled3.h"
#include "cmsis_os2.h"

osThreadId_t tid_led2;

static void Th_led2(void *argument);
static void led_Init(void);

static uint8_t cnt = 0;
 
int Init_Th_led2(void){
  tid_led2 = osThreadNew(Th_led2, NULL, NULL);
  if (tid_led2 == NULL) {
    return(-1);
  }
  return(0);
}

osThreadId_t id_Th_led2(void){
	return tid_led2;
}

static void Th_led2(void *argument){
	led_Init();
	osThreadFlagsWait(FLAG_START_LED2, osFlagsWaitAll, osWaitForever);
  while(1){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		osDelay(137);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		osDelay(137);
		cnt ++;
		if(cnt == 15){
			osThreadFlagsSet(id_Th_led3(), FLAG_START_LED3);	
		}
		if(cnt == 20){
			osThreadFlagsWait(FLAG_START_LED2, osFlagsWaitAll, osWaitForever);
			cnt = 1;
		}
  }
}

static void led_Init(void){
	static GPIO_InitTypeDef GPIO_InitStruct;
	
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
}
