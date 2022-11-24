#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id

unsigned char buffer[512];
extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
bool turn = true;
static void LCD_blanco(void);
static void LCD_reset(void);
static void LCD_wr_data(unsigned char data);
static void LCD_wr_cmd(unsigned char cmd);
static void LCD_init(void);
static void LCD_update(void);
static void delay(uint32_t n_microsegundos);
int i = 0;
TIM_HandleTypeDef tim7;



void Thread (void *argument);                   // thread function
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
  LCD_reset();
  LCD_init();
  LCD_update();
  return(0);
}
 
void Thread (void *argument) {
  
  int i = 0;
  while (1) {
          buffer[i] = 0xFF; // 0xFF for full print; 0xAA for lines; 
          buffer[i+128] = 0xFF;
          buffer[i+256] = 0xFF;
          buffer[i+384] = 0xFF;
    if(turn){
      i++;
      if(i == 128){
        i=0;
      }}
    else{
      i--;
      if(i == 0){
        i=128;
      }}
    
    LCD_update();
    osDelay(25);
    LCD_blanco();
    LCD_update();
  }
}


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
//Funcions
static void LCD_reset(void){
  static GPIO_InitTypeDef GPIO_InitStruct_LCD;
  /*CS*/
  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitStruct_LCD.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_LCD.Pull = GPIO_PULLUP;
  GPIO_InitStruct_LCD.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct_LCD.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct_LCD);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
  
  /*A0*/
  __HAL_RCC_GPIOF_CLK_ENABLE();
  GPIO_InitStruct_LCD.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_LCD.Pull = GPIO_PULLUP;
  GPIO_InitStruct_LCD.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct_LCD.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct_LCD);
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
  
  /*Reset*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct_LCD.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_LCD.Pull = GPIO_PULLUP;
  GPIO_InitStruct_LCD.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct_LCD.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_LCD);
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
  LCD_wr_cmd(0xA2);//Fija el valor de la tensión de polarización del LCD a 1/9
  LCD_wr_cmd(0xA0);//El direccionamiento de la RAM de datos del display es la normal
  LCD_wr_cmd(0xC8);//El scan en las salidas COM es el normal
  LCD_wr_cmd(0x22);//Fija la relación de resistencias interna a 2
  LCD_wr_cmd(0x2F);//Power on
  LCD_wr_cmd(0x40);//Display empieza en la línea 0
  LCD_wr_cmd(0xAF);//Display ON
  LCD_wr_cmd(0x81);//Contraste
  LCD_wr_cmd(0x17);//Valor de contraste
  LCD_wr_cmd(0xA4);//Display all points normal
  LCD_wr_cmd(0xA6);//LCD Display Normal // A6 MODO NORMAL A7 MODO INVERSO
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

void LCD_blanco(void){
  for(i = 0; i<512; i++)
    buffer[i] = 0x00;
}

