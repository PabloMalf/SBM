#include "mp3.h"


int carpeta=1;//variable auxiliar para seleccionar la carpeta
int cancion=0;//variable para saber la canción en la que estoy

/*----------------------------------------------------------------------------
 *                       Colas de mensajes del MP3
 *---------------------------------------------------------------------------*/
  
typedef struct {                                // object data type
  uint8_t Buf[32];
	uint8_t Idx;
	uint8_t carpeta;
} MSGQUEUE_OBJ_t;
MSGQUEUE_OBJ_t msg_mp3;

extern osMessageQueueId_t mid_MsgQueue_mp3;  

/*----------------------------------------------------------------------------
 *                   Callback del MP3 y definiciones
 *---------------------------------------------------------------------------*/
  
uint8_t buf_mp3[8];
GPIO_InitTypeDef GPIO_InitStruct_mp3;

osThreadId_t tid_myUSART6_Thread;//id del thread
 
/* USART Driver */
extern ARM_DRIVER_USART Driver_USART6;//definicion del nombre de la USART
static ARM_DRIVER_USART * USARTdrv = &Driver_USART6;//Variable del driver de la usart
ARM_USART_STATUS stat_mp3;// variable STATUS para saber si se ha liverado el buffer



void myUSART6_callback(uint32_t event)//se crea para ver los errores y los estados de los eventos
{
  uint32_t mask;
  mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |ARM_USART_EVENT_TRANSFER_COMPLETE |ARM_USART_EVENT_SEND_COMPLETE |ARM_USART_EVENT_TX_COMPLETE;//estados de la usart
  if (event & mask) {
    /* Success: Wakeup Thread */
    osThreadFlagsSet (tid_myUSART6_Thread, 0x02);//como se ha hecho bien mando el flag del thread
  }
  if (event & ARM_USART_EVENT_RX_TIMEOUT) {//ha habido un error porque ha acabado el tiempo del timeut
    __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
  }
  if (event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_TX_UNDERFLOW)) {//error porque supera el máximo y se desborda
    __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
  }
}


/*----------------------------------------------------------------------------
 *                                Thread mp3
 *---------------------------------------------------------------------------*/

///////////Init thread///////////
int Init_myUSART6_Thread (void) {
 
  tid_myUSART6_Thread = osThreadNew(myUSART6_Thread, NULL, NULL);
  if (tid_myUSART6_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}


/* CMSIS-RTOS Thread - UART command thread */
void myUSART6_Thread(void* args)
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
    USARTdrv->Initialize(myUSART6_callback);
    /*Power up the USART peripheral */
    USARTdrv->PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 9600 Bits/sec */
    USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |ARM_USART_DATA_BITS_8 |ARM_USART_PARITY_NONE |ARM_USART_STOP_BITS_1 |ARM_USART_FLOW_CONTROL_NONE, 9600);//en el mp3 es necesario 9600baudios
     
    /* Enable Receiver and Transmitter lines */
    USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
    USARTdrv->Control (ARM_USART_CONTROL_RX, 0);//desactivo la recepcion porque no recibo nada
 
    //////envio los comandos 
    //USARTdrv->Send(buf, 8);//primerlo lo mando y luego lo paro con el flag de abajo para esperarlo
//    USARTdrv->Send("\nPress Enter to receive a message", 34);
//    osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
//		SelectDevice();
//		carpeta1();
//    PlaySecondSong();
//		SetVolumen();
    while (1)
    {
			osMessageQueueGet(mid_MsgQueue_mp3, &msg_mp3, NULL, osWaitForever);
			if(msg_mp3.Idx==7){//modo reposo
				SleepMode();
			}
			if(msg_mp3.Idx==17){//modo reproducción porque enciende ell mp3
				WakeUp();
			}
			if(msg_mp3.Idx==1){//play
				Play();
				cancion=1;
			}
			if(msg_mp3.Idx==2){//pausa
				Pause();
			}
			if(msg_mp3.Idx==3){//siguiente canción
				NextSong();
				cancion++;
				
				if(cancion>3) 
					cancion=1;
			}
			if(msg_mp3.Idx==4){//anterior canción
				PreviousSong();
				cancion--;
				if(cancion<1) cancion=1;
			}
			if(msg_mp3.Idx==5){//carpeta anterior
				cancion=1;
				if( msg_mp3.carpeta > 1){
					
					if(msg_mp3.carpeta ==2){
						carpeta2();
					} 
					if(msg_mp3.carpeta ==1){
						carpeta1();
					}
				}
				else{
					carpeta1();
				}
			}
			if(msg_mp3.Idx==6){//carpeta siguiente
				cancion=1;
				if(msg_mp3.carpeta<3){
					
					if(msg_mp3.carpeta==2){
						carpeta2();
					}
					if(msg_mp3.carpeta==3){
						carpeta3();
					}
				}else {
					carpeta3();
				}
			}
			
    }
}

/*----------------------------------------------------------------------------
 *                           Thread volumen mp3
 *---------------------------------------------------------------------------*/

