/*
 * display.c
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
/* FreeRTOS includes. */
#include "rf_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <timers.h>
#include "pl_config.h"
#include "../../cfg/shrd_config.h"
#include "../../hal/flash_hal/flash_hal.h"
#include "../../hal/hal.h"
#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../../hal/hal_ad9361/zynq_bram.h"
#include "../../hal/input/input.h"
#include "../../srv/log/log.h"
#include "../alg/alg_init.h"
#include "../data_path/data_path.h"
#include "../sys_status_data/detection_param.h"

#include "../switch_filter/switch_filter.h"
#include "../alg/droneID/droneID_utils.h"
#include "../../app/key_handle/interrupt_handle.h"
#include "../../app/process_task/detect_process_task.h"

static TimerHandle_t TimerAD9361Init = NULL;
static QueueHandle_t xRf_ConfigQueue = NULL;

void Rf_Ad9361Sweep_f(void);
void Rf_ConfigTask_Post(uint32_t flag)
{
	if (xRf_ConfigQueue != NULL)
	{
		xQueueReset(xRf_ConfigQueue);
		xQueueSend(xRf_ConfigQueue, &flag, 10);
	}
}

static int32_t Rf_ConfigTask_Pend(uint32_t *flag, uint32_t timeout)
{

	int32_t result = -1;

	if (xRf_ConfigQueue != NULL)
	{
		if (xQueueReceive(xRf_ConfigQueue, flag, timeout))
		{
			result = 0;
		}
	}

	return result;
}

static const uint64_t uScanFreqBuf1[] =
{
		2414500e3,	//	2414500e3,
		2429495e3,	//	2429500e3,
		2444495e3,	//	2444500e3,
		2459500e3,	//	2459500e3,
		5756480e3,	//	5756500e3,
		5776482e3,	//	5776500e3,
		5796490e3,	//	5796500e3,
		5816488e3,	//	5816500e3,
};

static uint64_t droneID_sig_freq_cfg_buf[] =
{
		2414500e3,
		2429500e3,
		2444500e3,
		2459500e3,
		5756500e3,
		5776500e3,
		5796500e3,
		5816500e3,
};

static freq_cfg_t freq_cfg_buf[] =
{
		{2422000e3, -7500e3, 7500e3},
		{2452000e3, -7500e3, 7500e3},
		{5766500e3, -10000e3, 10000e3},
		{5806500e3, -10000e3, 10000e3},
};
//static freq_cfg_t freq_cfg_buf[] =
//{
//		{2422000e3, -7500e3, 7500e3},
//		{2452000e3, -7500e3, 7500e3},
//		{5766000e3, -9500e3, 10500e3},
//		{5806000e3, -9500e3, 10500e3},
//};
//static freq_cfg_t freq_cfg_buf[] =
//{
//		{2414000e3, 500e3, 500e3},
//		{2429000e3, 500e3, 500e3},
//		{2444000e3, 500e3, 500e3},
//		{2459000e3, 500e3, 500e3},
//		{5756000e3, 500e3, 500e3},
//		{5776000e3, 500e3, 500e3},
//		{5796000e3, 500e3, 500e3},
//		{5816000e3, 500e3, 500e3},
//};

static freq_cfg_t freq_diff_cfg_buf[] =
{
		{2422000e3, 0, -5000},
		{2452000e3, -5000, 0},
		{5766500e3, -20000, -18000},
		{5806500e3, -10000, -12000},
};

freq_cfg_t* GetFreq_cfg_buf(void)
{
	return &freq_cfg_buf;
}

