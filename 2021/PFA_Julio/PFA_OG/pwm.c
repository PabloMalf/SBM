#include "pwm.h"


GPIO_InitTypeDef GPIO_InitStruct_pwm;
TIM_OC_InitTypeDef htim2OCconf;			
TIM_HandleTypeDef htim2;


void Init_PWM(void){
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct_pwm.Mode = GPIO_MODE_AF_PP;//modo alternate del pull de salida
	GPIO_InitStruct_pwm.Pull = GPIO_PULLUP;//modo de subida:se ponen a high
	GPIO_InitStruct_pwm.Speed = GPIO_SPEED_MEDIUM;//modo de la frecuencia:se comprende para un rango de frecuencias medias
	GPIO_InitStruct_pwm.Alternate = GPIO_AF1_TIM2;//modo alternate del pin 3
	
	GPIO_InitStruct_pwm.Pin = GPIO_PIN_3;	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_pwm);
	
	
	htim2.Instance = TIM2;
	//El prescaler del TIM(2-4) es de 16 bits -- No puedes dividir entre numeros mayores de 65526--
	htim2.Init.Prescaler = 8399;//  84Mhz/8400= 10kHz		
	htim2.Init.Period = 19;// 10KHz/20Hz= 500Hz   
	
	__HAL_RCC_TIM2_CLK_ENABLE(); // habilita el reloj delperiférico del timer 2
	
	HAL_TIM_PWM_Init(&htim2);// configura el timer 2 en modo output compare (Para los valores del preescaler y el period definidos antes)
	htim2OCconf.OCMode = TIM_OCMODE_PWM1;//modo pwm  ("para generar niveles altos y bajos con anchos de pulso distintos")
	htim2OCconf.Pulse = 9;					// period * ciclo de trabajo = pulse  -> 20Hz*0.5 =10Hz
	HAL_TIM_PWM_ConfigChannel(&htim2,&htim2OCconf,TIM_CHANNEL_4);//para generar OC 
	HAL_TIM_PWM_Start(&htim2 , TIM_CHANNEL_4);//start el timer (trabajamos en modo interrupcion de salida por comparación)

	
}

//Timer tiempo de reproduccion de las canciones
osTimerId_t tim_zumb;                            // timer id
static uint32_t exec_zumb;                          // argument for the timer call back function

static void Timer_zumb_Callback (void const *arg) {
  // add user code here
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
}
// Create timers
int Init_Timer_zumb (void) {
  // Create one-shoot timer
  exec_zumb= 1U;
  tim_zumb = osTimerNew((osTimerFunc_t)&Timer_zumb_Callback, osTimerOnce, &exec_zumb, NULL);
	
  return NULL;
}


