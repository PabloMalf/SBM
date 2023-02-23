#include "joystick.h"

typedef struct {
  uint8_t Id;
} MSGQUEUE_OBJ_t;

osThreadId_t tid_Th_joystick;
osMessageQueueId_t mid_joystick;

static osTimerId_t Tmr_OneShot_50ms;
static osTimerId_t Tmr_OneShot_950ms;
static uint32_t flags;

int Init_Th_joystick(void) {

  tid_Th_joystick = osThreadNew(Th_joystick, NULL, NULL);
  if (tid_Th_joystick == NULL) {
    return(-1);
  }

  return(0);
}


int Init_Msg_joystick (void) {
 
  mid_joystick = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
  if (mid_joystick == NULL) {
    return (-1);
	}
 
  return(0);
}


void Th_joystick (void *argument) {
	
	MSGQUEUE_OBJ_t msg;
	Init_joystick();
	Init_Msg_joystick();
	Tmr_OneShot_50ms = osTimerNew(timer_one_shot_50ms_Callback, osTimerOnce, (void *)0, NULL);
	Tmr_OneShot_950ms = osTimerNew(timer_one_shot_950ms_Callback, osTimerOnce, (void *)0, NULL);
	
	while (1) {
		flags = osThreadFlagsWait(0x07U, osFlagsWaitAny, osWaitForever);
		if (((flags >> 1) & 0x01) == 1){ //Flag:  1   =   IRQ
			if(osTimerIsRunning(Tmr_OneShot_950ms)){
				osTimerDelete(Tmr_OneShot_950ms);
				msg.Id = 4U; //Central Corta
				osMessageQueuePut(mid_joystick, &msg, 0U, 0U);
			}else{
				osTimerStart(Tmr_OneShot_50ms, 50U);
			}
		}
		if (((flags >> 2) & 0x01) == 1){ //Flag:  2   =   Timer_50ms_CallBack
			osTimerDelete(Tmr_OneShot_50ms);
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)){ //Arriba                                      // do some work...
				msg.Id = 0U;
				osMessageQueuePut(mid_joystick, &msg, 0U, 0U);
			}
			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)){ //Derecha
				msg.Id = 1U;
				osMessageQueuePut(mid_joystick, &msg, 0U, 0U);
			}
			else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)){ //Abajo
				msg.Id = 2U;
				osMessageQueuePut(mid_joystick, &msg, 0U, 0U);
			}
			else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14)){ //Izquierda
				msg.Id = 3U;
				osMessageQueuePut(mid_joystick, &msg, 0U, 0U);
			}
			else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15)){ //Centro
				osTimerStart(Tmr_OneShot_950ms, 950U);
			}
		}
		if (((flags >> 3) & 0x01) == 1){ //Flag: 4   =   Timer_950ms_Callback
			msg.Id = 5U; //Central larga
			osMessageQueuePut(mid_joystick, &msg, 0U, 0U);
		}
    osThreadYield();                            
  }
}


void EXTI15_10_IRQHandler (void) {
	HAL_GPIO_EXTI_IRQHandler (GPIO_PIN_13);
}


void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin){
	if (GPIO_Pin == 13){
		osThreadFlagsSet(tid_Th_joystick, 0x01U);
	}
}


static void timer_one_shot_50ms_Callback (void* argument) {
  osThreadFlagsSet(tid_Th_joystick, 0x02U);
}


static void timer_one_shot_950ms_Callback (void* argument) {
  osThreadFlagsSet(tid_Th_joystick, 0x04U);
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