void GetFreqRegValue(void)
{
	uint32_t uIdxFreqoffset = 0;
	uint32_t uRfMod = 0;
#if( WORKMODE == DRONEID)
	uIdxFreqoffset = axi_read(eIDX_FREQOFFSET);
	uRfMod = axi_read(eRF_MOD);
#endif

//	LOG_DEBUG("RfMode:%d, IdxFreqoffset:%d\n", uRfMod, uIdxFreqoffset);
	if (uRfMod < (sizeof(freq_cfg_buf) / sizeof(freq_cfg_buf[0])))
	{
		if (uIdxFreqoffset == 0)
		{
			SetFreqOffsetHz(freq_cfg_buf[uRfMod].uFreqOffsetL);
//			SetFreqOffsetHz(freq_cfg_buf[uRfMod].uFreqOffsetL + freq_diff_cfg_buf[uRfMod].uFreqOffsetL);
		}
		else
		{
			SetFreqOffsetHz(freq_cfg_buf[uRfMod].uFreqOffsetR);
//			SetFreqOffsetHz(freq_cfg_buf[uRfMod].uFreqOffsetR + freq_diff_cfg_buf[uRfMod].uFreqOffsetR);
		}
		SetDroneIDSigFreqHz(droneID_sig_freq_cfg_buf[uRfMod * 2 + uIdxFreqoffset]);
	}
	else
	{
		LOG_ERROR("RfMode:%d, error\n", uRfMod);
	}
}


static uint8_t uDetectFreqItem = 0;
void SetDetectFreqItem(uint8_t value)
{
	if (value < Freq_MAX_CNT)
	{
		uDetectFreqItem = value;
	}
	else
	{
		uDetectFreqItem = 0;
	}
	set_rx_freq(uDetectFreqItem);
}

uint8_t GetDetectFreqItem(void)
{
	return uDetectFreqItem;
}

static uint8_t uSweepFlag = 1;
void SetSweepFlag(uint8_t value)
{
	uSweepFlag = value;
}

static void TimerAD9361InitCallback(void)
{

}
int32_t RF_SetSignalChannel_T1(int32_t CurFreqIndex)
{
	int32_t RunResult = 0;
	static uint8_t channel = eRF5250_CHANNEL3;
	uint8_t uDetectFreqItem = CurFreqIndex;
	sPL_SignalHandlingCfg *sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();

	sPL_Cfg->uLocalOscFreq = SYS_GetCenterFreq(uDetectFreqItem);

#if( WORKMODE == DRONEID)
	axi_write(eCFG_RF_MOD, uDetectFreqItem);
#endif

	LOG_DEBUG("uSweepFlag:CurFreqIndex:%d  CfgFreq:%0.1f \r\n", CurFreqIndex , sPL_Cfg->uLocalOscFreq);

//	if (((uSweepFlag & 0xF0) == 0x00))
	{
		if ((sPL_Cfg->uLocalOscFreq > 2200) && (sPL_Cfg->uLocalOscFreq <= 2500))
		{
			if (channel != eRF5250_CHANNEL3)
			{
				channel = eRF5250_CHANNEL3;
				Adrf5250Ctrl(eRF5250_CHANNEL3);
			}
		}
		else if ((sPL_Cfg->uLocalOscFreq > 5000) && (sPL_Cfg->uLocalOscFreq <= 5900))
		{
			if (channel != eRF5250_CHANNEL5)
			{
				channel = eRF5250_CHANNEL5;
				Adrf5250Ctrl(eRF5250_CHANNEL5);
			}
		}
		else if ((sPL_Cfg->uLocalOscFreq >= 400) && (sPL_Cfg->uLocalOscFreq <= 1000))
		{
			if (channel != eRF5250_CHANNEL1)
			{
				channel = eRF5250_CHANNEL1;
				Adrf5250Ctrl(eRF5250_CHANNEL1);
			}
		}
		else if ((sPL_Cfg->uLocalOscFreq > 1000) && (sPL_Cfg->uLocalOscFreq <= 2200))
		{
			if (channel != eRF5250_CHANNEL2)
			{
				channel = eRF5250_CHANNEL2;
				Adrf5250Ctrl(eRF5250_CHANNEL2);
			}
		}
		else if ((sPL_Cfg->uLocalOscFreq > 3100) && (sPL_Cfg->uLocalOscFreq <= 4500))
		{
			if (channel != eRF5250_CHANNEL4)
			{
				channel = eRF5250_CHANNEL4;
				Adrf5250Ctrl(eRF5250_CHANNEL4);
			}
		}
		else
		{
			Adrf5250Ctrl(eRF5250_CHANNEL3);
		}
	}

	return RunResult;
}

