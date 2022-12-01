#include "pwm.h"
#include "stm32f4xx_hal.h"

static osThreadId_t id_pwm;
static osThreadId_t id_pwm_test;

static TIM_HandleTypeDef tim2;

static void Th_pwm(void *argument);
static void Th_pwm_test(void *argument);

static void conf_pins(void);

osThreadId_t get_id_Th_pwm(void){
	return id_pwm;
}

osThreadId_t get_id_Th_pwm_test(void){
	return id_pwm_test;
}

int Init_Th_pwm(void){
  id_pwm = osThreadNew(Th_pwm, NULL, NULL);
  if(id_pwm == NULL)
    return(-1);
  return(0);
}

static void Th_pwm(void *argument) {
	conf_pins();
	static uint32_t flags;
	while(1){
		flags = osThreadFlagsWait(FLAG_PWM, osFlagsWaitAny, osWaitForever);
		if(flags & FLAG_PWM){
			HAL_TIM_PWM_Start(&tim2 , TIM_CHANNEL_4);
		}
	}
}

static void conf_pins(void){
	static GPIO_InitTypeDef GPIO_InitStruct_pwm;
	static TIM_OC_InitTypeDef tim2OCconf;			
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct_pwm.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct_pwm.Pull = GPIO_PULLUP;
	GPIO_InitStruct_pwm.Speed = GPIO_SPEED_MEDIUM;
	GPIO_InitStruct_pwm.Alternate = GPIO_AF1_TIM2;
	GPIO_InitStruct_pwm.Pin = GPIO_PIN_3;	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_pwm);

	__HAL_RCC_TIM2_CLK_ENABLE();
	tim2.Instance = TIM2;
	tim2.Init.Prescaler = 8399; //84Mhz/8400 = 10kHz		
	tim2.Init.Period = 19; //10KHz/10Hz = 1000Hz   
	
	HAL_TIM_PWM_Init(&tim2);
	tim2OCconf.OCMode = TIM_OCMODE_PWM1;
	tim2OCconf.Pulse = 9;
	HAL_TIM_PWM_ConfigChannel(&tim2,&tim2OCconf,TIM_CHANNEL_4);
}

/*TEST*/
int Init_Th_pwm_test(void){
  id_pwm_test = osThreadNew(Th_pwm_test, NULL, NULL);
  if(id_pwm_test == NULL)
    return(-1);
  return(0);
}

static void Th_pwm_test(void *argument) {
	Init_Th_pwm();
	osThreadFlagsSet(id_pwm, FLAG_PWM);
}
