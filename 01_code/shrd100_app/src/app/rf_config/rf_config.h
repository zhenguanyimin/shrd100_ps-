/*
 * rf_config.h
 *
 *  Created on: 2022-09-20
 *      Author: A19199
 */
#ifndef RF_CONFIG_H
#define RF_CONFIG_H
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../hal/hal.h"
#include "../../srv/flash_nv/flash_nv.h"

enum IRD_RATE
{
	IRD_RATE_5_12us,
	IRD_RATE_2_56us,
	IRD_RATE_10_24us,
};

enum RF_EVENT
{
	eRF_AD9361_CFG_F,
	eRF_PL_START_COLLECT,
	eRF_PL_STOP_COLLECT,
	eRF_AD9361_CFG_HIT_915,
	eRF_AD9361_CFG_HIT_GPS,
	eRF_AD9361_CFG_HIT_GPS_915,
	eRF_AD9361_CFG_CLOSE_HIT,
};

enum PL_REG_ADDR
{
	eCFG_PWR_GATE_L = XPAR_M08_AXI_BASEADDR + 0x00,
	eCFG_PWR_GATE_H = XPAR_M08_AXI_BASEADDR + 0x04,
	eCFG_RF_MOD 	= XPAR_M08_AXI_BASEADDR + 0x08,
	eCFG_PINC_CH0 	= XPAR_M08_AXI_BASEADDR + 0x0c,
	eCFG_PINC_CH1 	= XPAR_M08_AXI_BASEADDR + 0x10,
	eIDX_FREQOFFSET = XPAR_M08_AXI_BASEADDR + 0x20,
	eRF_MOD 		= XPAR_M08_AXI_BASEADDR + 0x24,
	eWR_DDR_ENABLE 	= XPAR_M09_AXI_BASEADDR + 0x14,
};


typedef struct freq_cfg
{
    uint64_t uFreq;      //hz
    int32_t uFreqOffsetL; //Frequency left Offset
    int32_t uFreqOffsetR; //Frequency right Offset
} freq_cfg_t;

#define DAC_BUF_SIZE 512+2
extern uint32_t dac_cfg_B_2_4G[DAC_BUF_SIZE];
extern uint32_t dac_cfg_B_5_8G[DAC_BUF_SIZE];
extern uint32_t dac_cfg_A_5_2G[DAC_BUF_SIZE];
extern uint32_t dac_cfg_A_5_8G[DAC_BUF_SIZE];
void RF_TEST(uint8_t channel);
void Rf_ConfigTask_Post(uint32_t flag);

void Rf_Ad9361_cfg_f(void);

int32_t RF_ConfigParamByFreqId(int32_t CurFreqIndex );
void RfConfig_Init();
void RfConfig_droneid_Init();
int32_t InitRf_Config_Task();
void SetDetectFreqItem(uint8_t value);
uint8_t GetDetectFreqItem(void);
int32_t DacRegCfg_init();
int32_t DacVotlCfg_init(FlashNV_ID id);
void SetSweepFlag(uint8_t value);
void GetFreqRegValue(void);

#endif /* RF_CONFIG_H */
