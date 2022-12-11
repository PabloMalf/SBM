#include "pwm.h"
#include "stm32f4xx_hal.h"

static osThreadId_t id_Th_pwm;
static osThreadId_t id_Th_pwm_test;

static void Th_pwm(void *argument);
static void myPWM_Init(TIM_HandleTypeDef *htim);

/*TEST*/
static void Th_pwm_test(void *argument);
static void myInit_led(void);

osThreadId_t get_id_Th_pwm(void){
	return id_Th_pwm;
}

int Init_Th_pwm(void){
  id_Th_pwm = osThreadNew(Th_pwm, NULL, NULL);
  if(id_Th_pwm == NULL)
    return(-1);
  return(0);
}

static void Th_pwm(void *argument) {
	static TIM_HandleTypeDef htim2 = {0};
	static uint32_t xFlags;
	
	myPWM_Init(&htim2);
	
	while(1){
		xFlags = osThreadFlagsWait(FLAG_SPK, osFlagsWaitAny, osWaitForever);
		if(xFlags & FLAG_SPK){
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
			osDelay(20U);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
		}
	}
}

static void myPWM_Init(TIM_HandleTypeDef *htim){
  static GPIO_InitTypeDef sgpio = {0};
	static TIM_OC_InitTypeDef soc = {0};
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	sgpio.Pin= GPIO_PIN_3;
	sgpio.Mode= GPIO_MODE_AF_PP;
  sgpio.Alternate= GPIO_AF1_TIM2;
	sgpio.Pull= GPIO_NOPULL;
  sgpio.Speed= GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &sgpio);
	
	__HAL_RCC_TIM2_CLK_ENABLE();
	htim -> Instance= TIM2;
	htim -> Init.Prescaler= 83;
	htim -> Init.Period= 249;	// tono de 4kHz
	htim -> Init.CounterMode= TIM_COUNTERMODE_UP;
	htim -> Init.ClockDivision= TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(htim);
	
	soc.OCMode= TIM_OCMODE_PWM1;
	soc.OCPolarity= TIM_OCPOLARITY_HIGH;
	soc.OCFastMode= TIM_OCFAST_DISABLE;
	soc.Pulse= 24; // 10%
	HAL_TIM_PWM_ConfigChannel(htim, &soc, TIM_CHANNEL_4);
}

/* TEST */
int Init_Th_pwm_test(void){
	id_Th_pwm_test= osThreadNew(Th_pwm_test, NULL, NULL);
	if(id_Th_pwm_test == NULL)
		return(-1);
	return(0);
}

static void Th_pwm_test(void *arguments){
	
	Init_Th_pwm();
	myInit_led();
	
	while(1){
		osThreadFlagsSet(get_id_Th_pwm(), FLAG_SPK);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		osDelay(1000U);
	}
}

static void myInit_led(void){
	static GPIO_InitTypeDef sgpio = {0};
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	sgpio.Pin = GPIO_PIN_0;
	sgpio.Mode = GPIO_MODE_OUTPUT_PP;
	sgpio.Pull = GPIO_NOPULL;
	sgpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &sgpio);
}
