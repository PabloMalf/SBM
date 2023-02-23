#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

#define MSGQUEUE_OBJECTS_led 16

typedef struct {
  uint8_t R;
	uint8_t G;
	uint8_t B;
} MSGQUEUE_OBJ_led_t;

extern osThreadId_t tid_Th_led;                        // thread id
extern osMessageQueueId_t mid_MsgQueue_led;

int Init_Th_led (void);
void Th_led (void *argument);
