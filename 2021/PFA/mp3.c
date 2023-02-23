#include "mp3.h"

osThreadId_t tid_Th_mp3;                        // thread id
osMessageQueueId_t mid_mp3;

static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;
char cmd;

void Th_mp3 (void *argument);                   // thread function

int Init_Msg_mp3 (void) {
  mid_mp3 = osMessageQueueNew(MSGQUEUE_OBJECTS_mp3, sizeof(MSGQUEUE_OBJ_mp3_t), NULL);
  if (mid_mp3 == NULL) {
    return (-1);
	}
  return(0);
}

int Init_Th_mp3(void) 
{
  tid_Th_mp3 = osThreadNew(Th_mp3, NULL, NULL);
  if (tid_Th_mp3 == NULL) {
    return(-1);
  }

  return(0);
}

void Th_mp3 (void *argument) 
{

	MSGQUEUE_OBJ_mp3_t msg;
	Init_Msg_mp3();
	
  while (1) {
    if(osOK == osMessageQueueGet(mid_mp3, &msg, 0U, 0U)) {
			if(msg.Command == 1){
				
			}
		}
    osThreadYield();                            // suspend thread
  }
}

void Init_USART (void){
    USARTdrv->Initialize(myUSART_callback);
    USARTdrv->PowerControl(ARM_POWER_FULL);
    USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, 4800);
     
    USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
    USARTdrv->Control (ARM_USART_CONTROL_RX, 1);
}

void myUSART_callback(uint32_t event)
{
  uint32_t mask;
  mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;
  if (event & mask) {
    osThreadFlagsSet(tid_Th_mp3, 0x01); //setter el flag correspondiente
  }
}
