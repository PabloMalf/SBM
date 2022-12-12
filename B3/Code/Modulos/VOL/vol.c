#include "vol.h"
#include "stm32f4xx_hal.h"

#define VOL_REF 15U
#define RESOLUTION_12B 4060U //4096U

static osThreadId_t id_Th_vol;
static osMessageQueueId_t id_MsgQueue_vol;

static MSGQUEUE_OBJ_VOL msg;

void Th_vol(void *argument);
static void myADC_Init(ADC_HandleTypeDef *hadc);
static float myADC_Get_Voltage(ADC_HandleTypeDef *hadc);

static osThreadId_t id_Th_vol_test;
static void Th_vol_test(void *argument);

osMessageQueueId_t get_id_MsgQueue_vol(void){
	return id_MsgQueue_vol;
}

static int Init_MsgQueue_vol(void){
  id_MsgQueue_vol = osMessageQueueNew(MSGQUEUE_OBJECTS_VOL, sizeof(MSGQUEUE_OBJ_VOL), NULL);
  if(id_MsgQueue_vol == NULL)
    return (-1); 
  return(0);
}

int Init_Th_vol(void){
  id_Th_vol = osThreadNew(Th_vol, NULL, NULL);
  if(id_Th_vol == NULL)
    return(-1);
  return(0);
}

static void myADC_Init(ADC_HandleTypeDef *hadc){
	static GPIO_InitTypeDef sgpio = {0};
	static ADC_ChannelConfTypeDef sadc = {0};
	
	/*PC0(ADC1)*/
	__HAL_RCC_GPIOC_CLK_ENABLE();
	sgpio.Pin = GPIO_PIN_0;
	sgpio.Mode = GPIO_MODE_ANALOG;
	sgpio.Pull = GPIO_NOPULL;
	sgpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOC, &sgpio);	
	
	__HAL_RCC_ADC1_CLK_ENABLE();
	hadc->Instance = ADC1;
	hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc->Init.Resolution = ADC_RESOLUTION_12B;
	hadc->Init.ContinuousConvMode = DISABLE;
	hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc->Init.DiscontinuousConvMode = DISABLE;
	hadc->Init.DMAContinuousRequests = DISABLE;
	hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV; //
	hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc->Init.NbrOfConversion = 1;
	hadc->Init.ScanConvMode = DISABLE;
	HAL_ADC_Init(hadc);
	
	sadc.Channel = 10;
	sadc.Rank = 1;
	sadc.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(hadc, &sadc);
}

static float myADC_Get_Voltage(ADC_HandleTypeDef *hadc){
	static HAL_StatusTypeDef status;
	static uint32_t raw_voltage; 
	uint8_t voltage;
	
	HAL_ADC_Start(hadc);
	
	do{
		status = HAL_ADC_PollForConversion(hadc, 0);
	}while(status != HAL_OK);
	
	raw_voltage = HAL_ADC_GetValue(hadc);
	voltage = raw_voltage * VOL_REF / RESOLUTION_12B;
	
	HAL_ADC_Stop(hadc);
	
	return voltage;
}

static void Th_vol(void *argument){
	static ADC_HandleTypeDef hadc = {0};
	static uint8_t voltage, prev_voltage = 0;
	
	Init_MsgQueue_vol();
	myADC_Init(&hadc);
	
	while(1){
		voltage = myADC_Get_Voltage(&hadc);
		if(prev_voltage != voltage){
			msg.volume_lvl = voltage;
			osMessageQueuePut(id_MsgQueue_vol, &msg, 0U, 0U);
			prev_voltage = voltage;
		}
	}
}

/*TEST*/
static void myInit_led(void){
	static GPIO_InitTypeDef sgpio;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	sgpio.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14;
	sgpio.Mode = GPIO_MODE_OUTPUT_PP;
	sgpio.Pull = GPIO_PULLUP;
	sgpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &sgpio);
}

int Init_Th_vol_test(void){
  id_Th_vol_test = osThreadNew(Th_vol_test, NULL, NULL);
  if(id_Th_vol_test == NULL)
    return(-1);
  return(0);
}

static void Th_vol_test(void *argument){
	static MSGQUEUE_OBJ_VOL msg2;
	
	Init_Th_vol();
	myInit_led();
	
	while(1){
		if(osOK == osMessageQueueGet(id_MsgQueue_vol, &msg2, NULL, osWaitForever)){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,  (msg2.voltage_level < 4 ? GPIO_PIN_RESET : GPIO_PIN_SET));
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  (msg2.voltage_level < 8 ? GPIO_PIN_RESET : GPIO_PIN_SET));
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, (msg2.voltage_level < 12 ? GPIO_PIN_RESET : GPIO_PIN_SET));
		}
	}
}
