#ifndef __TSL_H
#define __TSL_H
#include "cmsis_os2.h"
#include "Driver_I2C.h"

#define INIT_LIGHT_MEASURE 0x10000000
#define MSGQUEUE_OBJECTS_I2C 16

#define MAX_VALUE_VIS 38000 //37889
#define MIN_VALUE_VIS 75

typedef struct{
	uint8_t light_cen;
	uint8_t light_dec;
	uint8_t light_uni;
}MSGQUEUE_OBJ_I2C;

int Init_Th_I2C (void);

#endif
