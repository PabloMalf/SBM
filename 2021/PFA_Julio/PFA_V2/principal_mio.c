#include "principal.h"

/*--------------------------------*
 *					LCD
 *--------------------------------*/
static MSGQUEUE_OBJ_LCD msg_lcd;
extern osMessageQueueId_t mid_MsgQueue_lcd;


/*--------------------------------*
 *					CLOCK
 *--------------------------------*/
extern int hours;
extern int minutes;
extern int seconds;


/*--------------------------------*
 *					JOYSTICK
 *--------------------------------*/
static MSGQUEUE_OBJ_JOY msg_joy;
extern osMessageQueueId_t mid_MsgQueue_joy;


/*--------------------------------*
 *					RGB
 *--------------------------------*/
static MSGQUEUE_OBJ_RGB msg_rgb;
extern osMessageQueueId_t mid_MsgQueue_rgb;


/*--------------------------------*
 *					I2C (TSL2591)
 *--------------------------------*/
static MSGQUEUE_OBJ_I2C msg_i2c;
extern osMessageQueueId_t mid_MsgQueue_I2C;
extern osThreadId_t tid_Th_I2C;


/*--------------------------------*
 *					COM
 *--------------------------------*/
static MSGQUEUE_OBJ_COM msg_com_rx;
static MSGQUEUE_OBJ_COM msg_com_tx;
extern osMessageQueueId_t mid_MsgQueue_com_rx;
extern osMessageQueueId_t mid_MsgQueue_com_tx;


/*--------------------------------*
 *					BUFFER
 *--------------------------------*/
typedef struct{
	uint8_t hora;
	uint8_t min;
	uint8_t seg;
	uint8_t lum_cen; //‰ es un por millaje ...suck it
	uint8_t lum_dec;
	uint8_t lum_uni;
}time_lum_pack_type;

typedef struct{
	uint8_t capacidad;
	uint8_t elementos;
	uint8_t ultima_entrada;
	time_lum_pack_type* pos;
}buffer_t;

static time_lum_pack_type* parse_data_rx(MSGQUEUE_OBJ_I2C msg){
	time_lum_pack_type* data;
	data->hora = hours;
	data->min = minutes;
	data->seg = seconds;
	data->lum_cen = msg.light_cen;
	data->lum_dec = msg.light_dec;
	data->lum_uni = msg.light_uni;
	return data;
}

static buffer_t* crearBuffer(uint16_t _capacidad){
	buffer_t* buffer = malloc(sizeof(buffer_t));
	buffer->pos	= malloc(_capacidad*sizeof(time_lum_pack_type));
	if(buffer->pos == NULL)return NULL;
	buffer->capacidad =_capacidad;
	buffer->elementos = 0;
	buffer->ultima_entrada = 0;
	return buffer;
}

static time_lum_pack_type* introducirDatoBuffer(buffer_t* buffer,  time_lum_pack_type *dato){
	memcpy((buffer->pos + (buffer->ultima_entrada* sizeof(time_lum_pack_type))), dato, sizeof(time_lum_pack_type));
	buffer->ultima_entrada = (buffer->ultima_entrada + 1) % buffer->capacidad;
	if(buffer->elementos < buffer->capacidad)
		buffer->elementos++;
	return (buffer->pos + (buffer->ultima_entrada * sizeof(time_lum_pack_type)));
}

static time_lum_pack_type* borrarDatosBuffer(buffer_t* buffer){
	buffer->elementos = 0;
	buffer->ultima_entrada = 0;
	return buffer->pos;
}

static int *recogerDatoBuffer(buffer_t* buffer, time_lum_pack_type *medida, uint8_t n_elemento){
	if(n_elemento >buffer->elementos) return NULL;
	return memcpy(medida, buffer->pos+n_elemento*sizeof(time_lum_pack_type), sizeof(time_lum_pack_type));
}


/*--------------------------------*
 *					PRINCIPAL
 *--------------------------------*/
typedef enum{UNLOCKED = 0, LOCKED}lock_t;
typedef enum{INACTIVO = 0, ACTIVO, MEDIDA}estados_t;
static uint8_t tiempo_entre_ciclos;
static uint8_t numero_ciclos;
static uint8_t ciclos_left;
static uint8_t countdown;
static estados_t estado;
static lock_t lock;

