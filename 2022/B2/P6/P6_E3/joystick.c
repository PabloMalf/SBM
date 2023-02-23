#include "joystick.h"
#include "stm32f4xx_hal.h"

#define REBOTE_FLAG		0x01
#define IRQ_FLAG			0x02

static osThreadId_t tid_joystick;
static osMessageQueueId_t mid_Queue_joy;
static QUEUE_OBJ_JOY msg;

static uint32_t flags;
static osTimerId_t Tmr_rebote;

static void Th_joystick(void *argument);
static void led_Init(void);
static void joy_Init(void);

int Init_MsgQueue_joy (void) {
  mid_Queue_joy = osMessageQueueNew(QUEUE_OBJECTS_JOY, sizeof(QUEUE_OBJ_JOY), NULL);
  if (mid_Queue_joy == NULL)
    return (-1); 
  return(0);
}

int Init_Th_joystick(void){
  tid_joystick = osThreadNew(Th_joystick, NULL, NULL);
  if (tid_joystick == NULL) {
    return(-1);
  }
  return(Init_MsgQueue_joy());
}

osThreadId_t id_Th_joystick(void){
	return tid_joystick;
}

osMessageQueueId_t id_Queue_joystick(void){
	return mid_Queue_joy;
}

static void tmr_rebote_Callback (void* argument) {
	osThreadFlagsSet(tid_joystick, REBOTE_FLAG);
}

void Th_joystick(void *argument){
	led_Init();
	joy_Init();
	Tmr_rebote = osTimerNew(tmr_rebote_Callback, osTimerOnce, (void *)0, NULL);
	while(1){
		flags = osThreadFlagsWait((REBOTE_FLAG | IRQ_FLAG), osFlagsWaitAny, osWaitForever);
		if(flags & IRQ_FLAG){
			osTimerStart(Tmr_rebote, 50U);
		}
		if(flags & REBOTE_FLAG){
			switch(msg.tecla){
				case Arriba:
					if(HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_10) == GPIO_PIN_SET){
						osMessageQueuePut(mid_Queue_joy, &msg, 0U, 0U);
					}
				break;
				
				case Derecha:
					if(HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_11) == GPIO_PIN_SET){
						osMessageQueuePut(mid_Queue_joy, &msg, 0U, 0U);
					}
				break;
				
				case Abajo:
					if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_12) == GPIO_PIN_SET){
						osMessageQueuePut(mid_Queue_joy, &msg, 0U, 0U);
					}
				break;
				
				case Izquierda:
					if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_14) == GPIO_PIN_SET){
						osMessageQueuePut(mid_Queue_joy, &msg, 0U, 0U);
					}
				break;
				
				case Centro:
					if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_15) == GPIO_PIN_SET){
						osMessageQueuePut(mid_Queue_joy, &msg, 0U, 0U);
					}
				break;
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
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}

static void joy_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch(GPIO_Pin){
		case GPIO_PIN_10: msg.tecla = Arriba;			break;
		case GPIO_PIN_11: msg.tecla = Derecha; 		break;
		case GPIO_PIN_12:	msg.tecla = Abajo;			break;
		case GPIO_PIN_14: msg.tecla = Izquierda;	break;
		case GPIO_PIN_15:	msg.tecla = Centro;			break;
	}
	osThreadFlagsSet(tid_joystick, IRQ_FLAG);
}
