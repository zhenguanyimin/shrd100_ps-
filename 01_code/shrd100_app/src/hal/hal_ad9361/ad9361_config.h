#ifndef _AD9361_CONFIG_H_
#define _AD9361_CONFIG_H_
#include <sys/_stdint.h>

//#define PROJECT_VER "HJX MINI-TRX-Z-MGC-Rx-Gain Index-76"   //"HJX MINI-TRX-Z-TEST"
#define PROJECT_VER "HJX MINI-TRX-Z-2020 SDR"


#define AD9009_VERSION_X "2"
#define AD9009_VERSION_Y "0"
#define AD9009_VERSION_Z "0" 


typedef enum Freq_CFG
{
	Freq_RX_842m,
	Freq_RX_915m,

	Freq_RX_1080m,
	Freq_RX_1120m,
	Freq_RX_1160m,
	Freq_RX_1200m,
	Freq_RX_1240m,
	Freq_RX_1280m,
	Freq_RX_1320m,
	Freq_RX_1360m,

	Freq_RX_1437m,
//	Freq_RX_2420m,
	Freq_RX_2422m,
	Freq_RX_2437m,
//	Freq_RX_2460m,
	Freq_RX_2462m,
//	Freq_RX_4870m,

//	Freq_RX_4915m,

//	Freq_RX_4960m,
//	Freq_RX_5000m,
//	Freq_RX_5040m,
//	Freq_RX_5080m,
//	Freq_RX_5120m,
	Freq_RX_5160m,
	Freq_RX_5180m,
	Freq_RX_5200m,
	Freq_RX_5220m,
	Freq_RX_5240m,
//	Freq_RX_5280m,
//	Freq_RX_5320m,
//	Freq_RX_5360m,
//	Freq_RX_5400m,
//	Freq_RX_5445m,

//	Freq_RX_5490m,

//	Freq_RX_5535m,
//	Freq_RX_5580m,
//	Freq_RX_5625m,

	Freq_RX_5655m,

//	Freq_RX_5665m,

//	Freq_RX_5705m,

	Freq_RX_5695m,
	Freq_RX_5725m,

	Freq_RX_5745m,
	Freq_RX_5765m,
	Freq_RX_5785m,
	Freq_RX_5805m,
	Freq_RX_5825m,
//	Freq_RX_5830m,


	Freq_RX_5850m,
	Freq_RX_5890m,
	Freq_RX_5930m,

//	Freq_RX_5860m,


//	Freq_RX_5900m,
//	Freq_RX_5940m,
//	Freq_RX_5980m,
//
//	Freq_RX_842m,
//	Freq_RX_915m,
//	Freq_RX_1437m,
//	Freq_RX_1775m,
//	Freq_RX_2420m,
//	Freq_RX_2460m,
//	Freq_RX_5160m,
//	Freq_RX_5200m,
//	Freq_RX_5240m,
//	Freq_RX_5745m,
//	Freq_RX_5765m,
//	Freq_RX_5785m,
//	Freq_RX_5805m,
//	Freq_RX_5825m,
	Freq_MAX_CNT,
};

enum Freq_TX_CFG
{
	Freq_TX_915m= 0x01,
	Freq_TX_GPS,
	Freq_TX_GPS_915m,
};

typedef struct
{
	uint8_t enable;
	uint8_t type;//0:飞控信号   1:gps信号
}RfTxProperty_t;

float SetRxFreq(float uLocalOscFreq);
uint32_t get_rx_freq(uint32_t freqItem);
uint32_t set_rx_freq(uint32_t freq);
void cfgAd9361(void);
int32_t InitAd9361(void);
int32_t InitAd9361_Rx(void);
int32_t InitAd9361_Rx_droneid(void);
void ad9361_cfg_tx_init();
int32_t Ad9361_fre_hop_init(void);
int32_t Ad9361_fre_hop_set(int32_t rfRang);
int32_t Ad9361_fre_hop_start(void);
int32_t Ad9361_fre_hop_stop(void);
void SetAD9361Gpio(uint32_t value);
void SetAd9361Gain(uint8_t omnidirect_value, uint8_t orientation_value);
void SetAd9361GainCutoverFlag(uint8_t value);
uint8_t GetAd9361GainCutoverFlag(void);
void SetAd9361GainCutoverValue(uint8_t value1, uint8_t value2);
void SetAd9361GainAGC(uint8_t value);
void SetAd9361Reg(uint8_t addr, uint8_t addr2, uint8_t value);
void SetAd9361TxCfgFreq(uint8_t mode, uint8_t Mask, uint8_t *LowFreqHit);
void SetAd9361TxCfgFreqMode(uint8_t value);
#endif

