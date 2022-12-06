#include "rda5807m.h"
#include "Driver_I2C.h" 
#include "stm32f4xx_hal.h"

extern ARM_DRIVER_I2C Driver_I2C2;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C2;

static osThreadId_t id_Th_rda;
static osThreadId_t id_Th_rda_test;
static osMessageQueueId_t id_MsgQueue_rda_mosi;
static osMessageQueueId_t id_MsgQueue_rda_miso;

static uint16_t rda_WR_reg02 = 0x00;
static uint16_t rda_WR_reg03 = 0x00;
static uint16_t rda_WR_reg04 = 0x00;
static uint16_t rda_WR_reg05 = 0x00;
static uint16_t rda_WR_reg06 = 0x00;
static uint16_t rda_WR_reg07 = 0x00;
//static uint16_t rda_WR_reg08 = 0x00;
static uint16_t rda_RD_reg0A = 0x00;

static uint16_t frequency;

static void Th_rda(void *argument);
static void Th_rda_test(void *argument);

static void rda_init(void);
static int rda_write(void);
static int rda_read(void);
static void set_volume(uint8_t vol);

osThreadId_t get_id_Th_rda(void){
	return id_Th_rda;
}

osThreadId_t get_id_Th_rda_test(void){
	return id_Th_rda_test;
}

osMessageQueueId_t get_id_MsgQueue_rda_miso(void){
	return id_MsgQueue_rda_mosi;
}

osMessageQueueId_t get_id_MsgQueue_rda_mosi(void){
	return id_MsgQueue_rda_miso;
}

static int Init_MsgQueue_rda_miso(void){
  id_MsgQueue_rda_miso = osMessageQueueNew(MSGQUEUE_OBJECTS_RDA, sizeof(MSGQUEUE_OBJ_RDA_MISO), NULL);
  if(id_MsgQueue_rda_miso == NULL)
    return (-1); 
  return(0);
}

static int Init_MsgQueue_rda_mosi(void){
  id_MsgQueue_rda_mosi = osMessageQueueNew(MSGQUEUE_OBJECTS_RDA, sizeof(MSGQUEUE_OBJ_RDA_MOSI), NULL);
  if(id_MsgQueue_rda_mosi == NULL)
    return (-1); 
  return(0);
}

static void callback_i2c(uint32_t event){
	osThreadFlagsSet(id_Th_rda, event);
}

static void rda_init(void){
	I2Cdrv-> Initialize   (callback_i2c);
	I2Cdrv-> PowerControl (ARM_POWER_FULL);
	I2Cdrv-> Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  I2Cdrv-> Control      (ARM_I2C_BUS_CLEAR, 0);
	
	rda_WR_reg02 = (0x0000 | RDA_CONF_HZ | RDA_CONF_nMUTE |RDA_CONF_MONO | RDA_CONF_NEW_MET);
	rda_WR_reg03 = (0x0000);
	rda_WR_reg04 = (RDA_CONF_AFC_DIS);
	rda_WR_reg05 = (RDA_CONF_INT_MOD | RDA_CONF_SNR_TRESH | RDA_CONF_LNA_DUAL | RDA_CONF_LNA_2_1mA | RDA_CONF_INIT_VOL);
	rda_WR_reg06 = (RDA_CONF_OPEN_MODE);
	rda_WR_reg07 = (0x0000);
	//rda_WR_reg08 = (0x0000);
	rda_write();
}

