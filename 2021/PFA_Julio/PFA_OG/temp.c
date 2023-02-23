#include "temp.h"

 
/* Instanciación I2C driver */
extern ARM_DRIVER_I2C            Driver_I2C1;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;
static volatile uint32_t I2C_Event;//eventos
float temperatura = 0;//variable global de la temperatura 


osThreadId_t tid_Thread_temp;//id del thread

/*----------------------------------------------
 *                 Timers temperatura
 *----------------------------------------------*/ 



osTimerId_t tim_temp;                            // timer id
static uint32_t exec_temp;                          // argument for the timer call back function

static void Timer_temp_Callback (void const *arg) {
  // add user code here
	osThreadFlagsSet(tid_Thread_temp,0x10);
}
// Create timers
int Init_Timer_temp(void) {
  // Create one-shoot timer
  exec_temp= 5U;
  tim_temp= osTimerNew((osTimerFunc_t)&Timer_temp_Callback, osTimerPeriodic, &exec_temp, NULL);
	
  return NULL;
}
/*----------------------------------------------------------------------------
 *                                Thread temperaura
 *---------------------------------------------------------------------------*/

///////////Init thread///////////
int Init_Thread_temp (void) {
 
  tid_Thread_temp = osThreadNew(Thread_temp, NULL, NULL);
  if (tid_Thread_temp== NULL) {
    return(-1);
  }
 
  return(0);
}


void Thread_temp (void *argument) {
	uint8_t buf_temp[2];//buffer para almacenar la temperatura
	uint8_t cmd=0;//variable para el comando
	
	osTimerStart(tim_temp,1000U);//empiezo para las medidas continuas cada segundo
	init_temp();
  while (1) {
		osThreadFlagsWait(0x10,osFlagsWaitAny, osWaitForever);
			
		/*Primero mando el comando*/
		I2Cdrv->MasterTransmit (I2C_ADDR,&cmd , 2, true);
		/* Wait until transfer completed */
		while (I2Cdrv->GetStatus().busy);
		/*Despues recibo la temperatura*/
		I2Cdrv->MasterReceive (I2C_ADDR, buf_temp, 2, false);//leo el registro de la remperatura
		/* Wait until transfer completed */
		while (I2Cdrv->GetStatus().busy);
		
    temperatura=((buf_temp[0]<<8)|(buf_temp[1]))>>5;//MSB|LSB
		temperatura = (temperatura)*0.125; //temp=(suma de los complementos de las temp)*0.125ºC
  }
}

/*----------------------------------------------------------------------------
 *                          Init driver temperatura
 *---------------------------------------------------------------------------*/

void init_temp(void){
	/* Inicializar periférico I2C */
	I2Cdrv-> Initialize(NULL);
	/* Encendido periférico I2C */
	I2Cdrv-> PowerControl ( ARM_POWER_FULL );
	/*Control*/
	I2Cdrv->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  I2Cdrv->Control      (ARM_I2C_BUS_CLEAR, 0);
}