#if RF_T3 == 1
static void RfPowerPtotect()
{
	uint8_t rf_1Q_value = 0 , rf_2Q_value = 0 ;

	int delay_time[3] = { 20000 , 10000 , 5000};


	rf_1Q_value = Input_GetValue(EMIO_RF_1Q);
	rf_2Q_value = Input_GetValue(EMIO_RF_2Q);

	if( ( rf_1Q_value == 0x0 ) && ( rf_2Q_value == 0x0 ) )
	{
		return;
	}

	for( int i = 0 ; i < 3 ; i++ )
	{
		GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOL);

		//RX2
		GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOL);

		vTaskDelay(delay_time[i]);

		GPIO_OutputCtrl(EMIO_RF_RESET, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_RESET, IO_VOL);

		rf_1Q_value = Input_GetValue(EMIO_RF_1Q);
		rf_2Q_value = Input_GetValue(EMIO_RF_2Q);
		if( ( rf_1Q_value == 0x0 ) && ( rf_2Q_value == 0x0 ) )
		{
			return;
		}

	}

}

void RF_TEST(uint8_t channel)
{
	if (channel == RF_2400M_2460M)
	{
		//RX1
		GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOL);

		GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOH);

		//RX2
		GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOL);

		GPIO_OutputCtrl(EMIO_RF_CTLD_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLD_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLD_3, IO_VOH);
	}
	else if (channel == RF_2450M_2510M)
	{
		//RX1
		GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOH);

		GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOL);


		//RX2
		GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOH);

		GPIO_OutputCtrl(EMIO_RF_CTLD_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLD_2, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLD_3, IO_VOL);
	}
	else if (channel == RF_5150M_5250M)
	{
		//RX1
		GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOH);

		GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOL);

		//RX2
		GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOH);

		GPIO_OutputCtrl(EMIO_RF_CTLD_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLD_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLD_3, IO_VOL);
	}
	else if (channel == RF_5650M_5850M)
	{
		//RX1
		GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOL);

		GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOH);

		//RX2
		GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOL);

		GPIO_OutputCtrl(EMIO_RF_CTLD_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLD_2, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLD_3, IO_VOH);
	}
	else
	{
		//RX1
		GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOH);

		GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOL);

		//RX2
		GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOH);

		GPIO_OutputCtrl(EMIO_RF_CTLD_1, IO_VOH);
		GPIO_OutputCtrl(EMIO_RF_CTLD_2, IO_VOL);
		GPIO_OutputCtrl(EMIO_RF_CTLD_3, IO_VOL);
	}

}
int32_t RF_SetSignalChannel(int32_t CurFreqIndex)
{
	int32_t RunResult = 0;
	static uint8_t channel = eRF5250_CHANNEL3;
	uint8_t uDetectFreqItem = CurFreqIndex;
	sPL_SignalHandlingCfg *sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();

	RfPowerPtotect();

	sPL_Cfg->uLocalOscFreq = SYS_GetCenterFreq(uDetectFreqItem);

#if( WORKMODE == DRONEID)
	axi_write(eCFG_RF_MOD, uDetectFreqItem);
#endif
	LOG_DEBUG("uSweepFlag:CurFreqIndex:%d  CfgFreq:%0.1f \r\n", CurFreqIndex , sPL_Cfg->uLocalOscFreq);


//	if (((uSweepFlag & 0xF0) == 0x00))
	{
		if ((sPL_Cfg->uLocalOscFreq > 2400) && (sPL_Cfg->uLocalOscFreq <= 2460))
		{
			if (channel != RF_2400M_2460M)
			{

				//RX1
				channel = RF_2400M_2460M;
				GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOL);

				GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOH);

				//RX2
				GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOL);

				GPIO_OutputCtrl(EMIO_RF_CTLD_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLD_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLD_3, IO_VOH);
			}
		}
		else if ((sPL_Cfg->uLocalOscFreq > 2450) && (sPL_Cfg->uLocalOscFreq <= 2510))
		{
			if (channel != RF_2450M_2510M)
			{
				channel = RF_2450M_2510M;

				//RX1
				GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOH);

				GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOL);


				//RX2
				GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOH);

				GPIO_OutputCtrl(EMIO_RF_CTLD_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLD_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLD_3, IO_VOL);
			}
		}
		else if ((sPL_Cfg->uLocalOscFreq >= 5150) && (sPL_Cfg->uLocalOscFreq <= 5250))
		{
			if (channel != RF_5150M_5250M)
			{
				channel = RF_5150M_5250M;

				//RX1
				GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOH);

				GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOL);

				//RX2
				GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOH);

				GPIO_OutputCtrl(EMIO_RF_CTLD_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLD_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLD_3, IO_VOL);
			}
		}
		else if ((sPL_Cfg->uLocalOscFreq > 5650) && (sPL_Cfg->uLocalOscFreq <= 5850))
		{
			if (channel != RF_5650M_5850M)
			{
				channel = RF_5650M_5850M;

				//RX1
				GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOL);

				GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOH);

				//RX2
				GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOL);

				GPIO_OutputCtrl(EMIO_RF_CTLD_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLD_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLD_3, IO_VOH);
			}
		}
		else
		{
			if (channel != RF_400M_6000M)
			{
				channel = RF_400M_6000M;

				//RX1
				GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOH);

				GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOL);

				//RX2
				GPIO_OutputCtrl(EMIO_RF_CTLC_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLC_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLC_3, IO_VOH);

				GPIO_OutputCtrl(EMIO_RF_CTLD_1, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLD_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLD_3, IO_VOL);
			}

		}
	}

	return RunResult;
}
#else
int32_t RF_SetSignalChannel(int32_t CurFreqIndex)
{
	int32_t RunResult = 0;
	static uint8_t channel = eRF5250_CHANNEL3;
	uint8_t uDetectFreqItem = CurFreqIndex;
	sPL_SignalHandlingCfg *sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();

	sPL_Cfg->uLocalOscFreq = SYS_GetCenterFreq(uDetectFreqItem);

	if( SYS_GetWorkMode() == DRONEID_MODE)
	{
		axi_write(eCFG_RF_MOD, uDetectFreqItem);
	}

	LOG_DEBUG("uSweepFlag:CurFreqIndex:%d  CfgFreq:%0.1f \r\n", CurFreqIndex , sPL_Cfg->uLocalOscFreq);

//	if (((uSweepFlag & 0xF0) == 0x00))
	{
		if ((sPL_Cfg->uLocalOscFreq >= 400) && (sPL_Cfg->uLocalOscFreq <= 1000))
		{
			if (channel != FRQ_RANG_400M_1000M)
			{
				channel = FRQ_RANG_400M_1000M;
				GPIO_OutputCtrl(EMIO_RF_CTLA_0, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_0, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOH);

				GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOH);
			}
		}
		if ((sPL_Cfg->uLocalOscFreq > 1000) && (sPL_Cfg->uLocalOscFreq < 2400))
		{
			if (channel != FRQ_RANG_1000M_2000M)
			{
				channel = FRQ_RANG_1000M_2000M;
				GPIO_OutputCtrl(EMIO_RF_CTLA_0, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_0, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOL);

				GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOL);
			}
		}
		else if ((sPL_Cfg->uLocalOscFreq >= 2400) && (sPL_Cfg->uLocalOscFreq <= 2483))
		{
			if (channel != FRQ_RANG_2400M_2483M)
			{
				channel = FRQ_RANG_2400M_2483M;


				GPIO_OutputCtrl(EMIO_RF_CTLA_0, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_0, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOH);

				GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOH);
			}
		}
		else if ((sPL_Cfg->uLocalOscFreq >= 5100) && (sPL_Cfg->uLocalOscFreq <= 5300))
		{
			if (channel != FRQ_RANG_5100M_5300M)
			{
				channel = FRQ_RANG_5100M_5300M;

				GPIO_OutputCtrl(EMIO_RF_CTLA_0, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_0, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOH);

				GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOH);
			}
		}
