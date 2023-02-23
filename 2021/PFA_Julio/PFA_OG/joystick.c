#include "joystick.h"
 extern int puls;
  
typedef struct {                                // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t;
MSGQUEUE_OBJ_t msg_joy;

extern osMessageQueueId_t mid_MsgQueue_joy;                // message queue id
 


/*----------------------------------------------
 *                 Timers
 *----------------------------------------------*/ 
extern osThreadId_t tid_Thread_principal; 
/*----------------------TIMER PULSACIONES LARGAS------------------------*/
/*----- Periodic Timer -----*/
int ciclo=0;//cuenta los ciclos hasta llegar a 20 para encontrar la pulsación larga de 1s
osTimerId_t tim_id2_joy;                            // timer id
static uint32_t exec2_joy;                          // argument for the timer call back function
// Periodic Timer Function
static void Timer2_joy_Callback (void const *arg) {
	if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_15) == GPIO_PIN_SET){//Pulsado centro
		ciclo++;
	}
	else{//ya considero que he llegado al segundo de la pulsación larga
		if( ciclo > 19 )
		{
			puls++;
			msg_joy.Idx = 6;//Identificador de la pulsación larga del centro
			osTimerStop(tim_id2_joy);//paro el temporizador
			ciclo = 0;//actualizo el ciclo oir si lo vuelvo a pulsar
			osThreadFlagsSet (tid_Thread_principal, 0x20);
			osMessageQueuePut(mid_MsgQueue_joy, &msg_joy, 0U, 0U);//pongo el mensaje a la cola
		}else {
			msg_joy.Idx = 5;//Identificador de la pulsación corta del centro
			osTimerStop(tim_id2_joy);//paro el temporizador
			ciclo = 0;
			osThreadFlagsSet (tid_Thread_principal, 0x20);
			osMessageQueuePut(mid_MsgQueue_joy, &msg_joy, 0U, 0U);//pongo el mensaje a la cola
		}
		
	}
}


/*----------------------TIMER PULSACIONES CORTAS------------------------*/
/*----- One-Shoot Timer -----*/
osTimerId_t tim_id1_joy;                            // timer id
static uint32_t exec1_joy;                          // argument for the timer call back function

// One-Shoot Timer Function
static void Timer1_joy_Callback (void const *arg) {//Función Callback para los rebotes en los que se entiende que se ha producido una pulsación.
  // add user code here
	if( HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_10) == GPIO_PIN_SET ){//Pulsado arriba
		msg_joy.Idx = 1;//Identificador de la pulsación corta de arriba
		osThreadFlagsSet (tid_Thread_principal, 0x20);
		osMessageQueuePut(mid_MsgQueue_joy, &msg_joy, 0U, 0U);//pongo el mensaje a la cola
	}
	if(HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_11) == GPIO_PIN_SET ){//Pulsado el derecho
		msg_joy.Idx = 2;//Identificador de la pulsación corta de derecha
		osThreadFlagsSet (tid_Thread_principal, 0x20);
		osMessageQueuePut(mid_MsgQueue_joy, &msg_joy, 0U, 0U);//pongo el mensaje a la cola
	}
	if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_12) == GPIO_PIN_SET ){//Pulsado abajo
		msg_joy.Idx = 3;//Identificador de la pulsación corta de abajo
		osThreadFlagsSet (tid_Thread_principal, 0x20);
		osMessageQueuePut(mid_MsgQueue_joy, &msg_joy, 0U, 0U);//pongo el mensaje a la cola
	}
	if( HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_14) == GPIO_PIN_SET){//Pulsado izquierda
		msg_joy.Idx = 4;//Identificador de la pulsación corta de izquierda
		osThreadFlagsSet (tid_Thread_principal, 0x20);
		osMessageQueuePut(mid_MsgQueue_joy, &msg_joy, 0U, 0U);//pongo el mensaje a la cola
	}
	if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_15) == GPIO_PIN_SET){//Pulsado centro
		osTimerStart(tim_id2_joy, 50U); //cuenta de 1s para la pulsacion larga teniendo en cuenta los 1s de los rebotes
	}
}

 
// Create timers
int Init_Timers_joy (void) {
  // Create one-shoot timer
  exec1_joy = 1U;
  tim_id1_joy = osTimerNew((osTimerFunc_t)&Timer1_joy_Callback, osTimerOnce, &exec1_joy, NULL);
	// Create periodic timer
	exec2_joy = 2U;
  tim_id2_joy = osTimerNew((osTimerFunc_t)&Timer2_joy_Callback, osTimerPeriodic, &exec2_joy, NULL);
	
  return NULL;
}



/*----------------------------------------------------------------------------
 *                                Thread Joy
 *---------------------------------------------------------------------------*/

osThreadId_t tid_Thread_joy;                        // thread id
 
int Init_Thread_joy (void) {
 
  tid_Thread_joy = osThreadNew(Thread_joy, NULL, NULL);
  if (tid_Thread_joy == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_joy (void *argument) {
	
  while (1) {
    osThreadFlagsWait (0x04, osFlagsWaitAny , osWaitForever);//me llega el flag de la interrupción en la que se ha producido una pulsación
		osTimerStart(tim_id1_joy, 50U); //cuenta de 50ms para los rebotes
		
		osThreadYield();                            // suspend thread
  }
}



/*----------------------------------------------------------------------------
 *                       Definición Joystick
 *---------------------------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStruct_joy;

void Init_Joystick(void){
	//JOYSTICK
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct_joy.Mode = GPIO_MODE_IT_RISING;	//Detecta los flancos de subida
	GPIO_InitStruct_joy.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct_joy.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStruct_joy.Pin = GPIO_PIN_11;//Derecho
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_joy);
	GPIO_InitStruct_joy.Pin = GPIO_PIN_10;//Arriba
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_joy);
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);		//Habilitar tratado e interrupciones de la HAL
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStruct_joy.Pin = GPIO_PIN_12;//Abajo
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct_joy);
	GPIO_InitStruct_joy.Pin = GPIO_PIN_14;//Izquierda
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct_joy);
	GPIO_InitStruct_joy.Pin = GPIO_PIN_15;//Centro
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct_joy);
	
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);		//Habilitar tratado e interrupciones de la HAL
}


/*----------------------------------------------------------------------------
 *                          Interrupción
 *---------------------------------------------------------------------------*/
void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10); //Arriba
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11); //Derecha
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12); //Abajo
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14); //Izquierda
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15); //Centro
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	osThreadFlagsSet (tid_Thread_joy, 0x04); //mando al thread el flag indicando que se ha producido cualquier tipo de pulsación
}

