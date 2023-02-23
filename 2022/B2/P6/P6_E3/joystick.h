#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "cmsis_os2.h"

#define QUEUE_OBJECTS_JOY 16

typedef enum{Arriba, Abajo, Izquierda, Derecha, Centro}teclas_t;

typedef struct{
	teclas_t tecla;
}QUEUE_OBJ_JOY;

int Init_Th_joystick(void);
osThreadId_t id_Th_joystick(void);
osMessageQueueId_t id_Queue_joystick(void);

#endif
