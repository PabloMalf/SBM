#include "led.h"

GPIO_InitTypeDef GPIO_InitStruct_led;
  
typedef struct {
	uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t;
MSGQUEUE_OBJ_t msg_led;

extern osMessageQueueId_t mid_MsgQueue_led;

osThreadId_t tid_Thread_led;

int Init_Thread_led (void) {
  tid_Thread_led = osThreadNew(Thread_led, NULL, NULL);
  if (tid_Thread_led== NULL) {
    return(-1);
  }
  return(0);
}


void Thread_led (void *argument) {
  while (1) {
		osMessageQueueGet(mid_MsgQueue_led, &msg_led, NULL, 0U);
		if (msg_led.Idx==2){
			RGB_azul();
		}
		if(msg_led.Idx==1){
			RGB_verde();
		}
  }
}

osTimerId_t tim_led;
static uint32_t exec_led;

static void Timer_led_Callback (void const *arg) {
}

int Init_Timer_led(void) {
  exec_led= 3U;
  tim_led= osTimerNew((osTimerFunc_t)&Timer_led_Callback, osTimerPeriodic, &exec_led, NULL);
  return NULL;
}

void Init_LEDS(void){
	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitStruct_led.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct_led.Pull = GPIO_PULLUP;
	
	GPIO_InitStruct_led.Pin = GPIO_PIN_11;//led azul
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct_led);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
	GPIO_InitStruct_led.Pin = GPIO_PIN_12;//led verde
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct_led);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	GPIO_InitStruct_led.Pin = GPIO_PIN_13;//led rojo
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct_led);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	
}


/*----------------------------------------------------------------------------
 *                  			Funciones para Colores
 *---------------------------------------------------------------------------*/
//Activos a nivel bajo 
void RGB_rojo(void){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
}
void RGB_verde(void){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	osDelay(250);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	osDelay(250);
}
void RGB_azul(void){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	osDelay(1000);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	osDelay(1000);
}

