#include "rda5807m.h"
#include "Driver_I2C.h" 
#include "stm32f4xx_hal.h"

typedef struct{
	uint16_t reg2_WR;
	uint16_t reg3_WR;
	uint16_t reg4_WR;
	uint16_t reg5_WR;
	uint16_t reg6_WR;
	uint16_t reg7_WR;
	uint16_t reg8_WR;

	uint16_t regA_RD;
	uint16_t regB_RD;
	uint16_t regC_RD;
	uint16_t regD_RD;
	uint16_t regE_RD;
	uint16_t regF_RD;
}reg_rda_t;

typedef struct{
	uint8_t	 volume;
	uint8_t  freq_rssi;
	uint16_t frequency;
}data_t;

extern ARM_DRIVER_I2C Driver_I2C2;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C2;

static osThreadId_t id_Th_rda;
static osThreadId_t id_Th_rda_test;
static osMessageQueueId_t id_MsgQueue_rda_mosi;
static osMessageQueueId_t id_MsgQueue_rda_miso;

static void rda_init		(reg_rda_t*, data_t*);
static int rda_write		(reg_rda_t*);
static int rda_read			(reg_rda_t*, data_t*);
static void power_on		(reg_rda_t*);
static void power_off		(reg_rda_t*);
static void set_volume	(reg_rda_t*, data_t*);
static void next_100kHz	(reg_rda_t*, data_t*);
static void prev_100kHz	(reg_rda_t*, data_t*);
static void seek_up			(reg_rda_t*);
static void seek_down		(reg_rda_t*);
static void set_freq		(reg_rda_t*, uint16_t);

static void Th_rda(void *argument);
static void Th_rda_test(void *argument);

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

static void callback_i2c(uint32_t event){
	osThreadFlagsSet(id_Th_rda, event);
}

static void rda_init(reg_rda_t* reg, data_t* data){
	I2Cdrv-> Initialize   (callback_i2c);
	I2Cdrv-> PowerControl (ARM_POWER_FULL);
	I2Cdrv-> Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  I2Cdrv-> Control      (ARM_I2C_BUS_CLEAR, 0);
	
	reg->reg2_WR = (0x0000 | RDA_CONF_HZ | RDA_CONF_nMUTE | RDA_CONF_MONO | RDA_CONF_NEW_MET);
	reg->reg3_WR = (0x0000);
	reg->reg4_WR = (RDA_CONF_AFC_DIS);
	reg->reg5_WR = (RDA_CONF_INT_MOD | RDA_CONF_SNR_TRESH | RDA_CONF_LNA_DUAL | RDA_CONF_LNA_2_1mA | RDA_CONF_INIT_VOL);
	reg->reg6_WR = (RDA_CONF_OPEN_MODE);
	reg->reg7_WR = (0x0000);
	//reg.reg08_WR = (0x0000);
	rda_write(reg);
	
	data->volume = RDA_CONF_INIT_VOL;
}