static buffer_t* buffer;

osThreadId_t tid_Th_principal;
 
static void Th_principal (void *argument);
static void hour_setup(MSGQUEUE_OBJ_COM msg);
static void countdown_setup(MSGQUEUE_OBJ_COM msg);
static void countdown_read(MSGQUEUE_OBJ_COM msg); 
static void start_measure_cycle(MSGQUEUE_OBJ_COM msg);
static void number_measures(MSGQUEUE_OBJ_COM msg);
static void last_measure(MSGQUEUE_OBJ_COM msg);
static MSGQUEUE_OBJ_COM measureToMsg(MSGQUEUE_OBJ_COM *msg, uint8_t entrada);
static osTimerId_t Tmr_countdown;
static osTimerId_t Tmr_measures;

static void tmr_countdown_Callback (void* argument);
static void tmr_measures_Callback (void* argument);

int Init_Th_principal (void) {
  tid_Th_principal = osThreadNew(Th_principal, NULL, NULL);
  if (tid_Th_principal == NULL)
    return(-1);
  return(0);
}

static void Th_principal (void *argument) {
	osStatus_t status_msg;
	msg_rgb.estado_rgb = ON;
	osMessageQueuePut(mid_MsgQueue_rgb, &msg_rgb, NULL, 0U);
	msg_lcd.init_L1 = 30;
	msg_lcd.init_L2 = 15;
	countdown = 9;
	lock = UNLOCKED;
	estado = MEDIDA;
	Tmr_countdown = osTimerNew(tmr_countdown_Callback, osTimerOnce, (void *)0, NULL);
	Tmr_measures = osTimerNew(tmr_measures_Callback, osTimerPeriodic, (void *)0, NULL);
	buffer = crearBuffer(BUFFER_SIZE);
  while (1) {
		switch (estado){
			case INACTIVO:
				sprintf(msg_lcd.data_L1, "%02d : %02d : %02d", hours, minutes, seconds);
				sprintf(msg_lcd.data_L2, "ESTADO INACTIVO");
				osMessageQueuePut(mid_MsgQueue_lcd, &msg_lcd, NULL, 0U);
			
				status_msg = osMessageQueueGet(mid_MsgQueue_joy, &msg_joy, NULL, 0U);
				if((status_msg == osOK) && (msg_joy.tecla == Arriba) && (msg_joy.duracion == Larga)){
					msg_rgb.estado_led2 = ON;
					msg_rgb.estado_led3 = OFF;
					osMessageQueuePut(mid_MsgQueue_rgb, &msg_rgb, NULL, 0U);
					msg_lcd.init_L2 = 20;
					estado = ACTIVO;
				}
			break;
			
			case ACTIVO:
				sprintf(msg_lcd.data_L1, "%02d : %02d : %02d", hours, minutes, seconds);
				sprintf(msg_lcd.data_L2, "ESTADO ACTIVO");
				osMessageQueuePut(mid_MsgQueue_lcd, &msg_lcd, NULL, 0U);		
				
				status_msg = osMessageQueueGet(mid_MsgQueue_com_rx, &msg_com_rx, NULL, 0U);
				if(status_msg == osOK){
					switch (msg_com_rx.op){
						case HOUR_SETUP:
							hour_setup(msg_com_rx);
						break;
						
						case COUNTDOWN_SETUP:
							countdown_setup(msg_com_rx);
						break;
						
						case COUNTDOWN_READ:
							countdown_read(msg_com_rx);
						break;
						
						default:
							
						break;
					}
				}				
				status_msg = osMessageQueueGet(mid_MsgQueue_joy, &msg_joy, NULL, 0U);
				if((status_msg == osOK) && (msg_joy.tecla == Arriba) && (msg_joy.duracion == Larga)){
					msg_rgb.estado_led2 = OFF;
					msg_rgb.estado_led3 = OFF;
					osMessageQueuePut(mid_MsgQueue_rgb, &msg_rgb, NULL, 0U);
					estado = INACTIVO;
				}
			break;
			
			case MEDIDA:
				sprintf(msg_lcd.data_L1, "%02d : %02d : %02d", hours, minutes, seconds);
				osMessageQueuePut(mid_MsgQueue_lcd, &msg_lcd, NULL, 0U);
				if(lock == UNLOCKED){//En mitad de ciclo medidas
					//MANUAL
					status_msg = osMessageQueueGet(mid_MsgQueue_joy, &msg_joy, NULL, 0U);
					if((status_msg == osOK) && (msg_joy.tecla == Abajo) && (msg_joy.duracion == Corta)){
						osThreadFlagsSet(tid_Th_I2C, INIT_LIGHT_MEASURE);
						osMessageQueueGet(mid_MsgQueue_I2C, &msg_i2c, NULL, 50U);
						msg_rgb.estado_led3 = ON;
						osMessageQueuePut(mid_MsgQueue_rgb, &msg_rgb, NULL, 0U);
						//introducirDatoBuffer(buffer, parse_data_rx(msg_i2c));
						msg_lcd.init_L2 = 35;
						sprintf(msg_lcd.data_L2, "Luz: %d%d.%d%%", msg_i2c.light_cen, msg_i2c.light_dec,msg_i2c.light_uni); 
					}
					if((status_msg == osOK) && (msg_joy.tecla == Arriba) && (msg_joy.duracion == Corta)){
						msg_lcd.init_L2 = 20;
						sprintf(msg_lcd.data_L2, "C Luz: %d%d.%d%%", msg_i2c.light_cen, msg_i2c.light_dec,msg_i2c.light_uni);
					}
					//COMS
					status_msg = osMessageQueueGet(mid_MsgQueue_com_rx, &msg_com_rx, NULL, 0U);
					if(status_msg == osOK){	
						switch (msg_com_rx.op){
							case START_MEASURE:
								start_measure_cycle(msg_com_rx);
							break;
							
							case RET_NUM_MEASURE:
								number_measures(msg_com_rx);
							break;
								
							case RET_LAST_MEASURE:
								last_measure(msg_com_rx);
							break;
							
							case RET_ALL_MEASURE:
								
							break;
							
							case DEL_ALL_MEASURE:

							break;
							
							default:	

							break;
						}
					}
				}
			break;
		}
	}
}


