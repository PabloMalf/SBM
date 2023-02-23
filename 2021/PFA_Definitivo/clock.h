#ifndef CLOCK_H
#define CLOCK_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

#define MSGQUEUE_OBJECTS_clock 16

typedef enum {JAN = 1, FEB, MAR, APR, 
							MAY, JUNE, JULY, AUG, 
							SEPT, OCT, NOV, DEC
} month_t;

typedef enum {Monday, Tuesday, 
							Wensday, Thursday, 
							Friday, Saturday, Sunday
} today_t;

typedef struct {
	uint8_t seg;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	month_t month;
	uint16_t year; //From 0 AD to 4095 AD
} date_t;

typedef struct {
  date_t Date;
  uint8_t Id;
} MSGQUEUE_OBJ_clock_t;

extern date_t date;
extern uint8_t horas;
extern uint8_t minutos;
extern uint8_t segundos;
extern osThreadId_t tid_Th_clock;                        // thread id
extern osMessageQueueId_t mid_clock;

int Init_Th_clock (void);
void Th_clock (void *argument);

void timer_one_shot_1s_Callback(void *);

//EXTRAS
char* get_today(today_t);
int leap_year (int);
int number_days (date_t);
void update_date (date_t);



/**

	Flag 2: mandar por la cola de mensajes mid_reloj una actualizacion con la fecha actual

**/

#endif

