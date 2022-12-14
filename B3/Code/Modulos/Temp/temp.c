#include "temp.h"
#include "Driver_I2C.h" 
#include "stm32f4xx_hal.h"

#define I2C_ADDR 0x48

#define FLAG_TMR 0x01
#define FLAG_CALLBACK_I2C 0x02

extern ARM_DRIVER_I2C Driver_I2C1;
ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;

static osThreadId_t id_Th_temp;
static osThreadId_t id_Th_temp_test;
static osMessageQueueId_t id_MsgQueue_temp;

static void led_init(void);
static void temp_reset(void);
static void Th_temp(void *argument);
static void Th_temp_test(void *argument);

osMessageQueueId_t get_id_MsgQueue_temp(void){
	return id_MsgQueue_temp;
}

static int Init_MsgQueue_temp(void){
  id_MsgQueue_temp = osMessageQueueNew(MSGQUEUE_OBJECTS_TEMP, sizeof(MSGQUEUE_OBJ_TEMP), NULL);
  if(id_MsgQueue_temp == NULL)
    return (-1); 
  return(0);
}

int Init_Th_temp(void){
  id_Th_temp = osThreadNew(Th_temp, NULL, NULL);
  if(id_Th_temp == NULL)
    return(-1);
  return(Init_MsgQueue_temp());
}

int Init_Th_temp_test(void){
  id_Th_temp_test = osThreadNew(Th_temp_test, NULL, NULL);
  if(id_Th_temp_test == NULL)
    return(-1);
  return(Init_Th_temp());
}

static void callback_i2c(uint32_t event){
	osThreadFlagsSet(id_Th_temp, FLAG_CALLBACK_I2C);
}

void Th_temp(void *argument) {
	static MSGQUEUE_OBJ_TEMP msg;
	static uint8_t buf_temp[2];
	static uint8_t cmd = 0;
	
	temp_reset();

	I2Cdrv->MasterTransmit(I2C_ADDR, &cmd , 2, true);
	osThreadFlagsWait(FLAG_CALLBACK_I2C, osFlagsWaitAll, osWaitForever);
	I2Cdrv->MasterReceive(I2C_ADDR, buf_temp, 2, false);
	osThreadFlagsWait(FLAG_CALLBACK_I2C, osFlagsWaitAll, osWaitForever);
	msg.temperature = (((buf_temp[0] << 8) | buf_temp[1]) >> 5) * 0.125;
	osMessageQueuePut(id_MsgQueue_temp, &msg, 0U, 0U);
	
	while(1){
		I2Cdrv->MasterTransmit(I2C_ADDR, &cmd , 2, true);
		osThreadFlagsWait(FLAG_CALLBACK_I2C, osFlagsWaitAll, osWaitForever);
		I2Cdrv->MasterReceive(I2C_ADDR, buf_temp, 2, false);
		osThreadFlagsWait(FLAG_CALLBACK_I2C, osFlagsWaitAll, osWaitForever);
		msg.temperature = (((buf_temp[0] << 8) | buf_temp[1]) >> 5) * 0.125;
		osMessageQueuePut(id_MsgQueue_temp, &msg, 0U, 0U);
		osDelay(1000);
	}
}

static void temp_reset(void){
	I2Cdrv-> Initialize(callback_i2c);
	I2Cdrv-> PowerControl ( ARM_POWER_FULL );
	I2Cdrv->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
	I2Cdrv->Control      (ARM_I2C_BUS_CLEAR, 0);
}

void Th_temp_test(void* argument){
	static MSGQUEUE_OBJ_TEMP msg;
	float temp_prev = 1;
	led_init();
	Init_Th_temp();
	while(1){
		if(osOK == osMessageQueueGet(id_MsgQueue_temp, &msg, 0U, 0U)){
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			if(msg.temperature > temp_prev){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
			}
			else if(msg.temperature < temp_prev){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
			}
			temp_prev = msg.temperature;
		}
	}
}

static void led_init(void){
	static GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14), GPIO_PIN_RESET);
}
