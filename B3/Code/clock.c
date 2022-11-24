#include "clock.h"

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
	hours = minutes = seconds = 0;
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
