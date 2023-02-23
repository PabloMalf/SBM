#include "vol.h"
#include "stm32f4xx_hal.h"

osThreadId_t tid_Th_vol;                        // thread id
osMessageQueueId_t mid_vol;
ADC_HandleTypeDef ADC_InitStruct;

void Th_vol (void *argument);                   // thread function

int Init_Msg_vol (void) {
  mid_vol = osMessageQueueNew(MSGQUEUE_OBJECTS_vol, sizeof(MSGQUEUE_OBJ_vol_t), NULL);
  if (mid_vol == NULL) {
    return (-1);
	}
  return(0);
}

int Init_Th_vol(void) {
  tid_Th_vol = osThreadNew(Th_vol, NULL, NULL);
  if (tid_Th_vol == NULL) {
    return(-1);
  }

  return(0);
}

void Th_vol (void *argument)
{
	ADC_HandleTypeDef adchandle;
	ADC1_pins_F429ZI_config();
	float value;
	ADC_Init_Single_Conversion(&adchandle , ADC1);

	MSGQUEUE_OBJ_vol_t msg;
	Init_Msg_vol();
	
  while (1) {
		value = ADC_getVoltage(&adchandle , 10 );
		if(value != msg.Vol){
			msg.Vol = value;
			osMessageQueuePut(mid_vol, &msg, 0U, 0U);
		}
    osThreadYield();                            // suspend thread
  }
}

void ADC1_pins_F429ZI_config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int ADC_Init_Single_Conversion(ADC_HandleTypeDef *hadc, ADC_TypeDef* ADC_Instance)
{
	hadc->Instance = ADC_Instance;
	hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc->Init.Resolution = ADC_RESOLUTION_12B;
	hadc->Init.ScanConvMode = DISABLE;
	hadc->Init.ContinuousConvMode = DISABLE;
	hadc->Init.DiscontinuousConvMode = DISABLE;
	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc->Init.NbrOfConversion = 1;
	hadc->Init.DMAContinuousRequests = DISABLE;
	hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(hadc) != HAL_OK){
		return -1;
	}
	return 0;
}

float ADC_getVoltage(ADC_HandleTypeDef *hadc, uint32_t Channel)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	HAL_StatusTypeDef status;
	uint32_t raw = 0;
	float voltage = 0;
	sConfig.Channel = Channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
		return -1;
	}
	HAL_ADC_Start(hadc);
	do{
		status = HAL_ADC_PollForConversion(hadc, 0);
	}while (status != HAL_OK);
	raw = HAL_ADC_GetValue(hadc);
	voltage = raw * 3.3/4096;
	return voltage;
}

