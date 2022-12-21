#include "com.h"
#include "stm32f4xx_hal.h"
#include "Driver_USART.h"
#include "string.h"
#include <stdio.h>

// flags de la callback
#define FLAG_UART_ENVIADO		0x01U
#define FLAG_UART_RECIVIDO	0x02U
#define FLAG_UART_ERROR			0x04U

static osThreadId_t id_Th_com;
static osMessageQueueId_t id_MsgQueue_com;

extern ARM_DRIVER_USART Driver_USART3;
static ARM_DRIVER_USART *USARTdrv = &Driver_USART3;

void Th_com(void *argument);
static void myUART_Init(void);
static void myUART_Callback();
static void myUART_Get_Time_Of_Frame(MSGQUEUE_OBJ_COM_MOSI msg_mosi, uint8_t* hour, uint8_t* minutes, uint8_t* seconds);
static int myUART_Update_CMD(MSGQUEUE_OBJ_COM_MOSI msg_mosi);
static int myUART_Update_CLK(MSGQUEUE_OBJ_COM_MISO msg_miso);

/*TEST*/
static osThreadId_t id_Th_com_test;
void Th_com_test(void*arg);

osMessageQueueId_t get_id_MsgQueue_com(void){
	return id_MsgQueue_com;
}

static int Init_MsgQueue_com(void){
  id_MsgQueue_com = osMessageQueueNew(MSGQUEUE_OBJECTS_COM, sizeof(MSGQUEUE_OBJ_COM_MOSI), NULL);
  if(id_MsgQueue_com == NULL)
    return (-1); 
  return(0);
}

int Init_Th_com(void){
  id_Th_com  = osThreadNew(Th_com, NULL, NULL);
  if(id_Th_com  == NULL)
    return(-1);
  return(0);
}

static void Th_com(void *argument){
	static MSGQUEUE_OBJ_COM_MOSI msg_mosi;
	static MSGQUEUE_OBJ_COM_MISO msg_miso;
	
	Init_MsgQueue_com();
	myUART_Init();

	while(1){
		if(osMessageQueueGet(id_MsgQueue_com, &msg_mosi, NULL, 500U) == osOK)
			myUART_Update_CMD(msg_mosi);
		else{
			myUART_Update_CLK(msg_miso);	
		}
		osThreadYield();
	}
}

/* update de hora y trama de 12 bytes de la radio */
static int myUART_Update_CMD(MSGQUEUE_OBJ_COM_MOSI msg_mosi){
	static char buffer[50] = {0x00};
	static uint8_t h;
	static uint8_t m;
	static uint8_t s;
	static uint32_t xFlags;
	uint8_t k, j;
	j = 0;
	
	myUART_Get_Time_Of_Frame(msg_mosi, &h, &m, &s);
	sprintf(buffer, "%.2u:%.2u:%.2u---> ", h, m, s);
	for(k=13; k<48; k++){
		sprintf(&buffer[k], "%X", msg_mosi.frame_Tx[j]);
		k+=2;
		buffer[k]=0x20;
		j++;
	}
	sprintf(&buffer[48], "\n");
	sprintf(&buffer[49], "\r");
	
	USARTdrv -> Send(buffer, sizeof(buffer));
	xFlags = osThreadFlagsWait(FLAG_UART_ENVIADO | FLAG_UART_ERROR, osFlagsWaitAny, osWaitForever);
	if(xFlags & FLAG_UART_ERROR)
		return(-1);
	return(0);
}

/*config de la uart*/
static void myUART_Init(void){
	USARTdrv -> Initialize(myUART_Callback);
	USARTdrv -> PowerControl(ARM_POWER_FULL);
	USARTdrv -> Control(ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8 | ARM_USART_PARITY_NONE | 
											ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE, 9600);
	USARTdrv -> Control(ARM_USART_CONTROL_TX, 1);
	USARTdrv -> Control(ARM_USART_CONTROL_RX, 1);
}

/*callback*/
static void myUART_Callback(uint32_t event){
	
	if(event & ARM_USART_EVENT_SEND_COMPLETE)
		osThreadFlagsSet(id_Th_com, FLAG_UART_ENVIADO);
	
	if(event & ARM_USART_EVENT_RECEIVE_COMPLETE)
			osThreadFlagsSet(id_Th_com, FLAG_UART_RECIVIDO);
//	if((event & ARM_USART_EVENT_SEND_COMPLETE) | (event & ARM_USART_EVENT_RECEIVE_COMPLETE) )error
}

static void myUART_Get_Time_Of_Frame(MSGQUEUE_OBJ_COM_MOSI msg_mosi, uint8_t* h, uint8_t* m, uint8_t* s){
	*h = msg_mosi.hora / 3600;
	*m = (msg_mosi.hora - (*h * 3600)) / 60;
	*s = (msg_mosi.hora - (*h * 3600) - (*m * 60));
}

/*OPCIONAL del set hora desde teraterm. hora actualizada*/
static int myUART_Update_CLK(MSGQUEUE_OBJ_COM_MISO msg_miso){
	static char buffer [5]= {0x00};
	static uint32_t xFlag;
	
	USARTdrv -> Receive(buffer, sizeof(buffer));
	xFlag = osThreadFlagsWait(FLAG_UART_RECIVIDO | FLAG_UART_ERROR, osFlagsWaitAny, osWaitForever);
	if(xFlag & FLAG_UART_ERROR)
		return (-1);
	
	msg_miso.hora = ((buffer[0]*3600)+(buffer[2]*60)+buffer[4]); // 0x 1508f = 86159 nueva h 23:55:59
	osMessageQueuePut(id_MsgQueue_com, &msg_miso, 0U, 0U);
	return (0);
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
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}

int Init_Th_com_test(void){
	id_Th_com_test = osThreadNew(Th_com_test, NULL, 0U);
	if(id_Th_com_test != NULL)
		return(-1);
	return(0);
}

void Th_com_test(void*arg){
	static MSGQUEUE_OBJ_COM_MOSI msg2;
	static uint8_t cnt = 0; 
	
	Init_Th_com();
	myInit_led();
	msg2.hora = 85859; // 0x14F63 23:50:59
	while(1){

		if(cnt == 6){
			msg2.comPC = SET_TIME;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			osMessageQueuePut(id_MsgQueue_com, &msg2, 0U, 0U);
			osMessageQueueGet(id_MsgQueue_com, &msg2, NULL, osWaitForever);
		}
		
		msg2.comPC = CMD_RDA;
		cnt%2 == 0 ? memset(msg2.frame_Tx, 0xEE, sizeof(msg2.frame_Tx)) : memset(msg2.frame_Tx, 0xAA, sizeof(msg2.frame_Tx));
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		osMessageQueuePut(id_MsgQueue_com, &msg2, 0U, 0U);
		osDelay(1000U);
			
		msg2.hora++;
		cnt++;
	}
}

