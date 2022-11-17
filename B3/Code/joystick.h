#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_JOY 4

typedef enum {Arriba, Abajo, Izquierda, Derecha, Centro} teclas_t;

typedef enum {Corta, Larga} duraciones_t;

typedef struct {
  teclas_t tecla;
	duraciones_t duracion;
} MSGQUEUE_OBJ_JOY;

int Init_Th_joystick(void);
osThreadId_t id_Th_joystick(void);
osMessageQueueId_t id_MsgQueue_joystick(void);

#endif
