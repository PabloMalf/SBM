#include "rda5807m.h"
#include "Driver_I2C.h" 
#include "stm32f4xx_hal.h"
#include "string.h"

extern ARM_DRIVER_I2C Driver_I2C2;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C2;

static osThreadId_t id_Th_rda;
static osThreadId_t id_Th_rda_test;

static osMessageQueueId_t id_MsgQueue_rda_mosi;
static osMessageQueueId_t id_MsgQueue_rda_miso;

static uint16_t wr_registrers[6];
static uint16_t rd_regA;
static uint16_t rd_regB;
static uint16_t rd_regC;
static uint16_t rd_regD;
static uint16_t rd_regE;
static uint16_t rd_regF;

static int channel;
static float frequency;

static MSGQUEUE_OBJ_RDA_MOSI msg_mosi;
static MSGQUEUE_OBJ_RDA_MISO msg_miso;

void Th_rda(void *argument);
void Th_rda_test(void *argument);

static void rda_init(void);
static int rda_write(void);
static int rda_read(void);
static void set_volume(uint8_t vol);
static void set_freq(float);

osMessageQueueId_t get_id_MsgQueue_rda_miso(void){
	return id_MsgQueue_rda_miso;
}

osMessageQueueId_t get_id_MsgQueue_rda_mosi(void){
	return id_MsgQueue_rda_mosi;
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

void callback_i2c(uint32_t event){
	osThreadFlagsSet(id_Th_rda, event);
}

static void rda_init(void){
	I2Cdrv-> Initialize   (callback_i2c);
	I2Cdrv-> PowerControl (ARM_POWER_FULL);
	I2Cdrv-> Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  I2Cdrv-> Control      (ARM_I2C_BUS_CLEAR, 0);
	
	wr_registrers[0] = (0x0000 | RDA_CONF_HZ | RDA_CONF_nMUTE |RDA_CONF_MONO | RDA_CONF_NEW_MET);
	wr_registrers[1] = (0x0000);
	wr_registrers[2] = (RDA_CONF_AFC_DIS);
	wr_registrers[3] = (RDA_CONF_INT_MOD | RDA_CONF_SNR_TRESH | RDA_CONF_LNA_DUAL | RDA_CONF_LNA_2_1mA | RDA_CONF_INIT_VOL);
	wr_registrers[4] = (RDA_CONF_OPEN_MODE);
	wr_registrers[5] = (0x0000);
	rda_write();
}

static int rda_write(){
    static uint32_t flags;
    static uint8_t buffer[12];
    buffer[0]  = wr_registrers[0] >> 8;
		buffer[1]  = wr_registrers[0] & 0x00FF;
    buffer[2]  = wr_registrers[1] >> 8;
		buffer[3]  = wr_registrers[1] & 0x00FF;
    buffer[4]  = wr_registrers[2] >> 8;
		buffer[5]  = wr_registrers[2] & 0x00FF;
    buffer[6]  = wr_registrers[3] >> 8;
		buffer[7]  = wr_registrers[3] & 0x00FF;
    buffer[8]  = wr_registrers[4] >> 8;
		buffer[9]  = wr_registrers[4] & 0x00FF;
    buffer[10] = wr_registrers[5] >> 8;
		buffer[11] = wr_registrers[5] & 0x00FF;
	
    memcpy(msg_miso.reg, wr_registrers, 6);
    I2Cdrv->MasterTransmit(RDA_ADDR_WR, buffer, 12, false);
		osMessageQueuePut(id_MsgQueue_rda_miso, &msg_miso, NULL, 0U);
	
    flags = osThreadFlagsWait(0xFFFF, osFlagsWaitAny, osWaitForever);
    if((flags & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U && (flags & ARM_I2C_EVENT_TRANSFER_DONE) != true)
      return -1;
    return 0;
}

static int rda_read() {
	static uint32_t flags;
  static uint8_t data[12];
	
	I2Cdrv->MasterReceive(RDA_ADDR_RD, data, 12, false);
	flags = osThreadFlagsWait(0xFFFF, osFlagsWaitAny, osWaitForever);
	if((flags & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U && (flags & ARM_I2C_EVENT_TRANSFER_DONE) != true) {
		return -1;
	} else {
		rd_regA = ((rd_regA & data[0] << 8) | data[1]);
		rd_regB = ((rd_regB & data[2] << 8) | data[3]);
		rd_regC = ((rd_regC & data[4] << 8) | data[5]);
		rd_regD = ((rd_regD & data[6] << 8) | data[7]);
		rd_regE = ((rd_regE & data[8] << 8) | data[9]);
		rd_regF = ((rd_regF & data[10] << 8) | data[11]);
    return 0;
	}
}

static void get_info(){
	rda_read();
	msg_miso.freq = (((rd_regA & 0x03FF) * 100) + 87000);
	frequency = msg_miso.freq / 1000;
	osMessageQueueReset(id_MsgQueue_rda_miso);
  osMessageQueuePut(id_MsgQueue_rda_miso, &msg_miso, NULL, 0U);
}

static void power_on(void){
		wr_registrers[1] = wr_registrers[1] | RDA_TUNE_ON;
		wr_registrers[0] = wr_registrers[0] | RDA_PWR_ON;
		rda_write();
		wr_registrers[1] = wr_registrers[1] & 0xFFEF;
}

static void power_off(void){
	wr_registrers[0] = wr_registrers[0] ^ RDA_PWR_ON;
	rda_write();
}

static void seek_up(void){
	wr_registrers[0] = wr_registrers[0] | RDA_SEEK_UP;
	rda_write();
	wr_registrers[0] = wr_registrers[0] & ~RDA_SEEK_UP;
}

static void seek_down(void){
	wr_registrers[0] = wr_registrers[0] | RDA_SEEK_DOWN;
	rda_write();
	wr_registrers[0] = wr_registrers[0] & ~RDA_SEEK_DOWN;
}

static void next_100kHz(void){
	frequency = frequency + 0.1;
	set_freq(frequency);
}

static void prev_100kHz(void){
	frequency = frequency - 0.1;
	set_freq(frequency);
}

static void set_freq(float freq_aux){
  channel = ((freq_aux-START_FREQ)/0.1)+0.05;
  channel = channel & 0x03FF;
  wr_registrers[1] = channel * 64 + 0x10;
  rda_write();
  wr_registrers[1] = wr_registrers[1] & ~RDA_TUNE_ON;
	osDelay(500);
	get_info();
}

static void set_volume(uint8_t vol){
		if(vol > 15){
			vol = 15;
		}
		if(vol < 1){
			vol = 0;
		}
		wr_registrers[3] = (wr_registrers[3] & 0xFFF0) | vol;
		rda_write();
}

int Init_Th_rda(void){
  id_Th_rda = osThreadNew(Th_rda, NULL, NULL);
  if(id_Th_rda == NULL)
    return(-1);
  return(Init_MsgQueue_rda_miso() | Init_MsgQueue_rda_mosi());
}

void Th_rda(void *argument){
	rda_init();
	while(1){
		if(osOK == osMessageQueueGet(id_MsgQueue_rda_mosi, &msg_mosi, NULL, 0U)){
			switch(msg_mosi.comando){
				case cmd_power_on:				
					power_on();
					set_freq(98.0);
				break;
				
				case cmd_power_off:
					power_off();
				break;
				
				case cmd_seek_up:
					seek_up();
					osDelay(500);
					get_info();
				break;
				
				case cmd_seek_down:
					seek_down();
					osDelay(500);
					get_info();
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
				
				case cmd_get_info:
					get_info();
				break;
								
				case cmd_set_freq:					
					set_freq(msg_mosi.freq);
				break;
			}
		}
		osThreadYield();
	}
}

int Init_Th_rda_test(void){
  id_Th_rda_test = osThreadNew(Th_rda, NULL, NULL);
  if(id_Th_rda_test == NULL)
    return(-1);
  return(0);
}

void Th_rda_test(void *argument){
	Init_Th_rda();
	while(1){}
}
