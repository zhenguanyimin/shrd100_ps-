/**
 * config for all
 */
#ifndef __CFG__H__
#define __CFG__H__

//ad9361
enum Freq_CFG
{
//	Freq_RX_433m,
//	Freq_RX_842m,
//	Freq_RX_915m,
	Freq_RX_2420m,
//	Freq_RX_837m,
//	Freq_RX_2406m,
	Freq_RX_2460m,
//	Freq_RX_5160m,
//	Freq_RX_5200m,
//	Freq_RX_5240m,
	Freq_RX_5745m,
	Freq_RX_5785m,
	Freq_RX_5825m,
	Freq_RX_5865m,

//	Freq_RX_875m,
//	Freq_RX_2417m,
//	Freq_RX_2447m,
//	Freq_RX_2477m,
//	Freq_RX_5157m,
//	Freq_RX_5187m,
//	Freq_RX_5217m,
//	Freq_RX_5247m,
//	Freq_RX_5742m,
//	Freq_RX_5772m,
//	Freq_RX_5802m,
//	Freq_RX_5832m,
//	Freq_RX_5862m,
	Freq_MAX_CNT,
};


/* Gets the RX frequency. */
static uint32_t get_rx_freq(uint32_t freqItem)
{
	uint32_t freq = 0;
	switch(freqItem)
	{
//	case Freq_RX_875m:
//	{
//		freq = 875;
//	}break;
//	case Freq_RX_2417m:
//	{
//		freq = 2417;
//	}break;
//	case Freq_RX_2447m:
//	{
//		freq = 2447;
//	}break;
//	case Freq_RX_2477m:
//	{
//		freq = 2477;
//	}break;
//	case Freq_RX_5157m:
//	{
//		freq = 5157;
//	}break;
//	case Freq_RX_5187m:
//	{
//		freq = 5187;
//	}break;
//	case Freq_RX_5217m:
//	{
//		freq = 5217;
//	}break;
//	case Freq_RX_5247m:
//	{
//		freq = 5247;
//	}break;
//	case Freq_RX_5742m:
//	{
//		freq = 5742;
//	}break;
//	case Freq_RX_5772m:
//	{
//		freq = 5772;
//	}break;
//	case Freq_RX_5802m:
//	{
//		freq = 5802;
//	}break;
//	case Freq_RX_5832m:
//	{
//		freq = 5832;
//	}break;
//	case Freq_RX_5862m:
//	{
//		freq = 5862;
//	}break;

//	case Freq_RX_433m:
//	{
//		freq = 433;
//	}break;
//	case Freq_RX_837m:
//	{
//		freq = 837;
//	}break;
//	case Freq_RX_2406m:
//	{
//		freq = 2406;
//	}break;
//	case Freq_RX_842m:
//	{
//		freq = 842;
//	}break;
//	case Freq_RX_915m:
//	{
//		freq = 915;
//	}break;
	case Freq_RX_2420m:
	{
		freq = 2420;
	}break;
	case Freq_RX_2460m:
	{
		freq = 2460;
	}break;
//	case Freq_RX_5160m:
//	{
//		freq = 5160;
//	}break;
//	case Freq_RX_5200m:
//	{
//		freq = 5200;
//	}break;
//	case Freq_RX_5240m:
//	{
//		freq = 5240;
//	}break;
	case Freq_RX_5745m:
	{
		freq = 5745;
	}break;
	case Freq_RX_5785m:
	{
		freq = 5785;
	}break;
	case Freq_RX_5825m:
	{
		freq = 5825;
	}break;
	case Freq_RX_5865m:
	{
		freq = 5865;
	}break;
	case Freq_MAX_CNT:
	default:
		break;
	}

	return freq;
}


#endif

