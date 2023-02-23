#include "Th_Leds.h"

osThreadId_t tid_Th_Leds;                        // thread id

uint32_t flags;

void Init_Leds (void);

void Th_Leds (void *argument);                   // thread function

int Init_Th_Leds(void) {

  tid_Th_Leds = osThreadNew(Th_Leds, NULL, NULL);
  if (tid_Th_Leds == NULL) {
    return(-1);
  }

  return(0);
}

void Th_Leds (void *argument) {

	Init_Leds ();
	
  while (1) {

		flags = osThreadFlagsWait(0x07U, osFlagsWaitAny, osWaitForever);

		if (((flags << 31) >> 31) == 1){ //Flag: 1   =   VERDE
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		}
		if (((flags << 30) >> 31) == 1){ //Flag: 2   =   AZUL
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);;
		}
		if (((flags << 29) >> 31) == 1){ //Flag: 4   =   ROJO
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		}
    osThreadYield();                            // suspend thread
  }
}

void Init_Leds (void){
	GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14, GPIO_PIN_RESET);	
}