//		else if ((sPL_Cfg->uLocalOscFreq > 5650) && (sPL_Cfg->uLocalOscFreq <= 5850))
		else if (sPL_Cfg->uLocalOscFreq >= 5650)
		{
			if (channel != FRQ_RANG_5650M_6000M)
			{
				channel = FRQ_RANG_5650M_6000M;
				GPIO_OutputCtrl(EMIO_RF_CTLA_0, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_0, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLA_1, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_1, IO_VOL);

				GPIO_OutputCtrl(EMIO_RF_CTLA_2, IO_VOL);
				GPIO_OutputCtrl(EMIO_RF_CTLB_2, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLA_3, IO_VOH);
				GPIO_OutputCtrl(EMIO_RF_CTLB_3, IO_VOL);
			}
		}
		else
		{

		}
	}

	return RunResult;
}
#endif
int32_t RF_ConfigParamByFreqId(int32_t CurFreqIndex )
{
	int32_t RunResult;

	RunResult = RF_SetSignalChannel(CurFreqIndex);

	if (RunResult == 0)
	{
		SetRxFreq(SYS_GetCenterFreq(CurFreqIndex));
	}

	return RunResult;
}

void RfConfig_Init()
{
	sPL_SignalHandlingCfg *sPL_Cfg;
	uint32_t PL_Version_date;
	uint32_t PL_Version_time;


	PL_Version_date = axi_read_data(PL_VOERSION_VP0); /*set threshold*/
	PL_Version_time = axi_read_data(PL_VOERSION_VP1); /*set threshold*/

	axi_write_data(PL_FAN_PWM_RATE, 0x0000);
	axi_write_data(PL_DLY_EN_VTC, 0x0000);
	axi_write_data(PL_DLY_D, 150);
	axi_write_data(PL_DLY_LOAD, 0x1FFF);
	axi_write_data(PL_DLY_LOAD, 0x0000);
	axi_write_data(PL_DLY_EN_VTC, 0x1FFF);
	axi_write_data(PL_SL_SWITCH, 2);

	InitAd9361_Rx();

#if( WORKMODE == DRONEID)
	axi_write(eCFG_PWR_GATE_L, 0x00200000);
	axi_write(eCFG_PWR_GATE_H, 0x00000000);
	axi_write(eWR_DDR_ENABLE, 1);
#endif

	axi_write(0x80000024, 0x00);
	axi_write(0x80000028, 0x00);
	axi_write(0x8000002C, 0x135);
	axi_write(0x80000030, 0x135);
	axi_write(0x80000028, 0x7F);
	axi_write(0x80000024, 0x7F);

	axi_write_data(PL_AD_START, 1);

	sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();
	sPL_Cfg->uLocalOscFreq = SetRxFreq(SYS_GetCenterFreq(uDetectFreqItem));

	if ((sPL_Cfg->uLocalOscFreq >= 2200) && (sPL_Cfg->uLocalOscFreq <= 2500))
	{
		Adrf5250Ctrl(eRF5250_CHANNEL3);
	}
	else if ((sPL_Cfg->uLocalOscFreq >= 5000) && (sPL_Cfg->uLocalOscFreq <= 5900))
	{
		Adrf5250Ctrl(eRF5250_CHANNEL5);
	}
	else
	{
		Adrf5250Ctrl(eRF5250_CHANNEL3);
	}
	SetSwitchFilterGain(1500);

	LOG_DEBUG("PL_Version: %X %X\r\n", PL_Version_date & 0xFFFF, PL_Version_time);

	axi_write_data( PL_IRDRATE, IRD_RATE_5_12us );


}


