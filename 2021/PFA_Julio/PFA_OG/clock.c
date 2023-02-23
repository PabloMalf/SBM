#include "clock.h"

int segundos = 50;
int horas = 23;
int minutos = 59;

void Thread_clk (void *argument);
static void Timer_clk_Callback (void const *arg);

osTimerId_t tim_clk;
osThreadId_t tid_Thread_clk;
 
int Init_Thread_clk (void) {
  tid_Thread_clk = osThreadNew(Thread_clk, NULL, NULL);
  if (tid_Thread_clk == NULL) 
    return(-1);
  return(0);
}
 
void Thread_clk (void *argument) {
  tim_clk = osTimerNew((osTimerFunc_t)&Timer_clk_Callback, osTimerPeriodic, NULL, NULL);
	osThreadFlagsWait (0x01, osFlagsWaitAny , osWaitForever);
	osTimerStart(tim_clk, 1000U);
  while (1) {	
		osThreadYield();
  }
}

static void Timer_clk_Callback (void const *arg) {
	segundos++;
	if(segundos == 60){
		segundos = 0;
		minutos++;
		if(minutos == 60){
			minutos = 0;
			horas++;
			if(horas == 24){
				horas = 0;
			}
		}
	}
}
