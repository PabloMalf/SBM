#include "principal.h"

int puls =0;
int cnt_pausa=0;
int estado_inicial=0;
int s=0;
int m=0;
int num_carpeta=1;

extern int carpeta;
extern int cancion;
extern osTimerId_t tim_zumb;


//Timer tiempo de reproduccion de las canciones
osTimerId_t tim_repr;                            // timer id
static uint32_t exec_repr;                          // argument for the timer call back function

static void Timer_repr_Callback (void const *arg) {
  // add user code here
	s++;
	if(s>59){
		s = 0;
		m++;
		if(m>59){
			m=0;
		}
	}

}
// Create timers
int Init_Timer_repr (void) {
  // Create one-shoot timer
  exec_repr= 1U;
  tim_repr = osTimerNew((osTimerFunc_t)&Timer_repr_Callback, osTimerPeriodic, &exec_repr, NULL);
	
  return NULL;
}



/*----------------------------------------------------------------------------
 *                    Variables globales HORA Y TEMPERATURA
 *---------------------------------------------------------------------------*/
extern int segundos;
extern int horas;
extern int minutos;
extern osThreadId_t tid_Thread_clk;
extern float temperatura;

/*----------------------------------------------------------------------------
 *                       Colas de mensajes del COM
 *---------------------------------------------------------------------------*/

typedef struct {                                // object data type
  uint8_t Idx;
	uint8_t h;
	uint8_t m;
	uint8_t s;
} MSGQUEUE_OBJ_t_pc;
MSGQUEUE_OBJ_t_pc msg_principal_pc;

osMessageQueueId_t mid_MsgQueue_pc;                // message queue id
 
int Init_MsgQueue_pc(void) {
 
  mid_MsgQueue_pc = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t_pc), NULL);
  if (mid_MsgQueue_pc == NULL) {
    ; // Message Queue object not created, handle failure
  }
 
  return(0);
}
/*----------------------------------------------------------------------------
 *                       Colas de mensajes del volumen
 *---------------------------------------------------------------------------*/
 typedef struct { // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t_vol;

MSGQUEUE_OBJ_t_vol msg_principal_vol;

osMessageQueueId_t mid_MsgQueue_vol;                // message queue id
 
int Init_MsgQueue_vol(void) {
 
  mid_MsgQueue_vol = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t_vol), NULL);
  if (mid_MsgQueue_vol == NULL) {
    ; // Message Queue object not created, handle failure
  }
 
  return(0);
}

osMessageQueueId_t mid_MsgQueue_mvol;                // message queue id
 
int Init_MsgQueue_mvol(void) {
 
  mid_MsgQueue_mvol = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t_vol), NULL);
  if (mid_MsgQueue_vol == NULL) {
    ; // Message Queue object not created, handle failure
  }
 
  return(0);
}
////////////thread volumen//////
osThreadId_t tid_Thread_pvol;                        // thread id
 
void Thread_pvol (void *argument);                   // thread function
 
int Init_Thread_pvol (void) {
 
  tid_Thread_pvol = osThreadNew(Thread_pvol, NULL, NULL);
  if (tid_Thread_pvol == NULL) {
    return(-1);
  }
 
  return(0);
}
int vol;
 
void Thread_pvol (void *argument) {

  while (1) {
		
		osMessageQueueGet(mid_MsgQueue_vol, &msg_principal_vol, NULL, osWaitForever);
		if(vol!=msg_principal_vol.Idx){//solo entra si cambia el valor 
			vol=msg_principal_vol.Idx;
			osMessageQueuePut(mid_MsgQueue_mvol, &msg_principal_vol, NULL, 0U);//pongo el mensaje a la cola
		}
	}
}
/*----------------------------------------------------------------------------
 *                       Colas de mensajes del LCD
 *---------------------------------------------------------------------------*/
 typedef struct { // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t_lcd;
MSGQUEUE_OBJ_t_lcd msg_principal_lcd;

osMessageQueueId_t mid_MsgQueue_lcd;                // message queue id LCD
 
int Init_MsgQueue_lcd(void) {
 
  mid_MsgQueue_lcd = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t_lcd), NULL);
  if (mid_MsgQueue_lcd == NULL) {
    ; // Message Queue object not created, handle failure
  }
 
  return(0);
}

//////////////////////////////////Pintar REPOSO///////////////////////