void RfConfig_droneid_Init()
{
	sPL_SignalHandlingCfg *sPL_Cfg;
	uint32_t PL_Version_date;
	uint32_t PL_Version_time;

	PL_Version_date = axi_read_data(PL_VOERSION_VP0); /*set threshold*/
	PL_Version_time = axi_read_data(PL_VOERSION_VP1); /*set threshold*/

	axi_write_data(PL_FAN_PWM_RATE, 0x0001);
	axi_write_data(PL_DLY_EN_VTC, 0x0000);
	axi_write_data(PL_DLY_D, 150);
	axi_write_data(PL_DLY_LOAD, 0x1FFF);
	axi_write_data(PL_DLY_LOAD, 0x0000);
	axi_write_data(PL_DLY_EN_VTC, 0x1FFF);
	axi_write_data(PL_SL_SWITCH, 2);

	InitAd9361_Rx_droneid();

#if( WORKMODE == DRONEID)
	axi_write(eCFG_PWR_GATE_L, 0x00200000);
	axi_write(eCFG_PWR_GATE_H, 0x00000000);
	axi_write(eWR_DDR_ENABLE, 1);
#endif

	axi_write(0x80000024, 0x00);
	axi_write(0x80000028, 0x00);
	axi_write(0x8000002C, 0xA6);
	axi_write(0x80000028, 0x7F);
	axi_write(0x80000024, 0x7F);

	axi_write_data(PL_AD_START, 1);

	sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();
	sPL_Cfg->uLocalOscFreq = SetRxFreq(SYS_GetCenterFreq(uDetectFreqItem));

	if ((sPL_Cfg->uLocalOscFreq >= 2200) && (sPL_Cfg->uLocalOscFreq <= 2500))
	{
		Adrf5250Ctrl(eRF5250_CHANNEL3);
	}
	else if ((sPL_Cfg->uLocalOscFreq >= 5000) && (sPL_Cfg->uLocalOscFreq <= 5900))
	{
		Adrf5250Ctrl(eRF5250_CHANNEL5);
	}
	else
	{
		Adrf5250Ctrl(eRF5250_CHANNEL3);
	}
	SetSwitchFilterGain(1500);

	LOG_DEBUG("PL_Version: %X %X\r\n", PL_Version_date & 0xFFFF, PL_Version_time);

	axi_write_data( PL_IRDRATE, IRD_RATE_5_12us );

}


