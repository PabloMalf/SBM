#include "clock.h"

date_t date;

uint8_t horas;
uint8_t minutos;
uint8_t segundos;

osThreadId_t tid_Th_clock;                        // thread id
osMessageQueueId_t mid_clock;

static osTimerId_t Tmr_Periodic_1s;
static uint32_t flags;

void Th_clock (void *argument);                   // thread function

int Init_Msg_clock (void) {
  mid_clock = osMessageQueueNew(MSGQUEUE_OBJECTS_clock, sizeof(MSGQUEUE_OBJ_clock_t), NULL);
  if (mid_clock == NULL) {
    return (-1);
	}
  return(0);
}

int Init_Th_clock(void) {
  tid_Th_clock = osThreadNew(Th_clock, NULL, NULL);
  if (tid_Th_clock == NULL) {
    return(-1);
  }
  return(0);
}

void Th_clock (void *argument) {
	date.seg = 0;
	date.min = 0;
	date.hour = 0;
	date.day = 0;
	date.month = JAN;
	date.year = 2000;
	
	MSGQUEUE_OBJ_clock_t msg;
	Init_Msg_clock();
	
	Tmr_Periodic_1s = osTimerNew(timer_one_shot_1s_Callback, osTimerPeriodic, (void *)0, NULL);
	osTimerStart(Tmr_Periodic_1s, 1000U);
	
  while (1) {
		flags = osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
		if(((flags >> 1) & 0x01) == 1) {
			update_date(date);
			horas = date.hour;
			minutos = date.min;
			segundos = date.seg;
		}
		if(((flags >> 2) & 0x01) == 1){
			msg.Date = date;
			msg.Id = 1;
			osMessageQueuePut(mid_clock, &msg, 0U, 0U);
		}
    osThreadYield();
  }
}

void timer_one_shot_1s_Callback (void *argument)
{
		osThreadFlagsSet(tid_Th_clock, 0x01U);
}

void update_date (date_t a)
{
	a.seg ++;
	if (a.seg > 60) {
		a.seg = 0;
		a.min ++;
		if (a.min > 60) {
			a.min = 0;
			a.hour ++;
			if (a.hour > 23) {
				a.hour = 0;
				a.day ++;
				if (a.day > number_days(a)){
					a.day = 1;
					a.month ++;
					if (a.month > 12) {
						a.month = JAN;
						a.year ++;
						if(a.year == sizeof(a.year) - 1){
							//MILENIUM ERROR APROACHING
						}
					}
				}
			}
		}
	}	
}

int number_days (date_t a) 
{
	int out;
	switch (a.month) {
		case FEB:
			out = (28 + leap_year(a.year)); //LEAP_YEAR returns: 0 NO LEAP
																			//  								 1 LEAP
		break;
		
		case JAN: case MAR: case MAY: case JULY: case AUG: case OCT: case DEC:
			out = 31;
		break;
		
		case APR: case JUNE: case SEPT: case NOV:
			out = 30;
		break;
	}
	return out;
}


int leap_year (int a)
{
	int out;
	if (a % 4 != 0) 
		out = 0; //NO LEAP
	else if (a % 100 != 0)
		out = 1; //LEAP
	else if (a % 400 != 0)
		out = 0; //NO LEAP
	else 
		out = 1; //LEAP
	return out;
}

char* get_today(today_t a)
{
	char* c;
	switch (a){
		case Monday:
			c = "  Monday";
		break;
		  
		case Tuesday:
			c = " Tuesday";
		break;
		
		case Wensday:
			c = " Wensday";
		break;
		
		case Thursday:
			c = "Thursday";
		break;
		
		case Friday:
			c = "  Friday";
		break;
		
		case Saturday:
			c = " Saturday";
		break;
		
		case Sunday:
			c = "  Sunday";
		break;
	}
	return c;
}

