#include "main.h"
#include "Driver_SPI.h"
//#include "RTE_Device.h"
#include "Arial12x12.h"
#include "string.h"
#include <stdio.h>

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif

#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

#ifdef RTE_CMSIS_RTOS2_RTX5
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment 
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

#endif

unsigned char buffer[512];
static uint16_t positionL1, positionL2 = 0;

static char data_L1[32];
static char data_L2[32];

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
TIM_HandleTypeDef tim7;
	
static void SystemClock_Config(void);
static void Error_Handler(void);
int i = 0;
static void LCD_reset(void);
static void LCD_wr_data(unsigned char data);
static void LCD_wr_cmd(unsigned char cmd);
static void LCD_init(void);
static void LCD_update(void);
static void delay(uint32_t n_microsegundos);
static void symbolToLocalBuffer(uint8_t lane, uint8_t symbol);
static void symbolToLocalBuffer_L1(uint8_t symbol);
static void symbolToLocalBuffer_L2(uint8_t symbol);
static void update_data(char data_L1[32], char data_L2[32]);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  SystemCoreClockUpdate();

  /* App */

	LCD_reset();
	LCD_init();

	sprintf(data_L1,"Prueba de texto L1");
	sprintf(data_L2,"Prueba de texto L2");
	update_data(data_L1, data_L2);
	
	/* App */ 
	
#ifdef RTE_CMSIS_RTOS2
  osKernelInitialize ();

  /* Init Threads */
	
	
	
	/* Init Threads */

  osKernelStart();
#endif

  while(1){

	}
}


static void SystemClock_Config(void){
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
	
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
    Error_Handler();
  }
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK){
    Error_Handler();
  }
  if (HAL_GetREVID() == 0x1001){
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

static void Error_Handler(void){
  while(1){}
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){ 
  while (1){}
}
#endif

static void delay(uint32_t n_microsegundos){
	__HAL_RCC_TIM7_CLK_ENABLE();
	tim7.Instance = TIM7;
	tim7.Init.Prescaler = 83;
	tim7.Init.Period = n_microsegundos - 1;

	HAL_TIM_Base_Init(&tim7);
	HAL_TIM_Base_Start(&tim7);
	
	while(!__HAL_TIM_GET_FLAG(&tim7, TIM_FLAG_UPDATE)){} //Bloqueante
	__HAL_TIM_CLEAR_FLAG(&tim7, TIM_FLAG_UPDATE);
		
	HAL_TIM_Base_Stop(&tim7);
	HAL_TIM_Base_DeInit(&tim7);
		
	__HAL_RCC_TIM7_CLK_DISABLE();
}

static void LCD_reset(void){
	static GPIO_InitTypeDef GPIO_InitStruct;
	/*CS*/
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	
	/*A0*/
	__HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	
	/*Reset*/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	
	/*SPI*/
	SPIdrv->Initialize(NULL);
  SPIdrv-> PowerControl(ARM_POWER_FULL);
  SPIdrv-> Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS (8), 20000000);
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	delay(1000);
}

static void LCD_wr_data(unsigned char data){
	static ARM_SPI_STATUS stat;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	SPIdrv->Send(&data, sizeof(data));
	do{
		stat = SPIdrv->GetStatus();
	}while(stat.busy);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

static void LCD_wr_cmd(unsigned char cmd){
	static ARM_SPI_STATUS stat;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
	SPIdrv->Send(&cmd, sizeof(cmd));
	do{
		stat = SPIdrv->GetStatus();
	}	while(stat.busy);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

static void LCD_init(void){
	LCD_wr_cmd(0xAE);//display off
	LCD_wr_cmd(0xA2);//Fija el valor de la tensi�n de polarizaci�n del LCD a 1/9
	LCD_wr_cmd(0xA0);//El direccionamiento de la RAM de datos del display es la normal
	LCD_wr_cmd(0xC8);//El scan en las salidas COM es el normal
	LCD_wr_cmd(0x22);//Fija la relaci�n de resistencias interna a 2
	LCD_wr_cmd(0x2F);//Power on
	LCD_wr_cmd(0x40);//Display empieza en la l�nea 0
	LCD_wr_cmd(0xAF);//Display ON
	LCD_wr_cmd(0x81);//Contraste
	LCD_wr_cmd(0x17);//Valor de contraste
	LCD_wr_cmd(0xA4);//Display all points normal
	LCD_wr_cmd(0xA6);//LCD Display Normal
}

static void LCD_update(void){
	int i;	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB0);
	for(i = 0; i < 128; i++){LCD_wr_data(buffer[i]);}

	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB1);
	for(i = 128; i < 256; i++){LCD_wr_data(buffer[i]);}

	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB2);
	for(i = 256; i < 384; i++){LCD_wr_data(buffer[i]);}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB3);
	for(i = 384; i < 512; i++){LCD_wr_data(buffer[i]);}
}

static void symbolToLocalBuffer(uint8_t lane, uint8_t symbol){
	if(lane == 1)
		symbolToLocalBuffer_L1(symbol);
	if(lane == 2)
		symbolToLocalBuffer_L2(symbol);
}

static void symbolToLocalBuffer_L1(uint8_t symbol){
	static uint8_t i, value1, value2;
	static uint16_t offset = 0;
	offset = 25 * (symbol - ' ');
	for(i = 0; i < 12; i++){
		value1 = Arial12x12[offset + i * 2 + 1];
		value2 = Arial12x12[offset + i * 2 + 2];
		buffer[i + positionL1] = value1;
		buffer[i + 128 + positionL1] = value2;
	}
	positionL1 = positionL1 + Arial12x12[offset];
	LCD_update();
}

static void symbolToLocalBuffer_L2(uint8_t symbol){
	static uint8_t i, value1, value2;
	static uint16_t offset = 0;
	offset = 25 * (symbol - ' ');
	for( i = 0; i < 12; i++){
		value1 = Arial12x12[offset + i * 2 + 1];
		value2 = Arial12x12[offset + i * 2 + 2];
		buffer[i + 256 + positionL2] = value1;
		buffer[i + 384 + positionL2] = value2;
	}
	positionL2 = positionL2 + Arial12x12[offset];
	LCD_update();
}

static void update_data(char data_L1[32], char data_L2[32]){
	static int i, j;
	for(i = 0; i < positionL1; i++){
		buffer[i] = 0x00;
		buffer[i+128] = 0x00;
	}	
	for(i = 0; i < strlen(data_L1); i++){
		symbolToLocalBuffer(1, data_L1[i]);
	}
	for(i = positionL1; i < 128; i++){
		buffer[i] = 0x00;
		buffer[i+128] = 0x00;
	}	
	
	for(j = 0; j < positionL2; j++){
		buffer[j+256] = 0x00;
		buffer[j+384] = 0x00;
	}	
	for(j = 0; j < strlen(data_L2); j++){
		symbolToLocalBuffer(2, data_L2[j]);
	}
	for(j = positionL2; j < 128; j++){
		buffer[j+256] = 0x00;
		buffer[j+384] = 0x00;
	}
}

