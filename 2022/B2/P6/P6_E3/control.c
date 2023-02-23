#include "control.h"
#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "lcd.h"
#include "joystick.h"

static osThreadId_t tid_control;
static QUEUE_OBJ_JOY msg_joy;

static char data[32];

void Th_control(void *argument);

int Init_Th_control(void){
  tid_control = osThreadNew(Th_control, NULL, NULL);
  if (tid_control == NULL) {
    return(-1);
  }
  return(0);
}

void Th_control(void *argument){
	LCD_init();
	osStatus_t state;
	while(1){
		state = osMessageQueueGet	(id_Queue_joystick(), &msg_joy, NULL, 0U);
		if(state == osOK){
			switch(msg_joy.tecla){
				case Arriba:
						sprintf(data,"Arriba");
						update_data(data, data);
				break;
				
				case Abajo:
						sprintf(data,"Abajo");
						update_data(data, data);
				break;
				
				case Derecha:
						sprintf(data,"Derecha");
						update_data(data, data);
				break;
				
				case Izquierda:
						sprintf(data,"Izquierda");
						update_data(data, data);
				break;
				
				case Centro:
						sprintf(data,"Centro");
						update_data(data, data);
				break;
			}
		}
	}
}
