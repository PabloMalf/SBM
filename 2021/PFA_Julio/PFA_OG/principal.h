/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PRINCIPAL_H
#define __PRINCIPAL_H

/* Exported constants --------------------------------------------------------*/
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects

/* Includes ------------------------------------------------------------------*/
#include "clock.h"  
#include "lcd.h"
#include "temp.h"
#include "joystick.h"
#include "vol.h"
#include "pwm.h"
#include "led.h"
#include "mp3.h"
#include "com.h"


/* Exported functions ------------------------------------------------------- */

	//LCD
	int Init_MsgQueue_lcd(void);
	void pintar_hora(void);
	void pintar_temperatura(void);
	void pintar_Trepr(void);
	void pintar_mp3(void);
	
	//JOYSTICK
	int Init_MsgQueue_joy (void);
	
	//LED
	int Init_MsgQueue_led (void) ;
	
	//MP3
	int Init_MsgQueue_mp3 (void) ;
	int Init_MsgQueue_mvol(void) ;
	
	//COM
	int Init_MsgQueue_pc(void);
		
	//PRINCIPAL
	static void Timer_repr_Callback (void const *arg);
	int Init_Timer_repr (void);
	static void Timer_vol_Callback (void const *arg);
	int Init_Timer_vol (void);
	void Thread_pvol (void *argument) ;
	int Init_Thread_pvol (void);
	int Init_MsgQueue_vol(void);
	int Init_MsgQueue_principal (void);
	int Init_Thread_principal (void);

	
	
#endif /* __PRINCIPAL_H */
