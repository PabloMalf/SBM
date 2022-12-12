#include "rgb.h"
#include "stm32f4xx_hal.h"
#include "math.h"

#define RELAC_VOL_PULSE 6.6f

static osThreadId_t id_Th_rgb;
static osMessageQueueId_t id_MsgQueue_rgb;
static osThreadId_t id_Th_rgb_test;

void Th_rgb(void *argument);
static void myPWM_Init(GPIO_InitTypeDef *sgpio, TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc);
static void myPWM_New_Pulse(TIM_HandleTypeDef htim, TIM_OC_InitTypeDef soc, MSGQUEUE_OBJ_RGB msg);

/*TEST*/
void Th_rgb_test(void*arg);

osThreadId_t get_id_Th_rgb(void){
	return id_Th_rgb;
}

osMessageQueueId_t get_id_MsgQueue_rgb(void){
	return id_MsgQueue_rgb;
}

static int Init_MsgQueue_rgb(void){
  id_MsgQueue_rgb = osMessageQueueNew(MSGQUEUE_OBJECTS_RGB, sizeof(MSGQUEUE_OBJ_RGB), NULL);
  if(id_MsgQueue_rgb == NULL)
    return (-1); 
  return(0);
}

int Init_Th_rgb(void){
  id_Th_rgb = osThreadNew(Th_rgb, NULL, NULL);
  if(id_Th_rgb == NULL)
    return(-1);
  return(0);
}

void Th_rgb(void *argument) {
	static MSGQUEUE_OBJ_RGB msg;
	static GPIO_InitTypeDef sgpio;
	static TIM_HandleTypeDef htim4;
	static TIM_OC_InitTypeDef soc;
	
	Init_MsgQueue_rgb();
	myPWM_Init(&sgpio, &htim4, &soc);
	HAL_TIM_PWM_ConfigChannel(&htim4, &soc, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	
	while(1){
		if(osMessageQueueGet(id_MsgQueue_rgb, &msg, NULL, 0U) == osOK)
			myPWM_New_Pulse(htim4, soc, msg);
	}
}

static void myPWM_New_Pulse(TIM_HandleTypeDef htim, TIM_OC_InitTypeDef soc, MSGQUEUE_OBJ_RGB msg){
	static uint8_t  newValor;
	
	newValor = round(msg.pulse*RELAC_VOL_PULSE);
	HAL_TIM_PWM_Stop(&htim, TIM_CHANNEL_1);
	HAL_TIM_PWM_DeInit(&htim);
	soc.Pulse = (100 - newValor);
	HAL_TIM_PWM_Init(&htim);
	HAL_TIM_PWM_ConfigChannel(&htim, &soc, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim, TIM_CHANNEL_1);
}

static void myPWM_Init(GPIO_InitTypeDef *sgpio, TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc){
	
	/*TIM4 CHA1*/
	__HAL_RCC_GPIOD_CLK_ENABLE();
	sgpio->Pin = GPIO_PIN_12;
	sgpio->Mode = GPIO_MODE_AF_PP;
	sgpio->Alternate = GPIO_AF2_TIM4;
	sgpio->Pull = GPIO_NOPULL;
	sgpio->Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, sgpio);
	
	__HAL_RCC_TIM4_CLK_ENABLE();
	htim->Instance = TIM4;
	htim->Init.Prescaler = 839;
	htim->Init.Period = 99;	//1khz parpadeo
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(htim);
	
	soc->OCMode = TIM_OCMODE_PWM1;
	soc->Pulse = 49;
	soc->OCPolarity = TIM_OCPOLARITY_HIGH;
	soc->OCFastMode = TIM_OCFAST_DISABLE;
}

/*TEST*/
int Init_Th_rgb_test(void){
	id_Th_rgb_test = osThreadNew(Th_rgb_test, NULL, NULL);
	if(id_Th_rgb_test != osOK)
		return(-1);
	return(0);
}

void Th_rgb_test(void*arg){
	
	MSGQUEUE_OBJ_RGB msg2;
	uint8_t cnt = 0;
	
	Init_Th_rgb();
	while(1){
		osDelay(50U);
		if(cnt <15)
			cnt++;
		else
			cnt = 0;
		msg2.pulse = cnt;
		osMessageQueuePut(id_MsgQueue_rgb, &msg2, 0U, 0U);
	}
}