void Rf_Config_Task(void *p_arg)
{
	uint32_t flag;

	RfConfig_Init();

	while (1)
	{
		if (Rf_ConfigTask_Pend(&flag, 650) == 0)
		{
			enum RF_EVENT rf_flag = (enum RF_EVENT)flag;
			switch (rf_flag)
			{
			case eRF_AD9361_CFG_F:
			{
				Rf_Ad9361Sweep_f();
				break;
			}
			case eRF_PL_START_COLLECT:
			{
				axi_write_data(PL_AD_START, 1);
				break;
			}
			case eRF_PL_STOP_COLLECT:
			{
				axi_write_data(PL_AD_START, 0);
				break;
			}
			case eRF_AD9361_CFG_HIT_915:
			{
				SetAlgorithmStopFlag(1);
				data_path_out_en(0);
				InitAd9361();
				Ad9361_fre_hop_stop();
				Ad9361_fre_hop_set(Freq_TX_915m);
				Ad9361_fre_hop_start();
				break;
			}
			case eRF_AD9361_CFG_HIT_GPS:
			{
				SetAlgorithmStopFlag(1);
				data_path_out_en(0);
				InitAd9361();
				Ad9361_fre_hop_stop();
				Ad9361_fre_hop_set(Freq_TX_GPS);
				Ad9361_fre_hop_start();
				break;
			}
			case eRF_AD9361_CFG_HIT_GPS_915:
			{
				SetAlgorithmStopFlag(1);
				data_path_out_en(0);
				InitAd9361();
				Ad9361_fre_hop_stop();
				Ad9361_fre_hop_set(Freq_TX_GPS_915m);
				Ad9361_fre_hop_start();
				break;
			}
			case eRF_AD9361_CFG_CLOSE_HIT:
			{
				//				reset_ad936x();
				Ad9361_fre_hop_stop();
				InitAd9361_Rx();
				SetAlgorithmStopFlag(0);
				axi_write_data(PL_AD_START, 1);
				data_path_out_en(4);
				break;
			}
			default:
				break;
			}
		}
		else
		{
//			Rf_Ad9361_cfg_f();
			DET_CfgNextDetection();
		}
	}
}