void pintar_hora(void){//Funvión auxiliar para pintar la hora en el centro del LCD
	
	char c_hora [256];
	int l_hora = sprintf(c_hora, "%02d : %02d : %02d", horas, minutos, segundos);
	for(int i=0; i<l_hora; i++){
		symbolToLocalBuffer(2, c_hora[i]);
	}	
}
void pintar_temperatura(void){//Funvión auxiliar para pintar la hora en el centro del LCD
	
	char c_temp [256];
	
	int l_temp = sprintf(c_temp, "  SBM2021  T:%1.1fC", temperatura);
	for(int j=0; j<l_temp; j++){
		symbolToLocalBuffer(1, c_temp[j]);
	}	
}
///////////////////////////////Pintar REPRODUCCION////////////////////

void pintar_Trepr(void){
	
	char c_Trepr [256];
	
	int l_trepr = sprintf(c_Trepr, "Trepr: %02d:%02d", m, s);
	for(int i=0; i<l_trepr; i++){
		symbolToLocalBuffer(2, c_Trepr[i]);
	}	
}
void pintar_mp3(void){
	
	char c_mp3 [256];
	
	int l_mp3 = sprintf(c_mp3, " F:%02d  C:%02d  Vol:%02d",num_carpeta, cancion, vol);
	for(int j=0; j<l_mp3; j++){
		symbolToLocalBuffer(1, c_mp3[j]);
	}	
}

/*----------------------------------------------------------------------------
 *                       Colas de mensajes del Joystick
 *---------------------------------------------------------------------------*/
 typedef struct { // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t_joy;//lo uso para las colas del joystick
//COLA
MSGQUEUE_OBJ_t_joy msg_principal;

osMessageQueueId_t mid_MsgQueue_joy;                // message queue id
 
int Init_MsgQueue_joy (void) {
 
  mid_MsgQueue_joy = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t_joy), NULL);
  if (mid_MsgQueue_joy == NULL) {
    ; // Message Queue object not created, handle failure
  }
 
  return(0);
}


/*----------------------------------------------------------------------------
 *                       Colas de mensajes del RGBLED
 *---------------------------------------------------------------------------*/
 typedef struct { // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t_led;//lo uso para las colas del joystick
MSGQUEUE_OBJ_t_led msg_principal_led;

osMessageQueueId_t mid_MsgQueue_led;                // message queue id
 
int Init_MsgQueue_led (void) {
 
  mid_MsgQueue_led = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t_led), NULL);
  if (mid_MsgQueue_led== NULL) {
    ; // Message Queue object not created, handle failure
  }
 
  return(0);
}


/*----------------------------------------------------------------------------
 *                       Colas de mensajes del MP3
 *---------------------------------------------------------------------------*/
 typedef struct { // object data type
  uint8_t Buf[32];
  uint8_t Idx;
	uint8_t carpeta;
} MSGQUEUE_OBJ_t_mp3;//lo uso para las colas del joystick
MSGQUEUE_OBJ_t_mp3 msg_principal_mp3;

osMessageQueueId_t mid_MsgQueue_mp3;                // message queue id
 
int Init_MsgQueue_mp3 (void) {
 
  mid_MsgQueue_mp3 = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t_mp3), NULL);
  if (mid_MsgQueue_mp3 == NULL) {
    ; // Message Queue object not created, handle failure
  }
 
  return(0);
}


/*----------------------------------------------------------------------------
 *                      		PRINCIPAL
 *---------------------------------------------------------------------------*/
osThreadId_t tid_Thread_principal;                        // thread id
 
void Thread_principal (void *argument);                   // thread function
 
int Init_Thread_principal (void) {
 
  tid_Thread_principal = osThreadNew(Thread_principal, NULL, NULL);
  if (tid_Thread_principal == NULL) {
    return(-1);
  }
 
  return(0);
}

