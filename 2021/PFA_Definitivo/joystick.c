#include "joystick.h"

osThreadId_t tid_Th_joystick;
osMessageQueueId_t mid_MsgQueue_joystick;
MSGQUEUE_OBJ_joystick_t msg;

static osTimerId_t Tmr_OneShot_50ms;
static osTimerId_t Tmr_OneShot_950ms;
static uint32_t flags;
static int bloqueado;

int Init_Th_joystick(void) {

  tid_Th_joystick = osThreadNew(Th_joystick, NULL, NULL);
  if (tid_Th_joystick == NULL) {
    return(-1);
  }
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	Init_joystick();
	Init_Msg_joystick();
  return(0);
}


int Init_Msg_joystick (void) {
 
  mid_MsgQueue_joystick = osMessageQueueNew(MSGQUEUE_OBJECTS_joystick, sizeof(MSGQUEUE_OBJ_joystick_t), NULL);
  if (mid_MsgQueue_joystick == NULL) {
    return (-1);
	}
 
  return(0);
}


void Th_joystick (void *argument) {
	
	Tmr_OneShot_50ms = osTimerNew(timer_one_shot_50ms_Callback, osTimerOnce, (void *)0, NULL);
	Tmr_OneShot_950ms = osTimerNew(timer_one_shot_950ms_Callback, osTimerOnce, (void *)0, NULL);

	while (1) {
		flags = osThreadFlagsWait(0x0FU, osFlagsWaitAny, osWaitForever);
		if (((flags >> 0) & 0x01) == 1){ //IRQ
			if (!osTimerIsRunning(Tmr_OneShot_50ms)){
				osTimerStart(Tmr_OneShot_50ms, 50U);
			}
		}
		if (((flags >> 1) & 0x01) == 1) { //CallBack Timer 50ms
			if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15)){
				osTimerStart(Tmr_OneShot_950ms, 950U);
			} else {
				if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)){
					msg.puls = Arriba;
				}
				else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)){
					msg.puls = Derecha;
				}
				else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)){
					msg.puls = Abajo;
				}
				else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14)){
					msg.puls = Izquierda;
				}
				osMessageQueuePut(mid_MsgQueue_joystick, &msg, 0U, 0U);
			}
		}
		if (((flags >> 2) & 0x01) == 1){
			if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15)){
				msg.puls = Centro_L;
			}
			else{
				msg.puls = Centro;
			}
			osMessageQueuePut(mid_MsgQueue_joystick, &msg, 0U, 0U);
		}
    osThreadYield();                            
  }
}


void EXTI15_10_IRQHandler (void) {
	HAL_GPIO_EXTI_IRQHandler (GPIO_PIN_10); //Arriba
	HAL_GPIO_EXTI_IRQHandler (GPIO_PIN_11); //Derecha
	HAL_GPIO_EXTI_IRQHandler (GPIO_PIN_12); //Abajo
	HAL_GPIO_EXTI_IRQHandler (GPIO_PIN_14); //Izquierda
	HAL_GPIO_EXTI_IRQHandler (GPIO_PIN_15); //Centro
}

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin){
	switch (GPIO_Pin){
		case GPIO_PIN_10: case GPIO_PIN_11: case GPIO_PIN_12: case GPIO_PIN_14: case GPIO_PIN_15:
			osThreadFlagsSet(tid_Th_joystick, 0x01U);
		break;
		default:
		break;
	}
}

static void timer_one_shot_50ms_Callback (void* argument) {
	osThreadFlagsSet(tid_Th_joystick, 0x02U);
}

static void timer_one_shot_950ms_Callback (void* argument) {
  osThreadFlagsSet(tid_Th_joystick, 0x04U);
}

static void timer_one_shot_bloqueo_Callback (void* argument) {
  osThreadFlagsSet(tid_Th_joystick, 0x08U);
}

void Init_joystick (void) {
	GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}
