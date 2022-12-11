#include "com.h"
#include "stm32f4xx_hal.h"
#include "Driver_USART.h"
#include "string.h"
#include <stdio.h>

#define FLAG_CALLBACK_UART 0x01U 

static osThreadId_t id_Th_com;
static osMessageQueueId_t id_MsgQueue_com;

extern ARM_DRIVER_USART Driver_USART3;
static ARM_DRIVER_USART *USARTdrv = &Driver_USART3;

void Th_com(void *argument);
static void myUART_Init(void);
static void myUART_Callback();
static void myUART_Update_Data(MSGQUEUE_OBJ_COM msg);
static void myUART_Get_Time_Of_Frame(MSGQUEUE_OBJ_COM msg, uint8_t* hour, uint8_t* minutes, uint8_t* seconds);
static void myUART_Update_CMD(MSGQUEUE_OBJ_COM msg);

/*TEST*/
static osThreadId_t id_Th_com_test;
void Th_com_test(void*arg);

osThreadId_t get_id_Th_com(void){
	return id_Th_com;
}
osMessageQueueId_t get_id_MsgQueue_com(void){
	return id_MsgQueue_com;
}

static int Init_MsgQueue_com(void){
  id_MsgQueue_com = osMessageQueueNew(MSGQUEUE_OBJECTS_COM, sizeof(MSGQUEUE_OBJ_COM), NULL);
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
	static MSGQUEUE_OBJ_COM msg;
	
	Init_MsgQueue_com();
	myUART_Init();

	while(1){
		if(osMessageQueueGet(id_MsgQueue_com, &msg, NULL, osWaitForever) == osOK)
			myUART_Update_Data(msg);
		
		osThreadYield();
	}
}

static void myUART_Update_Data(MSGQUEUE_OBJ_COM msg){
	switch (msg.comPC){
		case CMD_RDA:
			myUART_Update_CMD(msg);
			
			break;
		case SET_TIME:
			break;
		
		case LCD_to_PC:
			break;
		
		case PC_CNTRL:
			break;
	}
}

static void myUART_Update_CMD(MSGQUEUE_OBJ_COM msg){
	static char buffer[50] = {0x00};	// ahorrar??
	static uint8_t h;
	static uint8_t m;
	static uint8_t s;
	uint8_t k, j;
	j = 0;
	
	myUART_Get_Time_Of_Frame(msg, &h, &m, &s);
	sprintf(buffer, "%.2u:%.2u:%.2u---> ", h, m, s);
	for(k=13; k<48; k++){
		sprintf(&buffer[k], "%X", msg.frame_Tx[j]);
		k+=2;
		buffer[k]=0x20;
		j++;
	}
	sprintf(&buffer[48], "\n");
	sprintf(&buffer[49], "\r");
	
	USARTdrv -> Send(buffer, sizeof(buffer));
	osThreadFlagsWait(FLAG_CALLBACK_UART, osFlagsWaitAny, osWaitForever);
}

static void myUART_Init(void){
	USARTdrv -> Initialize(myUART_Callback);
	USARTdrv -> PowerControl(ARM_POWER_FULL);
	USARTdrv -> Control(ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8 | ARM_USART_PARITY_NONE | 
											ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE, 9600);
	USARTdrv -> Control(ARM_USART_CONTROL_TX, 1);
	USARTdrv -> Control(ARM_USART_CONTROL_RX, 1);
}

static void myUART_Callback(){
	if(ARM_USART_EVENT_SEND_COMPLETE){
		osThreadFlagsSet(id_Th_com, FLAG_CALLBACK_UART);
	}
}

static void myUART_Get_Time_Of_Frame(MSGQUEUE_OBJ_COM msg, uint8_t* h, uint8_t* m, uint8_t* s){
	*h = msg.frame_time / 3600;
	*m = (msg.frame_time - (*h * 3600)) / 60;
	*s = (msg.frame_time - (*h * 3600) - (*m * 60));
}

/*TEST*/
int Init_Th_com_test(void){
	id_Th_com_test = osThreadNew(Th_com_test, NULL, 0U);
	if(id_Th_com_test != NULL)
		return(-1);
	return(0);
}

void Th_com_test(void*arg){
	static MSGQUEUE_OBJ_COM msg2;
	static uint8_t cnt = 0; 
	
	Init_Th_com();
	
	while(1){
		msg2.comPC = CMD_RDA;
		msg2.frame_time = 85801 + cnt;
		cnt%2 == 0 ? memset(msg2.frame_Tx, 0xEE, sizeof(msg2.frame_Tx)) : memset(msg2.frame_Tx, 0xAA, sizeof(msg2.frame_Tx));
		osMessageQueuePut(id_MsgQueue_com, &msg2, 0U, 0U);
		cnt++;
		osDelay(1000U);
	}
}
