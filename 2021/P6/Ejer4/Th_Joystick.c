#include "Th_Joystick.h"
#include "Th_Leds.h"
 
static osThreadId_t tid_Th_Joystick;                        // thread id
static osTimerId_t Tmr_OneShot;

static uint8_t cnt_Up;
static uint8_t cnt_Down;
static uint8_t cnt_Left;
static uint8_t cnt_Right;
static uint8_t cnt_Center;
 
void Th_Joystick (void *argument);                   // thread function
static void timer_one_shot_Callback (void *);

void int_to_bin (void);


void Init_Timer (void);
void Init_Joystick (void);
static uint32_t flags;
 

int Init_Th_Joystick(void) {
 
  tid_Th_Joystick = osThreadNew(Th_Joystick, NULL, NULL);
  if (tid_Th_Joystick == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Th_Joystick (void *argument) {
  Init_Joystick();
	Init_Timer();
	
	cnt_Up = 0;
	cnt_Down = 0;
	cnt_Left = 0;
	cnt_Right = 0;
	cnt_Center = 0;
	
  while (1) {
		flags = osThreadFlagsWait(0x03U, osFlagsWaitAny, osWaitForever);
		if (((flags >> 1) & 0x01) == 1){ //Flag:  1   =   IRQ
			osTimerStart(Tmr_OneShot, 50U);
		}
		if (((flags >> 2) & 0x01) == 1){ //Flag:  2   =   Timer_CallBack
			osTimerDelete(Tmr_OneShot);
			osThreadFlagsSet(tid_Th_Leds, 0x08U);
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)){ //Arriba
				cnt_Up++;
				osThreadFlagsSet(tid_Th_Leds, 0x01U);
			}
			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)){ //Derecha
				cnt_Right++;
				osThreadFlagsSet(tid_Th_Leds, 0x02U);
			}
			else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)){ //Abajo
				cnt_Down++;
				osThreadFlagsSet(tid_Th_Leds, 0x03U);
			}
			else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14)){ //Izquierda
				cnt_Left++;
				osThreadFlagsSet(tid_Th_Leds, 0x04U);
			}
			else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15)){ //Centro
				cnt_Center++;
				osThreadFlagsSet(tid_Th_Leds, 0x05U);
			}
		}
    osThreadYield();                            // suspend thread
  }
}


void Init_Joystick(void) {
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
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void Init_Timer(void){
	Tmr_OneShot = osTimerNew(timer_one_shot_Callback, osTimerOnce, (void *)0, NULL);     // (void*)0 is passed as an argument
	
}

static void timer_one_shot_Callback (void* argument) {
  osThreadFlagsSet(tid_Th_Joystick, 0x02U);
}

void int_to_bin (void){
	
}
