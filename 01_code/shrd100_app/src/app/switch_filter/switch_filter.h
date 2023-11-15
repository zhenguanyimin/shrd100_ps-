#include <stdint.h>

enum RF5250_CHANNEL
{
	eRF5250_CLOSE,
	eRF5250_CHANNEL1,
	eRF5250_CHANNEL2,
	eRF5250_CHANNEL3,
	eRF5250_CHANNEL4,
	eRF5250_CHANNEL5,
};

#if RF_T3 == 1
enum HMC345ALP3E_CHANNEL
{
	RF_400M_6000M,
	RF_2400M_2460M,
	RF_2450M_2510M,
	RF_5150M_5250M,
	RF_5650M_5850M,
};
#else
enum HMC345ALP3E_CHANNEL
{
	FRQ_RANG_400M_6000M,
	FRQ_RANG_400M_1000M,
	FRQ_RANG_1000M_2000M,
	FRQ_RANG_2400M_2483M,
	FRQ_RANG_5100M_5300M,
	FRQ_RANG_5650M_6000M,
};
#endif
enum HMC1119_SERNIN
{
	eHMC1119_SERNIN0, // attenuation 0 db
	eHMC1119_SERNIN1, // attenuation 0.25
	eHMC1119_SERNIN2, // attenuation 0.5
	eHMC1119_SERNIN3, // attenuation 1
	eHMC1119_SERNIN4, // attenuation 2
	eHMC1119_SERNIN5, // attenuation 4
	eHMC1119_SERNIN6, // attenuation 8
	eHMC1119_SERNIN7, // attenuation  16
	eHMC1119_SERNIN8, // attenuation 31.75
};

uint8_t SetSwitchFilterGain(uint16_t value);
void Adrf5250Ctrl(uint8_t channel);
void Adrf5250Disable();

int32_t SwitchFilterGainIoCtrl(uint8_t value);
