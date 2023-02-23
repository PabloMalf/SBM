#include "lcd.h"

//ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
ARM_SPI_STATUS estado;

TIM_HandleTypeDef htim7;
GPIO_InitTypeDef GPIO_InitStruct;
extern unsigned char buffer[512];

uint16_t posL1 = 0;
uint16_t posL2 = 0;
char cadena[80];

void delay(uint32_t n_microsegundos)
{
  uint32_t retardo = n_microsegundos - 1;
  // Configurar y arrancar el timer para generar un evento
  // pasados n_microsegundos
	htim7.Instance = TIM7;
	// Reloj 84Mhz --> Para 28Mhz --> periodo = 28*n_microsegundos = 1; Estados de cuenta para que llegue en n us
	htim7.Init.Prescaler = 83;
  htim7.Init.Period = retardo;
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	
	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start(&htim7);
  
  // Esperar a que se active el flag del registro de Match
  // correspondiente
	while(!__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE)){}
  //while(TIM7->CNT < retardo){}
  // Parar el Timer y ponerlo a 0 para la siguiente llamada a
  // la función
	__HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
  HAL_TIM_Base_Stop(&htim7);
  HAL_TIM_Base_DeInit(&htim7);
}

void LCD_reset(void)
{
	SPIdrv->Initialize(NULL);
	SPIdrv->PowerControl(ARM_POWER_FULL);
	SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
 
	//Configuramos los pines: RESET | A0 | CS
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  // Configuro RESET
	 GPIO_InitStruct.Pin = GPIO_PIN_6; 
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  // Configuro A0
   GPIO_InitStruct.Pin =  GPIO_PIN_13 ; 
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  // Configuro CS
   GPIO_InitStruct.Pin = GPIO_PIN_14; 
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	// Seleccionamos nivel alto
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

	// Generar el pulso de reset
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

  delay(2);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	// Después del reset debe haber un retardo de 1 ms
	delay(1000);
}

void LCD_wr_data(unsigned char data)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); // Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET); // Seleccionar A0 = 1;
	SPIdrv->Send(&data, sizeof(data)); // Escribir un dato (data) usando la función SPIDrv->Send(…);
	do{
		estado = SPIdrv->GetStatus(); // Esperar a que se libere el bus SPI;
	}while(estado.busy);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); // Seleccionar CS = 1;
}

void LCD_wr_cmd(unsigned char cmd)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); // Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET); // Seleccionar A0 = 0;
	SPIdrv->Send(&cmd, sizeof(cmd)); // Escribir un comando (cmd) usando la función SPIDrv->Send(…);
	do{
		estado = SPIdrv->GetStatus(); // Esperar a que se libere el bus SPI;
	}while(estado.busy); 
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); // Seleccionar CS = 1;
}

 void LCD_init(void)
{
	LCD_wr_cmd(0xAE); //Display off
	LCD_wr_cmd(0xA2); //Fija el valor de la relación de la tensión de polarización del LCD a 1/9
	LCD_wr_cmd(0xA0); //El direccionamiento de la RAM de datos del display es la normal
	LCD_wr_cmd(0xC8); //El scan en las salidas COM es el normal
	LCD_wr_cmd(0x22); //Fija la relación de resistencias interna a 2
	LCD_wr_cmd(0x2F); //Power on
	LCD_wr_cmd(0x40); //Display empieza en la línea 0
	LCD_wr_cmd(0xAF); //Display ON
	LCD_wr_cmd(0x81); //Contraste
	LCD_wr_cmd(0x17); //Valor Contraste
	LCD_wr_cmd(0xA4); //Display all points normal
	LCD_wr_cmd(0xA6); //LCD Display normal
 }
 
 void LCD_update(void)
{
 int i;
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB0); // Página 0
 for(i = 0; i < 128; i++){
	LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB1); // Página 1
 for(i = 128; i < 256; i++){
	LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB2); //Página 2
 for(i = 256; i < 384; i++){
	LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB3); // Pagina 3
 for(i = 384; i < 512; i++){
	LCD_wr_data(buffer[i]);
 }
}

void symbolToLocalBuffer_L1(uint8_t symbol){
	uint8_t value1, value2;
	uint16_t offset = 0;
	offset = 25*(symbol - ' ');
	for(int i = 0; i < 12; i++){
		value1 = Arial12x12[offset + i * 2 + 1];
		value2 = Arial12x12[offset + i * 2 + 2];
		buffer[i + 0 + posL1] = value1;
		buffer[i + 128 + posL1] = value2;
	}
	posL1 = posL1 + Arial12x12[offset];
}

void symbolToLocalBuffer_L2(uint8_t symbol){
	uint8_t value1, value2;
	uint16_t offset = 0;
	offset = 25*(symbol - ' ');
	for(int i = 0; i < 12; i++){
		value1 = Arial12x12[offset + i * 2 + 1];
		value2 = Arial12x12[offset + i * 2 + 2];
		buffer[i + 256 + posL2] = value1;
		buffer[i + 384 + posL2] = value2;
	}
	posL2 = posL2 + Arial12x12[offset];
}

void symbolToLocalBuffer(uint8_t line, uint8_t symbol){
	uint8_t value1, value2;
	uint16_t offset = 0;
	offset = 25*(symbol - ' ');
	for(int i = 0; i < 12; i++){
		if(line == 1){
		value1 = Arial12x12[offset + i * 2 + 1];
		value2 = Arial12x12[offset + i * 2 + 2];
		
		buffer[i + 0 + posL1] = value1;
		buffer[i + 128 + posL1] = value2;
		
		}else if(line == 2){
			value1 = Arial12x12[offset + i * 2 + 1];
		  value2 = Arial12x12[offset + i * 2 + 2 ];
			
			buffer[i + 256 + posL2] = value1;
		  buffer[i + 384 + posL2] = value2;
		}
	}
	posL1 = posL1 + Arial12x12[offset];
	posL2 = posL2 + Arial12x12[offset];
}

void escribirEntero(int entero){
	sprintf(cadena, " Mi numero: %d" , entero);
	//int longuitud = strlen(cadena);
	for(uint8_t i = 0; i < strlen(cadena); i++){
		symbolToLocalBuffer_L1(cadena[i]);
	}
}

void escribirFloat(float flotante){
	sprintf(cadena, " El float: %f", flotante);
	//int longuitud = strlen(cadena);
	for(uint8_t i = 0; i < strlen(cadena); i++){
		symbolToLocalBuffer_L1(cadena[i]);
	}
}

