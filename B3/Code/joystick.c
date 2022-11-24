#include "joystick.h"
#include "stm32f4xx_hal.h"

#define REBOTE_FLAG 0X01
#define CICLOS_FLAG 0X02
#define IRQ_FLAG    0x04

static osThreadId_t id_Th_joystick;
static osThreadId_t id_Th_joystick_test;
static osMessageQueueId_t id_MsgQueue_joystick;
static MSGQUEUE_OBJ_JOY msg;

static osTimerId_t Tmr_rebote;
static osTimerId_t Tmr_ciclos;
static uint32_t flags;
static uint8_t valida = 0;
static uint8_t cnt = 0;
static uint8_t max_cnt = ((PULSACION_TIEMPO / 50) - 1);

void Th_joystick(void *argument);
static void conf_pins(void);
static uint8_t check_pins(void);

osThreadId_t get_id_Th_joystickstick(void){
	return id_Th_joystick;
}

osThreadId_t get_id_Th_joystickstick_test(void){
	return id_Th_joystick_test;
}

osMessageQueueId_t get_id_MsgQueue_joystick(void){
	return id_MsgQueue_joystick;
}

static int Init_MsgQueue_joy(void){
  id_MsgQueue_joystick = osMessageQueueNew(MSGQUEUE_OBJECTS_JOY, sizeof(MSGQUEUE_OBJ_JOY), NULL);
  if(id_MsgQueue_joystick == NULL)
    return (-1); 
  return(0);
}

int Init_Th_joystick(void){
  id_Th_joystick = osThreadNew(Th_joystick, NULL, NULL);
  if(id_Th_joystick == NULL)
    return(-1);
  return(Init_MsgQueue_joy());
}

int Init_Th_joystick_test(void){
  id_Th_joystick_test = osThreadNew(Th_joystick, NULL, NULL);
  if(id_Th_joystick_test == NULL)
    return(-1);
  return(0);
}

static void tmr_rebote_Callback (void* argument) {
	osThreadFlagsSet(id_Th_joystick, REBOTE_FLAG);
}

static void tmr_ciclos_Callback (void* argument) {
  osThreadFlagsSet(id_Th_joystick, CICLOS_FLAG);
}

void Th_joystick(void *argument) {
	conf_pins();
	Tmr_rebote = osTimerNew(tmr_rebote_Callback, osTimerOnce, (void *)0, NULL);
	Tmr_ciclos = osTimerNew(tmr_ciclos_Callback, osTimerPeriodic, (void *)0, NULL);
	while(1) {
		flags = osThreadFlagsWait((CICLOS_FLAG | REBOTE_FLAG | IRQ_FLAG), osFlagsWaitAny, osWaitForever);
		if(flags & IRQ_FLAG){
			osTimerStart(Tmr_rebote, 50U);
		}
		if(flags & REBOTE_FLAG){
			if(check_pins()){
				valida = 1;
				cnt = 0;
				osTimerStart(Tmr_ciclos, 50U);
			} 
		}
		if(flags & CICLOS_FLAG){
			if(cnt == max_cnt){
				cnt = 0;
				msg.duracion = Larga;
				osTimerStop(Tmr_ciclos);
				osMessageQueuePut(id_MsgQueue_joystick, &msg, 0U, 0U);
			}else if(check_pins()){
				cnt ++;
			} 
			else if(valida == 1){
				msg.duracion = Corta;
				osTimerStop(Tmr_ciclos);
				osMessageQueuePut(id_MsgQueue_joystick, &msg, 0U, 0U);
			}
		}
  }
}

void Th_joystick_test(void *argument) {
	;
}

void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	valida = 0;
	switch(GPIO_Pin){
		case GPIO_PIN_10:
			msg.tecla = Arriba;
		break;
		
		case GPIO_PIN_11:
			msg.tecla = Derecha;
		break;
		
		case GPIO_PIN_12:
			msg.tecla = Abajo;
		break;
		
		case GPIO_PIN_14:
			msg.tecla = Izquierda;
		break;
		
		case GPIO_PIN_15:
			msg.tecla = Centro;
		break;
	}
	osThreadFlagsSet(id_Th_joystick, IRQ_FLAG);
}

static uint8_t check_pins(void){
	if((HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_10) == GPIO_PIN_SET)|
		 (HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_11) == GPIO_PIN_SET)|
		 (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_12) == GPIO_PIN_SET)|
		 (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_14) == GPIO_PIN_SET)|
		 (HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_15) == GPIO_PIN_SET)){
		return(1);
	}
	return(0);
}

static void conf_pins(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE();
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
