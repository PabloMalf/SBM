#include "com.h"
int c=0;
/*----------------------------------------------------------------------------
 *                       Colas de mensajes del PC
 *---------------------------------------------------------------------------*/
  
typedef struct {                                // object data type
  uint8_t Idx;
	uint8_t h;
	uint8_t m;
	uint8_t s;
} MSGQUEUE_OBJ_t;
MSGQUEUE_OBJ_t msg_pc;

extern osMessageQueueId_t mid_MsgQueue_pc;                // message queue id
 


/*----------------------------------------------------------------------------
 *                   Callback del MP3 y definiciones
 *---------------------------------------------------------------------------*/
  

char buf_com[50];//buffer de array creado para el envio de comandos


osThreadId_t tid_myUSART3_Thread;//id del thread
 
/* USART Driver */
extern ARM_DRIVER_USART Driver_USART3;//definicion del nombre de la USART
static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;//Variable del driver de la usart
ARM_USART_STATUS stat_com;// variable STATUS para saber si se ha liverado el buffer
ARM_DRIVER_VERSION     version; //version del driver
ARM_USART_CAPABILITIES drv_capabilities;//capacidades del driver


void myUSART3_callback(uint32_t event)//se crea para ver los errores y los estados de los eventos
{
  uint32_t mask;
  mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |ARM_USART_EVENT_TRANSFER_COMPLETE |ARM_USART_EVENT_SEND_COMPLETE |ARM_USART_EVENT_TX_COMPLETE;//estados de la usart
  if (event & mask) {
    /* Success: Wakeup Thread */
    osThreadFlagsSet (tid_myUSART3_Thread, 0x08);//como se ha hecho bien mando el flag del thread
  }
  if (event & ARM_USART_EVENT_RX_TIMEOUT) {//ha habido un error porque ha acabado el tiempo del timeut
    __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
  }
  if (event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_TX_UNDERFLOW)) {//error porque supera el máximo y se desborda
    __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
  }
}


/*----------------------------------------------------------------------------
 *                                Thread com-pc
 *---------------------------------------------------------------------------*/

///////////Init thread///////////
int Init_myUSART3_Thread (void) {
 
  tid_myUSART3_Thread = osThreadNew(myUSART3_Thread, NULL, NULL);
  if (tid_myUSART3_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}


/* CMSIS-RTOS Thread - UART command thread */
void myUSART3_Thread(void* args)
{
    
 
  #ifdef DEBUG
    version = USARTdrv->GetVersion();
    if (version.api < 0x200)   /* requires at minimum API version 2.00 or higher */
    {                          /* error handling */
        return;
    }
    drv_capabilities = USARTdrv->GetCapabilities();
    if (drv_capabilities.event_tx_complete == 0)
    {                          /* error handling */
        return;
    }
  #endif
//////////inicialización de la USART3 para el PC///////////////
    /*Initialize the USART driver */
    USARTdrv->Initialize(myUSART3_callback);
    /*Power up the USART peripheral */
    USARTdrv->PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 9600 Bits/sec */
    USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |ARM_USART_DATA_BITS_8 |ARM_USART_PARITY_NONE |ARM_USART_STOP_BITS_1 |ARM_USART_FLOW_CONTROL_NONE, 9600);//en el mp3 es necesario 9600baudios
     
    /* Enable Receiver and Transmitter lines */
    USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
    USARTdrv->Control (ARM_USART_CONTROL_RX, 0);//desactivo la recepcion porque no recibo nada
 
    //////envio los comandos 
    //USARTdrv->Send(buf, 8);//primerlo lo mando y luego lo paro con el flag de abajo para esperarlo
		
			//pc(horas,minutos,segundos);
			
    while (1)
    {
			osMessageQueueGet(mid_MsgQueue_pc, &msg_pc, NULL, osWaitForever);
			if(msg_pc.Idx==7){//moso reposo
				sprintf(buf_com, "%02d:%02d:%02d---> 7E FF 06 0A 00 00 00 EF\n\r", msg_pc.h,msg_pc.m,msg_pc.s);
				USARTdrv->Send(&buf_com,strlen(buf_com));
				osThreadFlagsWait (0x08, osFlagsWaitAny , osWaitForever);
			}
			if(msg_pc.Idx==6){//carpeta siguiente
				uint8_t cmd=0;
				if(c<3){
					c--;
					if(c==2){
						cmd=2;
					}
					if(c==1){
						cmd=1;
					}
				}else {
					cmd=1;
				}
				sprintf(buf_com, "%02d:%02d:%02d---> 7E FF 06 0B 00 %02d 01 EF\n\r", msg_pc.h,msg_pc.m,msg_pc.s,cmd);
				USARTdrv->Send(&buf_com,strlen(buf_com));
				osThreadFlagsWait (0x08, osFlagsWaitAny , osWaitForever);
			}
			if(msg_pc.Idx==5){//carpeta anterior
				uint8_t cmd=0;
				if(c<3){
					c++;
					if(c==2){
						cmd=2;
					}
					if(c==3){
						cmd=3;
					}
				}else {
					cmd=3;
				}
				sprintf(buf_com, "%02d:%02d:%02d---> 7E FF 06 0A 00 %02d 01 EF\n\r", msg_pc.h,msg_pc.m,msg_pc.s, cmd);
				USARTdrv->Send(&buf_com,strlen(buf_com));
				osThreadFlagsWait (0x08, osFlagsWaitAny , osWaitForever);
				
			}
			if(msg_pc.Idx==4){//cancion anterior
				sprintf(buf_com, "%02d:%02d:%02d---> 7E FF 06 02 00 00 00 EF\n\r", msg_pc.h,msg_pc.m,msg_pc.s);
				USARTdrv->Send(&buf_com,strlen(buf_com));
				osThreadFlagsWait (0x08, osFlagsWaitAny , osWaitForever);
			}
			if(msg_pc.Idx==3){//siguiente cancion
				sprintf(buf_com, "%02d:%02d:%02d---> 7E FF 06 01 00 00 00 EF\n\r", msg_pc.h,msg_pc.m,msg_pc.s);
				USARTdrv->Send(&buf_com,strlen(buf_com));
				osThreadFlagsWait (0x08, osFlagsWaitAny , osWaitForever);
			}
			if(msg_pc.Idx==2){//pausa
				sprintf(buf_com, "%02d:%02d:%02d---> 7E FF 06 0E 00 00 00 EF\n\r", msg_pc.h,msg_pc.m,msg_pc.s);
				USARTdrv->Send(&buf_com,strlen(buf_com));
				osThreadFlagsWait (0x08, osFlagsWaitAny , osWaitForever);
			}
			if(msg_pc.Idx==1){//play
				sprintf(buf_com, "%02d:%02d:%02d---> 7E FF 06 0D 00 00 00 EF\n\r", msg_pc.h,msg_pc.m,msg_pc.s);
				USARTdrv->Send(&buf_com,strlen(buf_com));
				osThreadFlagsWait (0x08, osFlagsWaitAny , osWaitForever);
			}
			if(msg_pc.Idx==17){//modo reproducción porque enciende ell mp3
				sprintf(buf_com, "%02d:%02d:%02d---> 7E FF 06 0B 00 00 00 EF\n\r", msg_pc.h,msg_pc.m,msg_pc.s);
				USARTdrv->Send(&buf_com,strlen(buf_com));
				osThreadFlagsWait (0x08, osFlagsWaitAny , osWaitForever);
			}
    }
}

 
/*----------------------------------------------------------------------------
 *                           Thread volumen com
 *---------------------------------------------------------------------------*/

