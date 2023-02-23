/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MP3_H
#define __MP3_H

/* Exported constants --------------------------------------------------------*/
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "Driver_USART.h"
#include "cmsis_os2.h"                   /* ARM::CMSIS:RTOS:Keil RTX */
#include <stdio.h>
#include <string.h>

/* Exported functions ------------------------------------------------------- */
/*Funciones*/
void myUSART6_callback(uint32_t event);
int Init_myUSART6_Thread (void) ;//Init thread del pc
void myUSART6_Thread(void *argument);//Thread del pc
void Init_pin_mp3(void);
void Thread_mvol (void *argument);
int Init_Thread_mvol (void);
/*Comandos*/
void carpeta1(void);
void carpeta2(void);
void carpeta3(void);
void NextSong(void);
void PreviousSong(void);
void PlayFirstSong(void);
void PlaySecondSong(void);
void VolumeUp(void);
void VolumeDownt(void);
void SetVolumen(void);
void SelectDevice(void);
void SleepMode(void);
void WakeUp(void);
void Reset(void);
void Play(void);
void Pause(void);
void StopPlay(void);
void Volume(void);


#endif /* __MP3_H */