extern osMessageQueueId_t mid_MsgQueue_mvol;  
 typedef struct { // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t_mvol;

MSGQUEUE_OBJ_t_mvol msg_mp3_vol;

osThreadId_t tid_Thread_mvol;                        // thread id
 
void Thread_mvol (void *argument);                   // thread function
 
int Init_Thread_mvol (void) {
 
  tid_Thread_mvol = osThreadNew(Thread_mvol, NULL, NULL);
  if (tid_Thread_mvol == NULL) {
    return(-1);
  }
 
  return(0);
}
int vol_mp3;
extern int vol;
void Thread_mvol (void *argument) {

  while (1) {

		osMessageQueueGet(mid_MsgQueue_mvol, &msg_mp3_vol, NULL, osWaitForever);
		vol_mp3=msg_mp3_vol.Idx;
		Volume();
	}
}

/*----------------------------------------------------------------------------
 *                       Inicialización de pines MP3
 *---------------------------------------------------------------------------*/
/*Función de inicializacion de los pines*/
/*void Init_pin_mp3(void){
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	GPIO_InitStruct_mp3.Mode = GPIO_MODE_AF_PP;	
	GPIO_InitStruct_mp3.Pull = GPIO_PULLUP;
	GPIO_InitStruct_mp3.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct_mp3.Alternate = GPIO_AF8_USART6;
	
	GPIO_InitStruct_mp3.Pin = GPIO_PIN_9;//TX
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_mp3);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
	GPIO_InitStruct_mp3.Pin = GPIO_PIN_14;//RX
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_mp3);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET);
}*/

/*----------------------------------------------------------------------------
 *                       Comandos del MP3
 *---------------------------------------------------------------------------*/
void carpeta1(void){
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x0F;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x01;
	buf_mp3[6]=0x01;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void carpeta2(void){
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x0F;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x02;
	buf_mp3[6]=0x01;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void carpeta3(void){
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x0F;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x03;
	buf_mp3[6]=0x01;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}


void NextSong(void){//siguiente cancion
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x01;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x00;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void PreviousSong(void){//cancion anterior
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x02;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x00;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void PlayFirstSong(void){//suena la primera cancion
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x03;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x01;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void PlaySecondSong(void){//suena la segunda canción
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x03;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x02;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void VolumeUp(void){//sube una unidad el volumen
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x04;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x00;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void VolumeDownt(void){//baja una unidad el volumen 
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x05;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x00;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void Volume(void){//da el volumen 
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x06;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	
	if(vol_mp3==0) buf_mp3[6]=0x00;
	else if(vol_mp3==1) buf_mp3[6]=0x01;
	else if(vol_mp3==2) buf_mp3[6]=0x02;
	else if(vol_mp3==3) buf_mp3[6]=0x03;
	else if(vol_mp3==4) buf_mp3[6]=0x04;
	else if(vol_mp3==5) buf_mp3[6]=0x05;
	else if(vol_mp3==6) buf_mp3[6]=0x06;
	else if(vol_mp3==7) buf_mp3[6]=0x07;
	else if(vol_mp3==8) buf_mp3[6]=0x08;
	else if(vol_mp3==9) buf_mp3[6]=0x09;
	else if(vol_mp3==10) buf_mp3[6]=0x0A;
	else if(vol_mp3==11) buf_mp3[6]=0x0B;
	else if(vol_mp3==12) buf_mp3[6]=0x0C;
	else if(vol_mp3==13) buf_mp3[6]=0x0D;
	else if(vol_mp3==14) buf_mp3[6]=0x0E;
	else if(vol_mp3==15) buf_mp3[6]=0x0F;
	
	else if(vol_mp3==16) buf_mp3[6]=0x10;
	else if(vol_mp3==17) buf_mp3[6]=0x11;
	else if(vol_mp3==18) buf_mp3[6]=0x12;
	else if(vol_mp3==19) buf_mp3[6]=0x13;
	else if(vol_mp3==20) buf_mp3[6]=0x14;
	else if(vol_mp3==21) buf_mp3[6]=0x15;
	else if(vol_mp3==22) buf_mp3[6]=0x16;
	else if(vol_mp3==23) buf_mp3[6]=0x17;
	else if(vol_mp3==24) buf_mp3[6]=0x18;
	else if(vol_mp3==25) buf_mp3[6]=0x19;
	else if(vol_mp3==26) buf_mp3[6]=0x1A;
	else if(vol_mp3==27) buf_mp3[6]=0x1B;
	else if(vol_mp3==28) buf_mp3[6]=0x1C;
	else if(vol_mp3==29) buf_mp3[6]=0x1D;
	else if(vol_mp3==30) buf_mp3[6]=0x1E;
	
	
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	//osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void SetVolumen(void){//pone el volumen a 30
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x06;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x1E;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void SelectDevice(void){//selecciona la tarjeta
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x09;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x02;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void SleepMode(void){//MODO REPOSO
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x0A;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x00;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void WakeUp(void){//Se enciende el mp3
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x0B;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x00;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void Reset(void){//reinicia la tarjeta
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x0C;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x00;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void Play(void){//empieza
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x0D;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x00;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void Pause(void){//para
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x0E;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x00;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
void StopPlay(void){//detiene todo el mp3
	buf_mp3[0]=0x7E;
	buf_mp3[1]=0xFF;
	buf_mp3[2]=0x06;
	buf_mp3[3]=0x16;
	buf_mp3[4]=0x00;
	buf_mp3[5]=0x00;
	buf_mp3[6]=0x00;
	buf_mp3[7]=0xEF;
	USARTdrv->Send(&buf_mp3, 8);//todos los datos enviados son de longitud de 8bits
	osThreadFlagsWait (0x02, osFlagsWaitAny , osWaitForever);
}