extern osMessageQueueId_t mid_MsgQueue_mvol;  
 typedef struct { // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t_cvol;

MSGQUEUE_OBJ_t_cvol msg_com_vol;

osThreadId_t tid_Thread_cvol;                        // thread id
 
void Thread_cvol (void *argument);                   // thread function
 
int Init_Thread_cvol (void) {
 
  tid_Thread_cvol = osThreadNew(Thread_cvol, NULL, NULL);
  if (tid_Thread_cvol == NULL) {
    return(-1);
  }
 
  return(0);
}
int vol_com;
char n1=0;
char n2=0;

void Thread_cvol (void *argument) {

  while (1) {

		osMessageQueueGet(mid_MsgQueue_mvol, &msg_com_vol, NULL, osWaitForever);
		vol_com=msg_com_vol.Idx;
		if(vol_com==0) {n1='0';n2='0';}
		else if(vol_com==1) {n1='0';n2='1';}
		else if(vol_com==2) {n1='0';n2='2';}
		else if(vol_com==3) {n1='0';n2='3';}
		else if(vol_com==4) {n1='0';n2='4';}
		else if(vol_com==5) {n1='0';n2='5';}
		else if(vol_com==6) {n1='0';n2='6';}
		else if(vol_com==7) {n1='0';n2='7';}
		else if(vol_com==8) {n1='0';n2='8';}
		else if(vol_com==9) {n1='0';n2='9';}
		else if(vol_com==10) {n1='0';n2='A';}
		else if(vol_com==11) {n1='0';n2='B';}
		else if(vol_com==12) {n1='0';n2='C';}
		else if(vol_com==13) {n1='0';n2='D';}
		else if(vol_com==14) {n1='0';n2='E';}
		else if(vol_com==15) {n1='0';n2='F';}
		
		else if(vol_com==16) {n1='1';n2='0';}
		else if(vol_com==17) {n1='1';n2='1';}
		else if(vol_com==18) {n1='1';n2='2';}
		else if(vol_com==19) {n1='1';n2='3';}
		else if(vol_com==20) {n1='1';n2='4';}
		else if(vol_com==21) {n1='1';n2='5';}
		else if(vol_com==22) {n1='1';n2='6';}
		else if(vol_com==23) {n1='1';n2='7';}
		else if(vol_com==24) {n1='1';n2='8';}
		else if(vol_com==25) {n1='1';n2='9';}
		else if(vol_com==26) {n1='1';n2='A';}
		else if(vol_com==27) {n1='1';n2='B';}
		else if(vol_com==28) {n1='1';n2='C';}
		else if(vol_com==29) {n1='1';n2='D';}
		else if(vol_com==30) {n1='1';n2='E';}
		sprintf(buf_com, "%02d:%02d:%02d---> 7E FF 06 06 00 00 %c%c EF\n\r", msg_pc.h,msg_pc.m,msg_pc.s, n1,n2);
		USARTdrv->Send(&buf_com,strlen(buf_com));
		
	}
	
}

//prueba inicial de la hora
void pc(int h, int m, int s){
	sprintf(buf_com, "%02d:", h);
    USARTdrv->Send(&buf_com,sizeof(&buf_com));
	sprintf(buf_com, "%02d:",  m);
    USARTdrv->Send(&buf_com,sizeof(&buf_com));
	sprintf(buf_com, "%02d ",s);
    USARTdrv->Send(&buf_com,sizeof(&buf_com));
    osThreadFlagsWait (0x08, osFlagsWaitAny , osWaitForever);
}