static int rda_write(reg_rda_t* reg){
		static uint32_t flags;
    static uint8_t data_reg[30];
		data_reg[0]  = reg->reg2_WR >> 8;
		data_reg[1]  = reg->reg2_WR & 0x00FF;
		data_reg[2]  = reg->reg3_WR >> 8;
		data_reg[3]  = reg->reg3_WR & 0x00FF;
		data_reg[4]  = reg->reg4_WR >> 8;
		data_reg[5]  = reg->reg4_WR & 0x00FF;
		data_reg[6]  = reg->reg5_WR >> 8;
		data_reg[7]  = reg->reg5_WR & 0x00FF;
		data_reg[8]  = reg->reg6_WR >> 8;
		data_reg[9]  = reg->reg6_WR & 0x00FF;
		data_reg[10] = reg->reg7_WR >> 8;
		data_reg[11] = reg->reg7_WR & 0x00FF;
		I2Cdrv->MasterTransmit(RDA_ADDR_WR, data_reg, 30, false);
		flags = osThreadFlagsWait(0xFFFF, osFlagsWaitAny, osWaitForever);
		if((flags & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U && (flags & ARM_I2C_EVENT_TRANSFER_DONE) != true)return -1;
		return 0;
}

static int rda_read(reg_rda_t* reg, data_t* data){
	static uint8_t data_reg[12];
	static uint32_t flags;
	static uint32_t a;
	static double freq;
	
	I2Cdrv->MasterReceive(RDA_ADDR_RD, data_reg, 12, false);
	flags = osThreadFlagsWait(0xFFFF, osFlagsWaitAny, osWaitForever);
	if((flags & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U && (flags & ARM_I2C_EVENT_TRANSFER_DONE) != true)return -1;

	reg->regA_RD = (((reg->regA_RD & data_reg[0])  << 8) | data_reg[1]);
	reg->regB_RD = (((reg->regB_RD & data_reg[2])  << 8) | data_reg[3]);
	reg->regC_RD = (((reg->regC_RD & data_reg[4])  << 8) | data_reg[5]);
	reg->regD_RD = (((reg->regD_RD & data_reg[6])  << 8) | data_reg[7]);
	reg->regE_RD = (((reg->regE_RD & data_reg[8])  << 8) | data_reg[9]);
	reg->regF_RD = (((reg->regF_RD & data_reg[10]) << 8) | data_reg[11]);
	
	
	data->freq_rssi = reg->regB_RD >> 10;
	data->frequency = ((reg->regA_RD & 0x03FF) + 870);
	
	freq = (((reg->regA_RD & 0x03FF) * 100) + 87000);
	data->freq_rssi = reg->regB_RD >> 9;//NO FUFA
	return 0;
}

static void power_on(reg_rda_t* reg){
	reg->reg2_WR = reg->reg2_WR | RDA_PWR_ON;
	reg->reg3_WR = reg->reg3_WR | RDA_TUNE;
	rda_write(reg);
	reg->reg3_WR = reg->reg3_WR & ~RDA_TUNE;
}

static void power_off(reg_rda_t* reg){
	reg->reg2_WR = reg->reg2_WR ^ RDA_PWR_ON;
	rda_write(reg);
}

static void set_volume(reg_rda_t* reg, data_t* data){
	data->volume = (data->volume > 15) ? 15 : data->volume;
	reg->reg5_WR= (reg->reg5_WR & 0xFFF0) | data->volume;
	rda_write(reg);
}

static void next_100kHz(reg_rda_t* reg, data_t* data){
	rda_read(reg, data);
	uint16_t channel = reg->regA_RD & 0x03FF;
	channel += (channel == 210) ? 0 : 1;
	reg->reg3_WR = channel << 6;
	rda_write(reg);
}

static void prev_100kHz(reg_rda_t* reg, data_t* data){
	rda_read(reg, data);
	uint16_t channel = reg->regA_RD & 0x03FF;
	channel--;
	reg->reg3_WR = channel << 6;
	rda_write(reg);
}

static void seek_up(reg_rda_t* reg){
	reg->reg2_WR = reg->reg2_WR | RDA_SEEK_UP;
	rda_write(reg);
	reg->reg2_WR = reg->reg2_WR & ~RDA_SEEK_UP;
}

static void seek_down(reg_rda_t* reg){
	reg->reg2_WR = reg->reg2_WR | RDA_SEEK_DOWN;
	rda_write(reg);
	reg->reg2_WR = reg->reg2_WR & ~RDA_SEEK_DOWN;
}

static void set_freq(reg_rda_t* reg, uint16_t freq){
	reg->reg3_WR = reg->reg3_WR | RDA_TUNE | ((freq - 870) << 6);
	rda_write(reg);
	reg->reg3_WR = reg->reg3_WR & ~RDA_TUNE;
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
	
	static data_t data;
	static reg_rda_t reg;
	rda_init(&reg, &data);
	
	while(1){
		if(osOK == osMessageQueueGet(id_MsgQueue_rda_mosi, &msg_mosi, NULL, 0U)){
			osMessageQueueReset(id_MsgQueue_rda_miso);
			switch(msg_mosi.comando){
				case cmd_power_on:
					power_on(&reg);
				break;
				
				case cmd_power_off:
					power_off(&reg);
				break;
				
				case cmd_seek_up:
					seek_up(&reg);
				break;
				
				case cmd_seek_down:
					seek_down(&reg);
				break;
				
				case cmd_next100kHz:
					next_100kHz(&reg, &data);
				break;
				
				case cmd_prev100kHz:
					prev_100kHz(&reg, &data);
				break;
				
				case cmd_set_vol:
					data.volume = msg_mosi.data;
					set_volume(&reg, &data);
				break;
				
				case cmd_set_freq:
					set_freq(&reg, msg_mosi.data);
				break;
				
				case cmd_get_info:
					
				break;
			}
			rda_read(&reg, &data);
			msg_miso.volume    = data.volume;
			msg_miso.frequency = data.frequency;
			msg_miso.freq_rssi = data.freq_rssi;
			osMessageQueuePut(id_MsgQueue_rda_miso, &msg_miso, NULL, 0U);
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
	static MSGQUEUE_OBJ_RDA_MOSI msg_mosi;
	static MSGQUEUE_OBJ_RDA_MISO msg_miso;
	
	static uint16_t freq;
	static int i;
	
	Init_Th_rda();
	
	msg_mosi.comando = cmd_set_vol;
	msg_mosi.data = 0;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg_mosi, NULL, 0U);
	osDelay(500);
	
	msg_mosi.comando = cmd_power_on;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg_mosi, NULL, 0U);
	osDelay(500);
	
	msg_mosi.comando = cmd_set_freq;
	msg_mosi.data = 980;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg_mosi, NULL, 0U);
	osDelay(5000);
	
	for(i = 0; i < 15; i++){
		msg_mosi.comando = cmd_set_vol;
		msg_mosi.data = i;
		osMessageQueuePut(id_MsgQueue_rda_mosi, &msg_mosi, NULL, 0U);
		osDelay(500);
	}
	
	msg_mosi.comando = cmd_get_info;
	osMessageQueuePut(id_MsgQueue_rda_mosi, &msg_mosi, NULL, 0U);
	osMessageQueueGet(id_MsgQueue_rda_miso, &msg_miso, NULL, osWaitForever);
	freq = msg_miso.frequency;
	
	while(1){}
}