void Rf_Ad9361Sweep_f(void)
{
	static uint8_t channel = eRF5250_CHANNEL3;
	sPL_SignalHandlingCfg *sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();

	uDetectFreqItem++;
	if (uDetectFreqItem >= Freq_MAX_CNT)
	{
		uDetectFreqItem = 0;
	}
	sPL_Cfg->uLocalOscFreq = SYS_GetCenterFreq(uDetectFreqItem);
//	LOG_DEBUG("sPL_Cfg->uLocalOscFreq:  %d \r\n", sPL_Cfg->uLocalOscFreq);

	if ((sPL_Cfg->uLocalOscFreq >= 2200) && (sPL_Cfg->uLocalOscFreq <= 2500))
	{
		if (channel != eRF5250_CHANNEL3)
		{
			channel = eRF5250_CHANNEL3;
			Adrf5250Ctrl(eRF5250_CHANNEL3);
		}
	}
	else if ((sPL_Cfg->uLocalOscFreq >= 5000) && (sPL_Cfg->uLocalOscFreq <= 5900))
	{
		if (channel != eRF5250_CHANNEL5)
		{
			channel = eRF5250_CHANNEL5;
			Adrf5250Ctrl(eRF5250_CHANNEL5);
		}
	}
	else
	{
		Adrf5250Ctrl(eRF5250_CHANNEL3);
	}
	if (TimerAD9361Init)
	{
		xTimerStart(TimerAD9361Init, 10);
	}
	set_rx_freq(uDetectFreqItem);

	if (TimerAD9361Init)
	{
		xTimerStop(TimerAD9361Init, 10);
	}
}

void Rf_Ad9361_cfg_f(void)
{
	sPL_SignalHandlingCfg *sPL_Cfg = aeagCfg_GetPlSignalHandlingCfg();

	if ((GetAlgorithmStopFlag() == 0) && (uSweepFlag == 1))
	{
		Rf_Ad9361Sweep_f();
	}

	/* double frequence process */

#if( WORKMODE == DRONEID)
	axi_write(eCFG_RF_MOD, uDetectFreqItem);
#endif

	sPL_Cfg->uLocalOscFreq = SYS_GetCenterFreq(uDetectFreqItem);
	LOG_DEBUG("sPL_Cfg->uLocalOscFreq:  [%d] %d \r\n", uDetectFreqItem, sPL_Cfg->uLocalOscFreq);
}

int32_t InitRf_Config_Task()
{
	int32_t retVal = -1;

	xRf_ConfigQueue = xQueueCreate(1, sizeof(uint32_t));

	TimerAD9361Init = xTimerCreate((const char *)"TimerRf",
								   (TickType_t)5000,
								   (UBaseType_t)pdFALSE,
								   (void *)1,
								   (TimerCallbackFunction_t)TimerAD9361InitCallback);
	if (TimerAD9361Init == NULL)
	{
		return retVal;
	}

	if (xTaskCreate(Rf_Config_Task, "Rf_Config_Task", TASK_STACK_SIZE_RF_CONFIG, NULL, TASK_PRI_RF_CONFIG, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}
