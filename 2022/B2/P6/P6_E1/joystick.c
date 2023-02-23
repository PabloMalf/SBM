#include "joystick.h"
#include "stm32f4xx_hal.h"

#define REBOTE_FLAG		0x01
#define IRQ_FLAG			0x02
#define BLOCKED_FLAG	0x04

static osThreadId_t tid_joystick;

static uint8_t lock = 0;
static uint32_t flags;
static osTimerId_t Tmr_rebote;
static osTimerId_t Tmr_shutdown;

static void Th_joystick(void *argument);
static void led_Init(void);
static void joy_Init(void);

int Init_Th_joystick(void){
  tid_joystick = osThreadNew(Th_joystick, NULL, NULL);
  if (Th_joystick == NULL) {
    return(-1);
  }
  return(0);
}

osThreadId_t id_Th_joystick(void){
	return tid_joystick;
}

static void tmr_rebote_Callback (void* argument) {
	osThreadFlagsSet(tid_joystick, REBOTE_FLAG);
}

static void tmr_shutdown_Callback (void* argument) {
	osThreadFlagsSet(tid_joystick, BLOCKED_FLAG);
}

void Th_joystick(void *argument){
	led_Init();
	joy_Init();
	Tmr_rebote = osTimerNew(tmr_rebote_Callback, osTimerOnce, (void *)0, NULL);
	Tmr_shutdown = osTimerNew(tmr_shutdown_Callback, osTimerOnce, (void *)0, NULL);
	osTimerStart(Tmr_shutdown, 3000U);
	while(1){
		if(!lock){
			flags = osThreadFlagsWait((REBOTE_FLAG | IRQ_FLAG | BLOCKED_FLAG), osFlagsWaitAny, osWaitForever);
			if(flags & IRQ_FLAG){
				osTimerStart(Tmr_rebote, 50U);
			}
			if(flags & REBOTE_FLAG){
				if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET){
					osTimerStart(Tmr_shutdown, 3000U);
				}
			}
			if(flags & BLOCKED_FLAG) {
				lock = 1;
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
			}
		}
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
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}

static void joy_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	osThreadFlagsSet(tid_joystick, IRQ_FLAG);
}
