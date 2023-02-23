#include "DRIVER_SPI.h"
#include "lcd.h"
#include "Arial12x12.h"


extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
ARM_SPI_STATUS estado;

TIM_HandleTypeDef htim7;
GPIO_InitTypeDef GPIO_InitStruct;
unsigned char buffer[512];

uint16_t positionL1 = 0;
uint16_t positionL2 = 0;
char cadena[80];

osThreadId_t tid_Th_lcd;

int Init_Th_lcd(void) 
{
  tid_Th_lcd = osThreadNew(Th_lcd, NULL, NULL);
  if (tid_Th_lcd == NULL) {
    return(-1);
  }
  return(0);
}

void Th_lcd (void *argument)
{
	LCD_reset();
	LCD_init();
	
	escribirFrase("prueba");
	osDelay(1000);
	while (1){

		osThreadYield();  
  }
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

  osDelay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	// Después del reset debe haber un retardo de 1 ms
	osDelay(1000);
}

void LCD_wr_data(unsigned char data)
{
 // Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
 // Seleccionar A0 = 1;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);

 // Escribir un dato (data) usando la función SPIDrv->Send(…);
	SPIdrv->Send(&data, sizeof(data));
 // Esperar a que se libere el bus SPI;
	do{
		estado = SPIdrv->GetStatus();
	}while(estado.busy);
	// Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_wr_cmd(unsigned char cmd)
{
 // Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
 // Seleccionar A0 = 0;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

 // Escribir un comando (cmd) usando la función SPIDrv->Send(…);
	SPIdrv->Send(&cmd, sizeof(cmd));
 // Esperar a que se libere el bus SPI;
	do{
		estado = SPIdrv->GetStatus();
	}while(estado.busy);
	// Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
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

 for(i=0;i<128;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB1); // Página 1

 for(i=128;i<256;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB2); //Página 2
 for(i=256;i<384;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB3); // Pagina 3


 for(i=384;i<512;i++){
 LCD_wr_data(buffer[i]);
 }
}

void symbolToLocalBuffer_L1(uint8_t symbol, int reset)
{
	
	uint8_t i, value1, value2;
	uint16_t offset = 0;
	
	offset = 25*(symbol - ' ');
	
	for(i = 0; i<12; i++){
		
		value1 = Arial12x12[offset+i*2+1];
		value2 = Arial12x12[offset+i*2+2];
		
		buffer[i + 0 + positionL1] = value1;
		buffer[i + 128 + positionL1] = value2;
	}
	positionL1 = positionL1 + Arial12x12[offset];
  if(reset==0) positionL1=0;
	LCD_update();
}

void symbolToLocalBuffer_L2(uint8_t symbol, int reset)
{
	
	uint8_t i, value1, value2;
	uint16_t offset = 0;
	
	offset = 25*(symbol - ' ');
	
	for(i = 0; i<12; i++){
		
		value1 = Arial12x12[offset+i*2+1];
		value2 = Arial12x12[offset+i*2+2];
		
		buffer[i + 256 + positionL2] = value1;
		buffer[i + 384 + positionL2] = value2;
	}
	positionL2 = positionL2 + Arial12x12[offset];
  if(reset==0) positionL2=0;
	LCD_update();
}

void symbolToLocalBuffer(uint8_t line, uint8_t symbol, int reset)
{
	uint8_t i, value1, value2;
	uint16_t offset = 0;
	
	offset = 25*(symbol - ' ');
	for(i = 0; i<12; i++){
		if(line == 1){
		value1 = Arial12x12[offset+i*2+1];
		value2 = Arial12x12[offset+i*2+2];
		
		buffer[i + 0 + positionL1] = value1;
		buffer[i + 128 + positionL1] = value2;
		
		}else if(line == 2){
			value1 = Arial12x12[offset+i*2+1];
		  value2 = Arial12x12[offset+i*2+2];
			
			buffer[i + 256 + positionL2] = value1;
		  buffer[i + 384 + positionL2] = value2;
		}
	}
	positionL1 = positionL1 + Arial12x12[offset];
	positionL2 = positionL2 + Arial12x12[offset];
  if(reset == 0){
		positionL2 = 0;
		positionL1 = 0;
	}
	LCD_update();
}

void escribirEntero(int entero)
{
	int i;
	sprintf(cadena, " Mi numero: %d" , entero);
	for(i=0; i< strlen(cadena);i++){
		symbolToLocalBuffer_L1(cadena[i],i);
	}
}

void escribirFloat(float flotante)
{
	int i;
	sprintf(cadena, " El float: %f" , flotante);
	for(i=0; i< strlen(cadena);i++){
		symbolToLocalBuffer_L1(cadena[i],i);
	}
}

void escribirFrase(char frase[])
{
	int i;
	sprintf(cadena, " %s" , frase);
	for(i=0; i< strlen(cadena);i++){
		symbolToLocalBuffer_L1(cadena[i],i);
	}
}

void limpiarPantalla(void)
{
	int i;
	for(i = 0; i < 512; i ++){
			buffer[i] = 0;
	}
	LCD_update();
}
