#include "clock.h"
#include "stm32f4xx_hal.h"

extern uint8_t hours;
extern uint8_t minutes;
extern uint8_t seconds;

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
	tim_clock = osTimerNew(Timer_Callback, osTimerPeriodic, NULL, NULL);
	osTimerStart(tim_clock, 1000U);
  while(1)	
		osThreadYield();
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
	id_Th_clock_test = osThreadNew(Th_clock_test, NULL, NULL);
	if(id_Th_clock_test== NULL)
		return(-1);
	return(0);
}

static void Th_clock_test(void *arguments){
	uint8_t last_sec, last_min, last_hour = 0;
	led_init();
	Init_Th_clock();
	set_clock(23,59,50);
	while(1){
		if(last_sec != seconds){
			last_sec = seconds;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		}
		if(last_min != minutes){
			last_min = minutes;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		}
		if(last_hour != hours){
			last_hour = hours;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
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
	
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14), GPIO_PIN_SET);
}