static void hour_setup(MSGQUEUE_OBJ_COM msg){ 
    char data[32];
    int index, index_p = 0;
    int horas, minutos, segundos;

    memcpy(data, msg.data, msg.size);
	
    index = strchr(data,':') - data + 1;	//Devuelve un puntero a 
    memcpy(data, data,(index - index_p));
    horas = atoi(data);
    
    index_p = index;
    memcpy(data, msg.data + index_p, (msg.size - index_p));

    index = strchr(data,':') - data + 1;
    memcpy(data, data,(index - index_p));
    minutos = atoi(data);
    
    index_p = index;
    memcpy(data, msg.data + index_p, (msg.size - index_p));

    index = strchr(data,':') - data + 1;
    memcpy(data, data,(index - index_p));
    segundos = atoi(data);;

		lock = UNLOCKED;
		estado = MEDIDA;
		set_clock(horas,minutos,segundos);
		sprintf(msg_lcd.data_L2, "ESTADO MEDIDA");		
		osMessageQueuePut(mid_MsgQueue_com_tx, &msg, NULL, 0U);
}

static void countdown_setup(MSGQUEUE_OBJ_COM msg){ 
	countdown = atoi(msg.data);
	osMessageQueuePut(mid_MsgQueue_com_tx, &msg, NULL, 0U);
}

static void countdown_read(MSGQUEUE_OBJ_COM msg){ 
	msg.data[0] = countdown;
	msg.size = 1;
	osMessageQueuePut(mid_MsgQueue_com_tx, &msg, NULL, 0U);
}

static void start_measure_cycle(MSGQUEUE_OBJ_COM msg){ 
	char data[32];
	int index_N, index_T;

	index_N = strchr(msg.data,'N') - msg.data + 1;
	index_T = strchr(msg.data,'T') - msg.data + 1;

	memcpy(data, msg.data + index_N, (index_T - index_N - 1));
	numero_ciclos = atoi(data);

	memset(data,0,sizeof(data));
	memcpy(data, (msg.data + index_T), (msg.size - index_T - 1));      
	tiempo_entre_ciclos = atoi(data);
	
	tiempo_entre_ciclos = tiempo_entre_ciclos * 1000;
	
	osTimerStart(Tmr_countdown, countdown);
}