static int rda_write(void){
		static uint32_t flags;
    static uint8_t data_reg[30];
		data_reg[0]  = rda_WR_reg02 >> 8;
		data_reg[1]  = rda_WR_reg02 & 0x00FF;
		data_reg[2]  = rda_WR_reg03 >> 8;
		data_reg[3]  = rda_WR_reg03 & 0x00FF;
		data_reg[4]  = rda_WR_reg04 >> 8;
		data_reg[5]  = rda_WR_reg04 & 0x00FF;
		data_reg[6]  = rda_WR_reg05 >> 8;
		data_reg[7]  = rda_WR_reg05 & 0x00FF;
		data_reg[8]  = rda_WR_reg06 >> 8;
		data_reg[9]  = rda_WR_reg06 & 0x00FF;
		data_reg[10] = rda_WR_reg07 >> 8;
		data_reg[11] = rda_WR_reg07 & 0x00FF;
		I2Cdrv->MasterTransmit(RDA_ADDR_WR, data_reg, 30, false);
		flags = osThreadFlagsWait(0xFFFF, osFlagsWaitAny, osWaitForever);
		if((flags & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U && (flags & ARM_I2C_EVENT_TRANSFER_DONE) != true)return -1;
		return 0;
}

static int rda_read(void){
	static uint32_t flags;
	uint8_t data_reg[12];
	
	I2Cdrv->MasterReceive(RDA_ADDR_RD, data_reg, 12, false);
	flags = osThreadFlagsWait(0xFFFF, osFlagsWaitAny, osWaitForever);
	if((flags & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U && (flags & ARM_I2C_EVENT_TRANSFER_DONE) != true)return -1;
	rda_RD_reg0A = ((uint16_t)data_reg[0]  << 8) & data_reg[1];
//	rda_RD_reg0B = ((uint16_t)data_reg[2]  << 8) & data_reg[3];
//	rda_RD_reg0C = ((uint16_t)data_reg[4]  << 8) & data_reg[5];
//	rda_RD_reg0D = ((uint16_t)data_reg[6]  << 8) & data_reg[7];
//	rda_RD_reg0E = ((uint16_t)data_reg[8]  << 8) & data_reg[9];
//	rda_RD_reg0F = ((uint16_t)data_reg[10] << 8) & data_reg[11];
	
	frequency = 870 + (rda_RD_reg0A & 0x03FF);
	return 0;
}

static void power_on(void){
	rda_WR_reg02 = rda_WR_reg02 | RDA_PWR_ON;
	rda_WR_reg03 = rda_WR_reg03 | RDA_TUNE;
	rda_write();
	rda_WR_reg03 = rda_WR_reg03 & ~RDA_TUNE;
}

static void power_off(void){
	rda_WR_reg02 = rda_WR_reg02 ^ RDA_PWR_ON;
	rda_write();
}

static void set_volume(uint8_t vol){
	vol = (vol > 15) ? 15 : vol;
	rda_WR_reg05 = (rda_WR_reg05 & 0xFFF0) | vol;
	rda_write();
}

static void next_100kHz(void){
	rda_read();
	uint16_t channel = rda_RD_reg0A & 0x03FF;
	channel += (channel == 210) ? 0 : 1;
	rda_WR_reg03 = channel << 6;
	rda_write();
}

static void prev_100kHz(void){
	rda_read();
	uint16_t channel = rda_RD_reg0A & 0x03FF;
	channel--;
	rda_WR_reg03 = channel << 6;
	rda_write();
}

static void seek_up(void){
	rda_WR_reg02 = rda_WR_reg02 | RDA_SEEK_UP;
	rda_write();
	rda_WR_reg02 = rda_WR_reg02 & ~RDA_SEEK_UP;
}

static void seek_down(void){
	rda_WR_reg02 = rda_WR_reg02 | RDA_SEEK_DOWN;
	rda_write();
	rda_WR_reg02 = rda_WR_reg02 & ~RDA_SEEK_DOWN;
}

int Init_Th_rda(void){
  id_Th_rda = osThreadNew(Th_rda, NULL, NULL);
  if(id_Th_rda == NULL)
    return(-1);
  return(Init_MsgQueue_rda_miso() | Init_MsgQueue_rda_mosi());
}

static void Th_rda(void *argument){
	static MSGQUEUE_OBJ_RDA_MISO msg_miso;
	static MSGQUEUE_OBJ_RDA_MOSI msg_mosi;
	rda_init();
	while(1){
		if(osOK == osMessageQueueGet(id_MsgQueue_rda_mosi, &msg_mosi, NULL, 0U)){
			switch(msg_mosi.comando){
				case cmd_power_on:
					power_on();
				break;
				
				case cmd_power_off:
					power_off();
				break;
				
				case cmd_seek_up:
					seek_up();
				break;
				
				case cmd_seek_down:
					seek_down();
				break;
				
				case cmd_next100kHz:
					next_100kHz();
				break;
				
				case cmd_prev100kHz:
					prev_100kHz();
				break;
				
				case cmd_set_vol:
					set_volume(msg_mosi.vol);
				break;
				
				case cmd_set_freq:
					
				break;
				
				case cmd_get_freq:
					rda_read();
					msg_miso.frequency = frequency;
					osMessageQueuePut(id_MsgQueue_rda_miso, &msg_miso, NULL, 0U);
				break;
			}
		}
		osThreadYield();
	}
}

/*TEST*/
int Init_Th_rda_test(void){
  id_Th_rda_test = osThreadNew(Th_rda_test, NULL, NULL);
  if(id_Th_rda_test == NULL)
    return(-1);
  return(0);
}

static void Th_rda_test(void *argument){
	static MSGQUEUE_OBJ_RDA_MOSI msg;
	static MSGQUEUE_OBJ_RDA_MISO msg_miso;
	int i;
	static uint16_t freq;
	Init_Th_rda();
	msg.comando = cmd_power_on;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg, NULL, 0U);
	msg.comando = cmd_seek_up;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg, NULL, 0U);
	osDelay(5000);
	msg.comando = cmd_seek_up;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg, NULL, 0U);
	osDelay(5000);
	msg.comando = cmd_seek_up;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg, NULL, 0U);
	osDelay(5000);
	msg.comando = cmd_seek_up;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg, NULL, 0U);
	osDelay(5000);
	msg.comando = cmd_seek_up;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg, NULL, 0U);
	osDelay(5000);
	
	
	msg.comando = cmd_get_freq;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg, NULL, 0U);
	osMessageQueueGet(id_MsgQueue_rda_miso, &msg_miso, NULL, 0U);
	
	freq = msg_miso.frequency;
	while(1){
		
	}
}
