#include "clock.h"
#include "stm32f4xx_hal.h"

uint32_t sec;

static void Th_clock(void *argument);
static void Th_clock_test(void *argument);
static void Timer_Callback(void *argument);
static void led_init(void);

static osTimerId_t tim_clock;
static osThreadId_t id_Th_clock;
static osThreadId_t id_Th_clock_test;
 
int Init_Th_clock(void) {
  id_Th_clock = osThreadNew(Th_clock, NULL, NULL);
  if (id_Th_clock == NULL)
    return(-1);
  return(0);
}
 
void Th_clock(void *argument){
	set_clock(0, 0, 0);
	tim_clock = osTimerNew(Timer_Callback, osTimerPeriodic, NULL, NULL);
	osTimerStart(tim_clock, 1000U);
  while(1)	
		osThreadYield();
}

static void Timer_Callback(void *argument){
	sec += (sec < 86399) ? 1 : -sec;
}

void set_clock (uint8_t hour, uint8_t min, uint8_t seg){
	osTimerStop(tim_clock);
	multiple_to_sec(&sec, hour, min, seg);
	osTimerStart(tim_clock, 1000U);
}

//extern uint32_t sec
//uint8_t h, m, s;
//sec_to_multiple(sec, &h, &m, &s);
//multiple_to_sec(&sec, h, m, s);

void multiple_to_sec(uint32_t* sec, uint8_t  hour, uint8_t  minutes, uint8_t  seconds){
	*sec = (hour * 3600) + (minutes * 60) + seconds;
}

void sec_to_multiple(uint32_t  sec, uint8_t* hour, uint8_t* minutes, uint8_t* seconds){
	*hour = sec / 3600;
	*minutes = (sec - (*hour * 3600)) / 60;
	*seconds = (sec - (*hour * 3600) - (*minutes * 60));
}

/*TEST*/
int Init_Th_clock_test(void){
	id_Th_clock_test = osThreadNew(Th_clock_test, NULL, NULL);
	if(id_Th_clock_test == NULL)
		return(-1);
	return(0);
}

static void Th_clock_test(void *arguments){
	uint8_t last_sec = 0;
	led_init();
	Init_Th_clock();
	set_clock(23, 59, 50);
	while(1){	
		if(last_sec != sec){
			last_sec = sec;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		}
		osThreadYield();
	}
}

static void led_init(void){
	static GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}