static void tmr_countdown_Callback (void* argument){
	osThreadFlagsSet(tid_Th_I2C, INIT_LIGHT_MEASURE);
	osMessageQueueGet(mid_MsgQueue_I2C, &msg_i2c, NULL, 50U);
	msg_rgb.estado_led3 = ON;
	osMessageQueuePut(mid_MsgQueue_rgb, &msg_rgb, NULL, 0U);
	introducirDatoBuffer(buffer, parse_data_rx(msg_i2c));
	sprintf(msg_lcd.data_L2, "C Luz: %d%d%d", msg_i2c.light_cen, msg_i2c.light_dec, msg_i2c.light_uni); 
	ciclos_left = numero_ciclos;
	osTimerStart(Tmr_measures, tiempo_entre_ciclos);
}

static void tmr_measures_Callback (void* argument) {
	ciclos_left --;
	if(ciclos_left != 0){
		osThreadFlagsSet(tid_Th_I2C, INIT_LIGHT_MEASURE);
		osMessageQueueGet(mid_MsgQueue_I2C, &msg_i2c, NULL, 50U);
		msg_rgb.estado_led3 = ON;
		osMessageQueuePut(mid_MsgQueue_rgb, &msg_rgb, NULL, 0U);
		introducirDatoBuffer(buffer, parse_data_rx(msg_i2c));
		sprintf(msg_lcd.data_L2, "C Luz: %d%d%d", msg_i2c.light_cen, msg_i2c.light_dec, msg_i2c.light_uni); 
	}
	else{
		lock = UNLOCKED;
		osTimerStop(Tmr_measures);
	}
}

static void number_measures(MSGQUEUE_OBJ_COM msg){
	msg.data[0]=buffer->elementos;
	osMessageQueuePut(mid_MsgQueue_com_tx, &msg, NULL, 0U);
}

static void last_measure(MSGQUEUE_OBJ_COM msg){
	msg.Number=0;
//	msg.op=0x40;
//	time_lum_pack_type medida;
//	recogerDatoBuffer(buffer,&medida, buffer->ultima_entrada);
//	msg.data[0]=medida.hora%10;
//	msg.data[1]=medida.hora-msg.data[0];
//	msg.data[2]=0x3A;
//	msg.data[3]=medida.min%10;
//	msg.data[4]=medida.min-msg.data[3];
//	msg.data[5]=0x3A;
//	msg.data[6]=medida.seg%10;
//	msg.data[7]=medida.seg-msg.data[6];
//	msg.data[8]=0x2D;
//	msg.data[9]=medida.lum_cen;
//	msg.data[10]=medida.lum_dec;
//	msg.data[11]=0x2E;
//	msg.data[12]=medida.lum_uni;
//	msg.data[13]=0x25;
//	msg.size=14;
	measureToMsg(&msg, buffer->ultima_entrada);
	osMessageQueuePut(mid_MsgQueue_com_tx, &msg, NULL, 0U);
}
static MSGQUEUE_OBJ_COM measureToMsg(MSGQUEUE_OBJ_COM *msg, uint8_t entrada){
	//msg.Number=0;
	msg->op=0x40;
	time_lum_pack_type medida;
	recogerDatoBuffer(buffer,&medida, entrada);
	msg->data[0]=medida.hora%10;
	msg->data[1]=medida.hora-msg->data[0];
	msg->data[2]=0x3A;
	msg->data[3]=medida.min%10;
	msg->data[4]=medida.min-msg->data[3];
	msg->data[5]=0x3A;
	msg->data[6]=medida.seg%10;
	msg->data[7]=medida.seg-msg->data[6];
	msg->data[8]=0x2D;
	msg->data[9]=medida.lum_cen;
	msg->data[10]=medida.lum_dec;
	msg->data[11]=0x2E;
	msg->data[12]=medida.lum_uni;
	msg->data[13]=0x25;
	msg->size=14;
	return *msg;
}
