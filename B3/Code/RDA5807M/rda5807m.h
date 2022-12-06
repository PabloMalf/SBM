#ifndef __RDA_H
#define __RDA_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_RDA 4

//Addres
#define RDA_ADDR_WR 0x20 >> 1
#define RDA_ADDR_RD 0x22 >> 1
//Reg2
#define RDA_CONF_HZ 		0x8000
#define RDA_CONF_nMUTE		0x6000
#define RDA_CONF_MONO		0x4000
#define RDA_CONF_BASS		0x2000
#define RDA_SEEK_UP			0x0300
#define RDA_SEEK_DOWN		0x0100
#define RDA_CONF_NEW_MET	0x0004
#define RDA_PWR_ON			0x0001
//Reg3
#define RDA_TUNE			0x0010
//Reg4
#define RDA_CONF_AFC_DIS	0x0100
//Reg5
#define RDA_CONF_INT_MOD	0x8000
#define RDA_CONF_SNR_TRESH	0x0400
#define RDA_CONF_LNA_DUAL	0x00C0
#define RDA_CONF_LNA_3_0mA	0x0030
#define RDA_CONF_LNA_2_8mA	0x0020
#define RDA_CONF_LNA_2_1mA	0x0010
#define RDA_CONF_LNA_1_8mA	0x0000
#define RDA_CONF_INIT_VOL	2
//Reg6
#define RDA_CONF_OPEN_MODE	0x4000

typedef enum {
	cmd_power_on,
	cmd_power_off,
	cmd_seek_up, 
	cmd_seek_down, 
	cmd_next100kHz, 
	cmd_prev100kHz, 
	cmd_set_vol,
	cmd_set_freq,
	cmd_get_freq} comando_t;

typedef struct{
	comando_t comando;
	uint8_t vol;
} MSGQUEUE_OBJ_RDA_MOSI;

typedef struct{
	uint16_t frequency;
} MSGQUEUE_OBJ_RDA_MISO;

int Init_Th_rda(void);
int Init_Th_rda_test(void);

osThreadId_t get_id_Th_rda(void);
osMessageQueueId_t get_id_MsgQueue_rda_mosi(void);
osMessageQueueId_t get_id_MsgQueue_rda_miso(void);

#endif