void Thread_principal (void *argument) {
	if (estado_inicial==0){
		osThreadFlagsSet (tid_Thread_clk, 0x01);
		msg_principal_lcd.Idx = 0;//id del reposo para el LCD
		osMessageQueuePut(mid_MsgQueue_lcd, &msg_principal_lcd, NULL, 0U);//pongo el mensaje a la cola
	}
  while (1) {
		osThreadFlagsWait (0x20, osFlagsWaitAny , osWaitForever);//me llega del joystick para interrumpir la cola
		osMessageQueueGet(mid_MsgQueue_joy, &msg_principal, NULL, osWaitForever);
		Init_PWM();
		osTimerStart(tim_zumb, 100U); //1s para el zumbador
		if(msg_principal.Idx==6 && puls==2){//Estado de reposo->pulsación larga del central
			
			msg_principal_led.Idx=0;//identificador del modo reposo
			osMessageQueuePut(mid_MsgQueue_led, &msg_principal_led, NULL, 0U);//pongo el mensaje a la cola
			
			puls = 0; 
			osThreadFlagsSet (tid_Thread_clk, 0x01);
			msg_principal_lcd.Idx=0;//id del reposo para el LCD
			osMessageQueuePut(mid_MsgQueue_lcd, &msg_principal_lcd, NULL, 0U);//pongo el mensaje a la cola
			
			msg_principal_mp3.Idx=7;//ifentificador del modo reposo para el mp3
			osMessageQueuePut(mid_MsgQueue_mp3, &msg_principal_mp3, NULL, 0U);//pongo el mensaje a la cola
			
			msg_principal_pc.Idx=7;//identificador del modo reposo del com:paso las horas los segundos y los minutos
			msg_principal_pc.h=horas;
			msg_principal_pc.m=minutos;
			msg_principal_pc.s=segundos;
			osMessageQueuePut(mid_MsgQueue_pc, &msg_principal_pc, NULL, 0U);//pongo el mensaje a la cola
			
		}
		
		if(msg_principal.Idx==6 && puls==1){//estado del modo reproduccion->pulsación larga del central
			
			s=0; 
			m=0;
			msg_principal_led.Idx=2;//identificador del modo pausa
			osMessageQueuePut(mid_MsgQueue_led, &msg_principal_led, NULL, 0U);//pongo el mensaje a la cola
			
			
			osTimerStart(tim_repr, 1000U); //cuenta de 1s para contar los segundos
			msg_principal_lcd.Idx=1;//id del modo reproduccion  para el LCD
			osMessageQueuePut(mid_MsgQueue_lcd, &msg_principal_lcd, NULL, 0U);//pongo el mensaje a la cola
			
			msg_principal_mp3.Idx=17;//id que le mando al mp3 para que cuando empiece la reproducción empiece a sonar la primera canción de la primera carpeta
			osMessageQueuePut(mid_MsgQueue_mp3, &msg_principal_mp3, NULL, 0U);//pongo el mensaje a la cola

			cnt_pausa=1;//esta en pausa
			
			msg_principal_pc.Idx=17;//identificador del modo reposo del com:paso las horas los segundos y los minutos
			msg_principal_pc.h=horas;
			msg_principal_pc.m=minutos;
			msg_principal_pc.s=segundos;
			osMessageQueuePut(mid_MsgQueue_pc, &msg_principal_pc, NULL, 0U);//pongo el mensaje a la cola
		}
		
		
		if(msg_principal.Idx==1 && puls==1){//se selecciona la carpeta anterior->pusador arriba
			if(num_carpeta>1){ 
				num_carpeta--;
				msg_principal_mp3.carpeta=num_carpeta;
			}
			
			msg_principal_mp3.Idx=5;//id para que pase a la anterior carpeta 
			osMessageQueuePut(mid_MsgQueue_mp3, &msg_principal_mp3, NULL, 0U);//pongo el mensaje a la cola
			msg_principal_led.Idx=1;//identificador del modo play para el led
			osMessageQueuePut(mid_MsgQueue_led, &msg_principal_led, NULL, 0U);//pongo el mensaje a la cola
			
			cnt_pausa=0;
			
			msg_principal_pc.Idx=5;//identificador del modo reposo del com:paso las horas los segundos y los minutos
			msg_principal_pc.h=horas;
			msg_principal_pc.m=minutos;
			msg_principal_pc.s=segundos;
			osMessageQueuePut(mid_MsgQueue_pc, &msg_principal_pc, NULL, 0U);//pongo el mensaje a la cola
		}
		
		if(msg_principal.Idx==2 && puls==1){// indica que pase a la siguiente cancion->pulsador derecho
			msg_principal_led.Idx=1;//identificador del modo play para el led
			osMessageQueuePut(mid_MsgQueue_led, &msg_principal_led, NULL, 0U);//pongo el mensaje a la cola
			
			msg_principal_mp3.Idx=3;//id para que pase a la siguiente canción de la carpeta actual
			osMessageQueuePut(mid_MsgQueue_mp3, &msg_principal_mp3, NULL, 0U);//pongo el mensaje a la cola
			
			cnt_pausa=0;
			
			msg_principal_pc.Idx=3;//identificador del modo reposo del com:paso las horas los segundos y los minutos
			msg_principal_pc.h=horas;
			msg_principal_pc.m=minutos;
			msg_principal_pc.s=segundos;
			osMessageQueuePut(mid_MsgQueue_pc, &msg_principal_pc, NULL, 0U);//pongo el mensaje a la cola
		}
		
		if(msg_principal.Idx==3 && puls==1){//se selecciona la siguiente carpeta->pusador abajo
			if(num_carpeta<3){
				num_carpeta++;
				msg_principal_mp3.carpeta=num_carpeta;
			}
			msg_principal_mp3.Idx=6;//id para que pase a la siguiente carpeta
			osMessageQueuePut(mid_MsgQueue_mp3, &msg_principal_mp3, NULL, 0U);//pongo el mensaje a la cola
			msg_principal_led.Idx=1;//identificador del modo play para el led
			osMessageQueuePut(mid_MsgQueue_led, &msg_principal_led, NULL, 0U);//pongo el mensaje a la cola
			
			cnt_pausa=0;
			
			msg_principal_pc.Idx=6;//identificador del modo reposo del com:paso las horas los segundos y los minutos
			msg_principal_pc.h=horas;
			msg_principal_pc.m=minutos;
			msg_principal_pc.s=segundos;
			osMessageQueuePut(mid_MsgQueue_pc, &msg_principal_pc, NULL, 0U);//pongo el mensaje a la cola
		}
		
		if(msg_principal.Idx==4 && puls==1){//indica que pase a la canción anterior ->pulsa izquierda
			msg_principal_led.Idx=1;//identificador del modo play para el led
			osMessageQueuePut(mid_MsgQueue_led, &msg_principal_led, NULL, 0U);//pongo el mensaje a la cola
			msg_principal_mp3.Idx=4;//id para que pase a la siguiente canción de la carpeta actual
			osMessageQueuePut(mid_MsgQueue_mp3, &msg_principal_mp3, NULL, 0U);//pongo el mensaje a la cola
			
			cnt_pausa=0;
			
			msg_principal_pc.Idx=4;//identificador del modo reposo del com:paso las horas los segundos y los minutos
			msg_principal_pc.h=horas;
			msg_principal_pc.m=minutos;
			msg_principal_pc.s=segundos;
			osMessageQueuePut(mid_MsgQueue_pc, &msg_principal_pc, NULL, 0U);//pongo el mensaje a la cola
		}
		
		if(msg_principal.Idx==5 && puls==1){//modo pausa o reproducción-> se hace con pulsación corta del central
			cnt_pausa++;
			if( cnt_pausa==2){//se vuelve a poner a play
				cnt_pausa=0;
				msg_principal_led.Idx=1;//identificador del modo play para el led
				osMessageQueuePut(mid_MsgQueue_led, &msg_principal_led, NULL, 0U);//pongo el mensaje a la cola
				msg_principal_mp3.Idx=1;//identificador del modo play para el mp3
				osMessageQueuePut(mid_MsgQueue_mp3, &msg_principal_mp3, NULL, 0U);//pongo el mensaje a la cola	
				
				msg_principal_pc.Idx=1;//identificador del modo reposo del com:paso las horas los segundos y los minutos
				msg_principal_pc.h=horas;
				msg_principal_pc.m=minutos;
				msg_principal_pc.s=segundos;
				osMessageQueuePut(mid_MsgQueue_pc, &msg_principal_pc, NULL, 0U);//pongo el mensaje a la cola
				
			}
			if(cnt_pausa==1){//se vuelve a poner en pausa
				msg_principal_led.Idx=2;//identificador del modo pausa para el led
				osMessageQueuePut(mid_MsgQueue_led, &msg_principal_led, NULL, 0U);//pongo el mensaje a la cola
				msg_principal_mp3.Idx=2;//ifentificador del modo pausa para el mp3
				osMessageQueuePut(mid_MsgQueue_mp3, &msg_principal_mp3, NULL, 0U);//pongo el mensaje a la cola
				
				msg_principal_pc.Idx=2;//identificador del modo reposo del com:paso las horas los segundos y los minutos
				msg_principal_pc.h=horas;
				msg_principal_pc.m=minutos;
				msg_principal_pc.s=segundos;
				osMessageQueuePut(mid_MsgQueue_pc, &msg_principal_pc, NULL, 0U);//pongo el mensaje a la cola
			}
			
		}
		
	}
}

