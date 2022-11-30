#include "clock.h"
#include "lcd.h"
#include <stdio.h>

uint8_t hours;
uint8_t minutes;
uint8_t seconds;

static void Th_clock(void *argument);
static void Th_clock_test(void *argument);
static void Timer_Callback(void *argument);

static osTimerId_t tim_clock;
static osThreadId_t id_Th_clock;
static osThreadId_t id_Th_clock_test;
 
int Init_Th_clock(void) {
  id_Th_clock = osThreadNew(Th_clock, NULL, NULL);
  if (id_Th_clock == NULL)
    return(-1);
  return(0);
}

int Init_Th_clock_test(void) {
  id_Th_clock_test = osThreadNew(Th_clock, NULL, NULL);
  if (id_Th_clock_test == NULL)
    return(-1);
  return(0);
}
 
void Th_clock(void *argument){
	//hours = minutes = seconds = 0;
  tim_clock = osTimerNew(Timer_Callback, osTimerPeriodic, NULL, NULL);
	osTimerStart(tim_clock, 1000U);
  while(1)	
		osThreadYield();
}

static void Th_clock_test(void *argument){
	;
}

static void Timer_Callback(void *argument){
	seconds++;
	if(seconds == 60){
		seconds = 0;
		minutes++;
		if(minutes == 60){
			minutes = 0;
			hours++;
			if(hours == 24){
				hours = 0;
			}
		}
	}
}

void set_clock (uint8_t hour, uint8_t min, uint8_t sec) {
	hours = (hour < 24) ? hour : 0;
	minutes = (min < 60) ? min : 0;
	seconds = (sec < 60) ? sec : 0;
}

/*TEST*/

int Init_Th_clock_test(void){
	
	id_Th_clock_test= osThreadNew(Th_clock_test, NULL, NULL);
	if(id_Th_clock_test== NULL)
		return(-1);
	return(0);
}

static void Th_clock_test(void *arguments){

	Init_Th_clock();
	Init_Th_lcd();
	set_clock(20,26,0);
	
	
	MSGQUEUE_OBJ_LCD msg;
	
	msg.init_L1= 50;
	sprintf(msg.data_L1, "HORA");
	
	msg.init_L2=45;
	while(1){

		sprintf(msg.data_L2, "%.2u:%.2u:%.2u", hours, minutes, seconds);
		osMessageQueuePut(get_id_MsgQueue_lcd(), &msg, NULL, 0U);
		osDelay(50U);
	}
}

