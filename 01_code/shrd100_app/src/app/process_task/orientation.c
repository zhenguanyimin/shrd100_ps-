/* Standard includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include "orientation.h"

#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../../hal/input/input.h"
#include "../../srv/heartbeat_packet/heartbeat_packet.h"
#include "../../srv/log/log.h"
#include "../alg/axisConvert.h"
#include "../alg/dirCalc.h"
#include "../alg/ifDirCalc.h"
#include "../data_path/data_path.h"
#include "../rf_config/rf_config.h"
#include "../sys_status_data/detection_param.h"
#include "../sys_status_data/detection_target.h"
#include "../devInfo_process/devInfo_process.h"
#include "../../app/process_task/detect_interface.h"
#include "../../drv/uart_ps/uartps_drv.h"
void OrientateDebugPrintf(void);
#define LEVEL_CNT_MAX 256
#define PITCH_CNT_MAX 256
#define ORIENTATION_CNT_MAX 256


SemaphoreHandle_t OrientationMutex = NULL;
static int32_t LevelDetecte[3][LEVEL_CNT_MAX];
static int32_t PitchAngle[3][PITCH_CNT_MAX];
static uint8_t ModeChangeFlag = 0;
static uint16_t GroupCnt = 0;
static int GroupCntNumDx = 0;
static uint8_t OrientateStep = STEP_IF_ENTER_DIR;
static uint8_t ifDirCircFreqFlag = 0;
static int32_t LevelAmpMax = 0, PitchAmpMax = 0;
static float LevelAmpMax1 = 0, PitchAmpMax1 = 0;

static int32_t LevelAngleCen = 0x7fffffff, PitchAngleCen = 0x7fffffff;
static float LevelAngleCen1 = 0x7fffffff, PitchAngleCen1 = 0x7fffffff;
double latDrone; double lngDrone;
double hDrone;

static ORI_orientationStatus_e OrientateStates = ORIENTATION_STATUS_NONE; // 给外部蜂鸣器指示使用，表示目前定向到哪一步了
static uint8_t ifOrientateProgressing = 0; // 由于判断目前是否正在定向计算
static QueueHandle_t xOrientateDetParamQueue = NULL;

static TimerHandle_t TimerFuncKeyHandle = NULL;

int32_t IfChangeAntenna = 0;

uint8_t EnterFuncKeyFlag = 0;
void SetLevelAngleCen(int32_t data)
{
	LevelAngleCen = data;
}

int32_t GetLevelAngleCen(void)
{
	return LevelAngleCen;
}

void SetPitchAngleCen(int32_t data)
{
	PitchAngleCen = data;
}
int32_t GetPitchAngleCen(void)
{
	return PitchAngleCen;
}

ORI_orientationStatus_e GetOrientationStatus(void)
{
	return OrientateStates;
}

void SetOrientationStatus(ORI_orientationStatus_e status)
{
	OrientateStates = status;
}

static uint8_t OrientateDetMode = 0;// 0 no; 1:LevelDetecte   2:PitchAngle
void SetOrientateDetMode(uint8_t mode)
{
	if ((OrientateDetMode != mode) && (mode == 0))
	{
		ModeChangeFlag = 1;
	}
	OrientateDetMode = mode;
}

static uint8_t ManualOrientateMode = 0;//
static int32_t ManualAngle = 0;//
static int32_t ManualPitch = 0;//
static int32_t ManualAmp;
static uint8_t ManualFreqItem;
void SetManualOrientateMode(uint8_t mode, int32_t Angle, int32_t Pitch, int32_t Amp, uint8_t FreqItem)
{
	ManualOrientateMode = mode;
	ManualAngle = Angle;
	ManualPitch = Pitch;
	ManualAmp = Amp;
	ManualFreqItem = FreqItem;
}


static uint8_t DroneLocationMode = 0;//
static int32_t DroneAngle = 0;//
static int32_t DronePitch = 0;//

uint16_t QxPower = 0x00;
uint16_t DxPower = 0x00;
int32_t DxHorizon = 0x00;

void get_dir_para( uint16_t* qQxPower ,uint16_t* qDxPower ,uint32_t* qdDxAngle  )
{
	qQxPower[0] 	= QxPower;
	qDxPower[0] 	= DxPower;
	qdDxAngle[0]	= DxHorizon;
}

void set_dir_para( uint16_t l_QxPower ,uint16_t l_DxPower ,float l_dDxAngle  )
{
	QxPower		= l_QxPower;
	DxPower 	= l_DxPower;
	DxHorizon	= (uint32_t)(l_dDxAngle*100);
}


void SetDroneLocationMode(uint8_t mode, int32_t Angle, int32_t Pitch)
{
	DroneLocationMode = mode;
	DroneAngle = Angle;
	DronePitch = Pitch;
}

uint8_t GetOrientateDetMode(void)
{
	return OrientateDetMode;
}

void SetAntennaMode(uint8_t mode)
{
	if (mode == ANTENNA_2_4)
	{
		GPIO_OutputCtrl(EMIO_CTRL_OUT7, IO_HIGTH);
		GPIO_OutputCtrl(EMIO_CTRL_OUT6, IO_LOW);
	}
	else if (mode == ANTENNA_5_8)
	{
		GPIO_OutputCtrl(EMIO_CTRL_OUT7, IO_LOW);
		GPIO_OutputCtrl(EMIO_CTRL_OUT6, IO_LOW);
	}
//	else if (mode == DISABLE_DIR_ANT)
//	{
//		GPIO_OutputCtrl(EMIO_CTRL_OUT7, IO_HIGTH);
//		GPIO_OutputCtrl(EMIO_CTRL_OUT6, IO_HIGTH);
//	}
}

uint8_t EligibleEntryOrientationFlag = 0;
uint8_t uav_number = 0;

uint8_t GetEligibleEntryOrientationFlag()
{
	return EligibleEntryOrientationFlag;
}
void SetEligibleEntryOrientationFlag(uint8_t flag)
{
	EligibleEntryOrientationFlag = flag;
}

void SetUavNumber(uint8_t flag)
{
	if(uav_number < SlideWinCntMAX)
	{
		uav_number = flag;
	}
	else
	{
		uav_number = 0 ;
	}

}
uint8_t GetUavNumber()
{
	return uav_number;
}

void JudgeEligibleEntryOrientation(void)
{
	static uint8_t LastStatus = 0;
	uint8_t Status = 0;

	Status = Input_GetValue(EMIO_GPS_SW);//按下是高电平
	if ((Status != LastStatus) && (EnterFuncKeyFlag == 0))
	{
		if ((GetSysWorkingMode() != eWORK_MODE_HIT_REMOTE_IMG) && (GetSysWorkingMode() != eWORK_MODE_HIT_GNSS)
				&& (GetSysWorkingMode() != eWORK_MODE_HIT_ALL))
		{
			if ((ifDirCircFreqFlag == 1) && (Status == 1))
			{
				EnterFuncKeyFlag = 1;

				if (TimerFuncKeyHandle)
				{
					xTimerStart(TimerFuncKeyHandle, 10);
				}

				EligibleEntryOrientationFlag = !EligibleEntryOrientationFlag;
				LOG_DEBUG("ifDirCircFreqFlag=%d,EligibleEntryOrientationFlag=%d\r\n", ifDirCircFreqFlag, EligibleEntryOrientationFlag);
				
				if (EligibleEntryOrientationFlag == 0)
				{
					if ((GetSysWorkingMode() != eWORK_MODE_ELIGIBLE_ENTRY_DIR) && (GetSysWorkingMode() != eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR))
					{
						SetSysWorkingMode(eWORK_MODE_DETECT);
					}
					EnterHitModeClearDir();
				}
			}
			else if (Status == 1)
			{
				EnterFuncKeyFlag = 1;
				if (TimerFuncKeyHandle)
				{
					xTimerStart(TimerFuncKeyHandle, 10);
				}
				LOG_DEBUG("ifDirCircFreqFlag=%d,Flag=%d\r\n", ifDirCircFreqFlag, EligibleEntryOrientationFlag);
				EligibleEntryOrientationFlag = 0;
				if ((GetSysWorkingMode() != eWORK_MODE_ELIGIBLE_ENTRY_DIR) && (GetSysWorkingMode() != eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR))
				{
				SetSysWorkingMode(eWORK_MODE_DETECT);
				}
				EnterHitModeClearDir();
			}
		}
	}
	LastStatus = Status;
}

uint8_t EnterManualStrategyMode = 0;
void SetEnterManualStrategyMode(uint8_t value)
{
	EnterManualStrategyMode = value;
}

uint8_t GetEnterManualStrategyMode(void)
{
	return EnterManualStrategyMode;
}

extern droneResult_t WifiMiniSeOut;
uint8_t IfDirMiniSeFlag = 0;
void JudgeIfDirMiniSe(void)
{
	DroneReportResult_t* pDroneReportResult;
	pDroneReportResult = SYS_GetDroneInfo();

	if (memcmp(pDroneReportResult->ReportResult[0].DroneResult.name, WifiMiniSeOut.name, strlen(WifiMiniSeOut.name)) == 0)
	{
		IfDirMiniSeFlag = 1;
	}
	else
	{
		IfDirMiniSeFlag = 0;
	}
}

uint8_t GetJudgeIfDirMiniSe(void)
{
	return IfDirMiniSeFlag;
}

void JudgeEligibleEntryOrientation1(void)
{
	static uint8_t LastStatus = 0;
	static TickType_t tick, tick2;
	uint8_t Status = 0;

	Status = Input_GetValue(EMIO_GPS_SW);//按下是高电平

	if ((Status == 1) && (LastStatus == 0))
	{
		tick = xTaskGetTickCount();
	}
	else if ((Status == 0) && (LastStatus == 1))
	{
		tick2 = xTaskGetTickCount();
		uint32_t time = tick2 - tick;
		LOG_DEBUG("time=%d\r\n", time);
//		if ((time >= 90) && (time <= 50000))
		if (time >= 90)
		{
			LOG_DEBUG("ifDirCircFreqFlag=%d,EligibleEntryOrientationFlag=%d,EnterManualStrategyMode=%d\r\n", ifDirCircFreqFlag, EligibleEntryOrientationFlag, EnterManualStrategyMode);
			if (EnterManualStrategyMode == 0)
			{
				if (ifDirCircFreqFlag == 1)
				{
					EligibleEntryOrientationFlag = !EligibleEntryOrientationFlag;
					LOG_DEBUG("EligibleEntryOrientationFlag=%d\r\n", EligibleEntryOrientationFlag);
					if (EligibleEntryOrientationFlag == 0)
					{
						if ((GetSysWorkingMode() != eWORK_MODE_ELIGIBLE_ENTRY_DIR) && (GetSysWorkingMode() != eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR))
						{
							SetSysWorkingMode(eWORK_MODE_DETECT);
							ClearDroneInfoSlideWin();
						}
						EnterHitModeClearDir();
					}
				}
				else
				{
					if (EligibleEntryOrientationFlag == 1)
					{
						EligibleEntryOrientationFlag = 0;
						if ((GetSysWorkingMode() != eWORK_MODE_ELIGIBLE_ENTRY_DIR) && (GetSysWorkingMode() != eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR))
						{
							ClearDroneInfoSlideWin();
							SetSysWorkingMode(eWORK_MODE_DETECT);
						}
						EnterHitModeClearDir();
					}
				}
			}
			else
			{
				EnterManualStrategyMode = 2;
			}
		}
//		else if ((time > 5000) && (time < 9000))
//		{
//			EnterManualStrategyMode = 1;
//			LOG_DEBUG("EnterManualStrategyMode=%d\r\n", EnterManualStrategyMode);
//		}
	}
	LastStatus = Status;
}

uint8_t shortKeyFlag = 0;
uint8_t longKeyFlag = 0;
uint8_t startCheckLongKeyRelase = 0;
uint8_t allowExitLevelOrientFlag = 0;
uint8_t firstInLongKeyFlag = 0;

void KeyScanf(void)
{
	uint32_t l_mcu_key = 0;

	l_mcu_key = get_muc_dir_key_cnt();

	if( l_mcu_key == 1 )
	{
		uav_number = 0 ;
		EligibleEntryOrientationFlag ^= 1 ;
		LOG_DEBUG("MCU PRESS KEY EVENT!\r\n");
	}

}

void JudgeEligibleEntryOrientation2(void)
{
	KeyScanf();
	if (shortKeyFlag || (firstInLongKeyFlag == 1))
	{
		LOG_DEBUG("ifDirCircFreqFlag=%d,EligibleEntryOrientationFlag=%d,EnterManualStrategyMode=%d,shortKeyFlag=%d\r\n", ifDirCircFreqFlag, EligibleEntryOrientationFlag, EnterManualStrategyMode,shortKeyFlag);
		if (EnterManualStrategyMode == 0)
		{
			if (ifDirCircFreqFlag == 1)
			{
				EligibleEntryOrientationFlag = !EligibleEntryOrientationFlag;
				LOG_DEBUG("EligibleEntryOrientationFlag=%d\r\n", EligibleEntryOrientationFlag);
				if (EligibleEntryOrientationFlag == 0)
				{
					if ((GetSysWorkingMode() != eWORK_MODE_ELIGIBLE_ENTRY_DIR) && (GetSysWorkingMode() != eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR))
					{
						SetSysWorkingMode(eWORK_MODE_DETECT);
						ClearDroneInfoSlideWin();
					}
					EnterHitModeClearDir();
				}
			}
			else
			{
				if (EligibleEntryOrientationFlag == 1)
				{
					EligibleEntryOrientationFlag = 0;
					if ((GetSysWorkingMode() != eWORK_MODE_ELIGIBLE_ENTRY_DIR) && (GetSysWorkingMode() != eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR))
					{
						ClearDroneInfoSlideWin();
						SetSysWorkingMode(eWORK_MODE_DETECT);
					}
					EnterHitModeClearDir();
				}
			}
		}
		else
		{
			EnterManualStrategyMode = 2;
		}

//		shortKeyFlag = !shortKeyFlag;
		shortKeyFlag = 0;

		if (firstInLongKeyFlag == 1)
			firstInLongKeyFlag = 2;
	}

	if (longKeyFlag == 1)
	{
		startCheckLongKeyRelase = 1;
		if (firstInLongKeyFlag == 0)
		{
			firstInLongKeyFlag = 1;
		}
	}

	if (startCheckLongKeyRelase == 1)
	{
		if (longKeyFlag == 0)
		{
			if (GetSysWorkingMode() == eWORK_MODE_SCANNING_HORI)
				allowExitLevelOrientFlag = 1;
		}
	}

	if (longKeyFlag == 0)
	{
		startCheckLongKeyRelase = 0;
	}


}

void EnterHitModeClearDir(void)
{
	GroupCntNumDx = 0;
//	statusN = 0;
	ifOrientateProgressing = 0;
	OrientateStates = ORIENTATION_STATUS_NONE;
	OrientateStep = STEP_IF_ENTER_DIR;
	ModeChangeFlag = 0;
	OrientateDetMode = 0;
	SetManualOrientateMode(0, 0, 0, 0, 0);
	SetAntennaMode(ANTENNA_ALL);
	SetLastDetFreqItem(0xFF);
//	UpdataUAVGpsInfo(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFF);
    if ((GetSysWorkingMode() >= eWORK_MODE_HIT_REMOTE_IMG) && (GetSysWorkingMode() <= eWORK_MODE_HIT_ALL))
    {
    }
    else
    {//打击模式下不更新定向侦测的角度
    	LOG_DEBUG("GetSysWorkingMode():%d\r\n", GetSysWorkingMode());
	SetLevelAngleCen(0x7FFFFFFF);
	SetPitchAngleCen(0x7FFFFFFF);
    }
	LevelAmpMax = 0;
	PitchAmpMax = 0;
//	HitProcessTask_Post(eEVENT_NO_WARNING);
	ifDirCircFreqFlag = 0;

	SYS_SetDetectionMode(DEFINED_SWEEP_FREQ_MODE);
	EligibleEntryOrientationFlag = 0;
	axi_write_data(PL_I_FPGA_TEMP3_DIR, 0);
//	SetAd9361Gain(76);
//	SetAd9361Gain(56, 66);
	allowExitLevelOrientFlag = 0;
}

int32_t DirGainCfg(int32_t AmpDatOmni)
{
	LOG_DEBUG("AmpDatOmni %d IfChangeAntenna:%d\r\n", AmpDatOmni, IfChangeAntenna);
	if (AmpDatOmni > 14500)
	{
		if (IfChangeAntenna == ANTENNA_2_4)
		{
//			SetAd9361Gain(50);
			SetAd9361Gain(50, 50);
			LOG_DEBUG("SetAd9361Gain 50 \r\n");
		}
		else
		{
//			SetAd9361Gain(60);
			SetAd9361Gain(60, 60);
			LOG_DEBUG("SetAd9361Gain 60 \r\n");
		}
	}
	return 0;
}

static float listAngle[ORIENTATION_CNT_MAX];
static float listAmQ[ORIENTATION_CNT_MAX];	// 全向
static float listAmD[ORIENTATION_CNT_MAX]; // 定向
static float listAmC[ORIENTATION_CNT_MAX];	//
static int listFP[ORIENTATION_CNT_MAX];   // 过门线

static float listAngleAz[ORIENTATION_CNT_MAX]; // 定向时对应的水平方位角
static float listAngleEl[ORIENTATION_CNT_MAX]; // 定向时对应的俯仰角

static float statusFW1 = 0; // 状态6方位瞄准对应的方位边界起始
static float statusFW2 = 0; // 状态6方位瞄准对应的方位边界终止
//static float statusFW3 = 0; // 状态6方位瞄准对应的方位瞄准值

static float printTmpUp = 0; // 用于输出调试信息
static float printTmpDown= 0; // 用于输出调试信息
static int8_t printIndex = 0; // 用于输出调试信息
static int32_t printCenFreq = 0; // 用于输出调试信息

void ClearFw1AndFw2(void)
{
	statusFW1 = 0;
	statusFW2 = 0;
	LevelAngleCen1 = 0;
}

uint16_t GetAzimuthStartAngle(void)
{
	return statusFW1 * 100;
}

uint16_t GetAzimuthEndAngle(void)
{
	return statusFW2 * 100;
}

void OrientateDebugPrintf(void)
{
	int32_t i;
	LOG_TEST("GroupCntNumDx=%d\r\n", GroupCntNumDx);
	{
		LOG_TEST("FW1=%0.1f,FW2=%0.1f,LevelAngle=%0.1f,TmpUp=%0.0f,TmpDown=%0.0f,targIndex=%d,CenFreq=%d,\r\n", statusFW1, statusFW2,
				LevelAngleCen1, printTmpUp, printTmpDown, printIndex, printCenFreq);
	}
	if (GroupCntNumDx > 0)
	{
		LOG_TEST("listFP(%d):", GroupCntNumDx);
	}
	LOG_PRINTF("\r\n");
	vTaskDelay(15);
	LOG_TEST("");
	for(i = 0; i < GroupCntNumDx; i++)
	{
		LOG_PRINTF("%d,", listFP[i]);
	}
	LOG_PRINTF("\r\n");
	vTaskDelay(15);
	LOG_TEST("");
	if (GroupCntNumDx > 0)
	{
		LOG_TEST("listAmQ(%d):", GroupCntNumDx);
	}
	LOG_PRINTF("\r\n");
	vTaskDelay(15);
	LOG_TEST("");
	for(i = 0; i < GroupCntNumDx; i++)
	{
		LOG_PRINTF("%0.0f,", listAmQ[i]);
	}
	LOG_PRINTF("\r\n");
	LOG_TEST("");
	if (GroupCntNumDx > 0)
	{
		LOG_TEST("listAmD(%d):", GroupCntNumDx);
	}
	LOG_PRINTF("\r\n");
	vTaskDelay(15);
	LOG_TEST("");
	for(i = 0; i < GroupCntNumDx; i++)
	{
		LOG_PRINTF("%0.2f,", listAmD[i]);
	}
	LOG_PRINTF("\r\n");
	LOG_TEST("");
	//1
	if (GroupCntNumDx > 0)
	{
		LOG_TEST("listAmC(%d):", GroupCntNumDx);
	}
	LOG_PRINTF("\r\n");
	vTaskDelay(15);
	LOG_TEST("");
	for(i = 0; i < GroupCntNumDx; i++)
	{
		LOG_PRINTF("%0.0f,", listAmC[i]);
	}
	LOG_PRINTF("\r\n");
	vTaskDelay(15);
	LOG_TEST("");

	if (GroupCntNumDx > 0)
	{
		LOG_TEST("listAngleAz(%d):", GroupCntNumDx);
	}
	LOG_PRINTF("\r\n");
	LOG_TEST("");
	for(i = 0; i < GroupCntNumDx; i++)
	{
		LOG_PRINTF("%0.2f,", listAngleAz[i]);
	}
	LOG_PRINTF("\r\n");
	vTaskDelay(15);
	LOG_TEST("");
	if (GroupCntNumDx > 0)
	{
		LOG_PRINTF("listAngleEl(%d):", GroupCntNumDx);
	}
	LOG_PRINTF("\r\n");
	vTaskDelay(15);
	LOG_TEST("");
	for(i = 0; i < GroupCntNumDx; i++)
	{
		LOG_PRINTF("%0.2f,", listAngleEl[i]);
	}
	LOG_PRINTF("\r\n");
}


ORI_orientationStatus_e CaculateOrientateStates(int level)
{
	ORI_orientationStatus_e ret = ORIENTATION_STATUS_NONE;
	if ((level == -1) || (level == 0))
	{
		ret = ORIENTATION_STATUS_WITIN_THE_RANGE_LEVEL_1;
	}
	else if ((level == 1) || (level == 2))
	{
		ret = ORIENTATION_STATUS_WITIN_THE_RANGE_LEVEL_2;
	}
	else
	{
		ret = ORIENTATION_STATUS_NONE;
	}

	return ret;
}

void EnterDetectModeClearDir(void)
{
//	OrientateStep = STEP_IF_ENTER_DIR;
	ModeChangeFlag = 0;
	OrientateDetMode = 0;
	SetManualOrientateMode(0, 0, 0, 0, 0);

	memset(listAmQ, 0 , ORIENTATION_CNT_MAX*sizeof(float));
	memset(listAmD, 0 , ORIENTATION_CNT_MAX*sizeof(float));
	memset(listAmC, 0 , ORIENTATION_CNT_MAX*sizeof(float));
	memset(listFP, 0 , ORIENTATION_CNT_MAX*sizeof(int));

	memset(listAngleAz, 0 , ORIENTATION_CNT_MAX*sizeof(float));
	memset(listAngleEl, 0 , ORIENTATION_CNT_MAX*sizeof(float));
	statusFW1 = 0;
	statusFW2 = 0;
	LevelAngleCen1 = 0;
	GroupCntNumDx = 0;

	ClearFw1AndFw2();

}
//void OrientateProcessV2(uint32_t FrameNo, int32_t Angle, int32_t Pitch, int32_t Amp, int32_t AmpDatOmni, uint8_t FreqItem, uint32_t Freq)

//void OrientateProcessV2(uint32_t FrameNo, int32_t Angle, int32_t Pitch, int32_t Amp, int32_t AmpDatOmni, uint8_t FreqItem, uint32_t Freq, unsigned short ***omniMat, unsigned short ***orienMat,
//		float bw, uint8_t CurrentFreqItem, float fs, char flag)
void OrientateProcess(uint32_t FrameNo, float Angle, float Pitch, int32_t Amp, uint8_t FreqItem, unsigned short ***omniMat, unsigned short ***orienMat,
		uint8_t CurrentFreqItem, float fs, DroneReportResult_t *pDroneReportResult)
{
	// 计算测幅频率范围
	float tmpF = 0;
	float tmpBW = 0;
	float dtF = fs / 128;
	float tmpUp = 0;
	float tmpDown = 0;
	float tmpML;
	float tmpMW;
	int ret = 0;
	int useRow = 3000;
	float cenFreq = 0;
	static uint8_t firstFlag = 0;
	static uint16_t GroupCnt = 0;
//	uint32_t i = 0;

	static uint8_t Flag = 0;
	static uint8_t AppropriateFreq = 0;
	int32_t IfChangeAntenna = 0;
	int32_t TotalAzimuth = 0;

	static int32_t AmpDatOmni = 0;
	static uint32_t Freq = 0;
	static float bw = 0;
	static char flag = 0;

	DET_OutsideInterface_t *pOutsideInterface = NULL;
	pOutsideInterface = DET_GetOutsideInterface();

	set_dir_para( listAmQ[GroupCntNumDx] , listAmD[GroupCntNumDx] , Angle );

	if ((Angle == -50000) && (Pitch == -50000) && (Pitch == -50000))
	{
		FreqItem = 0xFF;
		return;
	}

	if( OrientateStep == STEP_IF_ENTER_DIR )
	{
		SetSysWorkingMode(eWORK_MODE_ELIGIBLE_ENTRY_DIR);
//			if (ifDirCircFreq(Freq))
		if (EligibleEntryOrientationFlag == 1)
		{

			Freq = pDroneReportResult->ReportResult[uav_number].DroneResult.freq[0];
			AmpDatOmni = pDroneReportResult->ReportResult[uav_number].DroneResult.amp;
			bw = pDroneReportResult->ReportResult[uav_number].DroneResult.bw;
			flag = pDroneReportResult->ReportResult[uav_number].DroneResult.flag;

			AppropriateFreq = ifDirCircFreq(Freq);
			if (AppropriateFreq)
			{

				SYS_SetWorkMode(SPECTRUM_DX);

				JudgeIfDirMiniSe();
//					ifDirCircPitchflag = ifDirCircPitch(PitchAngle[0], GroupCnt);

				IfChangeAntenna = (ifDirCircFreq(SYS_GetCenterFreq(FreqItem)));

				LOG_DEBUG("IfChangeAntenna %d\r\n", IfChangeAntenna);
//					if (ifDirCircPitchflag != 0)

				SetDirDroneInfo();
				SetLevelAngleCen(0x7FFFFFFF);
				SetPitchAngleCen(0x7FFFFFFF);
				OrientateStep = STEP_IF_AZIMUTH_AIMED;

				SYS_SetDefinedFreqParamByItem(FreqItem);
				SYS_SetDetectionMode(DEFINED_FREQ_MODE);

				SetOrientateDetMode(MODE_LEVELDETECTE);//进入水平
				SetSysWorkingMode(eWORK_MODE_SCANNING_HORI);

//				DirGainCfg(AmpDatOmni);

				EnterDetectModeClearDir();
			}
			else
			{

				SetSysWorkingMode(eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR);
				LOG_DEBUG("FreqItem %d, get_rx_freq %f\r\n", FreqItem, SYS_GetCenterFreq(FreqItem));
			}
		}
	}
	else if( OrientateStep == STEP_IF_AZIMUTH_AIMED )
	{
		if( EligibleEntryOrientationFlag == 1 )
		{

			if (GroupCntNumDx >= (ORIENTATION_CNT_MAX - 1))
			{

				EnterDetectModeClearDir();

				LOG_DEBUG("frame cout out 256, GroupCntNumDx=%d\r\n", GroupCntNumDx);

			}
			else
			{
				listAngleAz[GroupCntNumDx] = Angle;
				listAngleEl[GroupCntNumDx] = Pitch;
			}

			cenFreq = SYS_GetCenterFreq(CurrentFreqItem);
			tmpF = Freq;
			tmpBW = bw;

			tmpML = 64 + ((tmpF - cenFreq) / dtF);
			tmpMW = ((tmpBW / dtF) / 2);
			tmpDown = (tmpML - tmpMW);
			tmpUp = (tmpML + tmpMW);

			printTmpUp = tmpDown + 2;
			printTmpDown = tmpUp - 2;
			printIndex = flag;
			printCenFreq = cenFreq;

			ret = sfDxNewProcessing(omniMat, orienMat, (int)(tmpDown + 2), (int)(tmpUp - 2), flag, useRow, listAngle, listAmQ, listAmD, listAmC, listFP, GroupCntNumDx);

			LOG_DEBUG("GroupCntNumDx=%d,FW1=%0.0f,FW2=%0.0f,LevelAngleCen1=%0.0f,tmpDown=%0.0f,tmpUp=%0.0f,flag=%d,cenFreq=%0.0f\r\n"
					, GroupCntNumDx, statusFW1, statusFW2, LevelAngleCen1, tmpDown, tmpUp, flag, cenFreq);

			if (GroupCntNumDx >= ORIENTATION_CNT_MAX)
			{
				OrientateDebugPrintf();
			}
			if (GroupCntNumDx < ORIENTATION_CNT_MAX)
			{
				GroupCntNumDx++;
			}

			if (GetSysWorkingMode() != eWORK_MODE_AIM)
			{
				if(sfDxIfAzimuthEnough(&listAngleAz[0], GroupCntNumDx, &statusFW1, &statusFW2) == 1)//判断水平扫描范围是否足够（超过360度）
				{

					ret =  sfDxIfLegal(listAngleAz, listAngleEl, listAmQ, listAmD, listAmC, listFP, GroupCntNumDx);
					OrientateDebugPrintf();
					if (ret == 0)
					{
//						OrientateStep = STEP_FAULT_ANGLE_NOT_ENOUGH;
						LOG_DEBUG("sfDxIfLegal() ret =%d\r\n", ret);
					}
					else if (ret == (-1))
					{
//						OrientateStep = STEP_FAULT_UNIDENTIFIED_TARGET;
						LOG_DEBUG("sfDxIfLegal() ret =%d\r\n", ret);
					}
					else
					{
						// 修改第二处 20230704
						int rret = 0;
						LOG_DEBUG("[dd]before sfDxDirCalc +++++++++++++\r\n");
						vTaskDelay(5);
						rret = sfDxDirCalc(&listAmD[0], &listAngleAz[0], GroupCntNumDx, &LevelAmpMax1, &LevelAngleCen1, (AppropriateFreq-1));
						LOG_DEBUG("LevelDetecte:  ampMax1 %f, angleCen1:%f\r\n", LevelAmpMax1, LevelAngleCen1);
						LOG_DEBUG("[dd]after sfDxDirCalc -------------\r\n");
						vTaskDelay(5);
						OrientateDebugPrintf();
						LOG_DEBUG("[dd]end ===================\r\n");

						if (rret == 0)
						{
//							OrientateStep = STEP_FAULT_UNIDENTIFIED_TARGET;
							LOG_DEBUG("sfDxDirCalc rret=%d\r\n", rret);
						}
						else
						{
							SetLevelAngleCen(LevelAngleCen1*100);
							SetSysWorkingMode(eWORK_MODE_AIM);
							GroupCnt = LEVEL_CNT_MAX/2;
						}
					}
					EnterDetectModeClearDir();
				}
			}
			else
			{
				if( GroupCnt <= LEVEL_CNT_MAX )
				{
					GroupCnt++;
//					SetLevelAngleCen(0x7FFFFFFF);
				}
			}
		}
		else
		{

			OrientateStep = STEP_IF_ENTER_DIR;
			SYS_SetWorkMode(SPECTRUM_QX);
			SetSysWorkingMode(eWORK_MODE_DETECT);
			SetLastDetFreqItem(0xFF);
			SetLevelAngleCen(0x7FFFFFFF);
			SetPitchAngleCen(0x7FFFFFFF);
			SYS_SetDetectionMode(DEFINED_SWEEP_FREQ_MODE);
			EnterDetectModeClearDir();

			LOG_DEBUG("FreqItem %d, get_rx_freq %d\r\n", FreqItem, get_rx_freq(FreqItem));

		}
	}

	LOG_DEBUG("FNo:%d Step:%d Angle:%d Pitch:%d AngleCen:%d PitchCen:%d tA:%d\r\n",
			FrameNo, OrientateStep, Angle, Pitch , LevelAngleCen, PitchAngleCen, TotalAzimuth);

    if (OrientationMutex)
    {
    	xSemaphoreGive(OrientationMutex);
    }
}

void OrientateProcessV2(uint32_t FrameNo, float Angle, float Pitch, int32_t Amp, uint8_t FreqItem, unsigned short ***omniMat, unsigned short ***orienMat,
		uint8_t CurrentFreqItem, float fs, DroneReportResult_t *pDroneReportResult)
{
	// 计算测幅频率范围
	float tmpF = 0;
	float tmpBW = 0;
	float dtF = fs / 128;
	float tmpUp = 0;
	float tmpDown = 0;
	float tmpML;
	float tmpMW;
	int ret = 0;
	int useRow = 3000;
	float cenFreq = 0;
	static uint8_t firstFlag = 0;
//	uint32_t i = 0;

	static uint8_t Flag = 0;
	static uint8_t AppropriateFreq = 0;
	int32_t IfChangeAntenna = 0;
	int32_t TotalAzimuth = 0;

	static int32_t AmpDatOmni = 0;
	static uint32_t Freq = 0;
	static float bw = 0;
	static char flag = 0;

	DET_OutsideInterface_t *pOutsideInterface = NULL;
	pOutsideInterface = DET_GetOutsideInterface();


	set_dir_para( listAmQ[GroupCntNumDx] , listAmD[GroupCntNumDx] , Angle );

    if (OrientationMutex)
    {
    	xSemaphoreTake(OrientationMutex, portMAX_DELAY);
    }

	if (ManualOrientateMode == 1)
	{
		Angle = ManualAngle;
		Pitch = ManualPitch;
		Amp = ManualAmp;
		FreqItem = ManualFreqItem;
	}
	else if (ManualOrientateMode == 2)
	{
		FreqItem = ManualFreqItem;
	}

	if ((Angle == -50000) && (Pitch == -50000) && (Pitch == -50000))
	{
		FreqItem = 0xFF;
	}

	if (FreqItem != 0xFF)
	{
//		if (((OrientateStep == STEP_IF_AZIMUTH_ENOUGH) || (OrientateStep == STEP_IF_AZIMUTH_AIMED)) && ((EligibleEntryOrientationFlag == 1) || (longKeyFlag == 1)))
		if ((OrientateStep == STEP_IF_AZIMUTH_ENOUGH) && ((EligibleEntryOrientationFlag == 1) || (longKeyFlag == 1)))
		{

			if (firstFlag == 0)
			{
				firstFlag = 1;
				GroupCntNumDx = 0;
				statusFW1 = 0;
				statusFW2 = 0;
				LevelAngleCen1 = 0;
//				statusFW3 = 65536;
			}

			if (GroupCntNumDx < (ORIENTATION_CNT_MAX - 1))
			{
				listAngleAz[GroupCntNumDx] = Angle;
				listAngleEl[GroupCntNumDx] = Pitch;
			}
			else
			{
				OrientateDebugPrintf();
				SetSysWorkingMode(eWORK_MODE_DETECT);
				EnterHitModeClearDir();
				LOG_DEBUG("frame cout out 256, GroupCntNumDx=%d\r\n", GroupCntNumDx);
				memset(listAmQ, 0 , ORIENTATION_CNT_MAX*sizeof(float));
				memset(listAmD, 0 , ORIENTATION_CNT_MAX*sizeof(float));
				memset(listAmC, 0 , ORIENTATION_CNT_MAX*sizeof(float));
				memset(listFP, 0 , ORIENTATION_CNT_MAX*sizeof(int));

				memset(listAngleAz, 0 , ORIENTATION_CNT_MAX*sizeof(float));
				memset(listAngleEl, 0 , ORIENTATION_CNT_MAX*sizeof(float));
				statusFW1 = 0;
				statusFW2 = 0;
				LevelAngleCen1 = 0;
//				statusFW3 = 65536;
			}

			ifOrientateProgressing = 1;
//			CurrentFreqItem = pOutsideInterface->pGetDetectFreqItem();
			cenFreq = SYS_GetCenterFreq(CurrentFreqItem);
			tmpF = Freq;
			tmpBW = bw;

			tmpML = 64 + ((tmpF - cenFreq) / dtF);
			tmpMW = ((tmpBW / dtF) / 2);
			tmpDown = (tmpML - tmpMW);
			tmpUp = (tmpML + tmpMW);

			printTmpUp = tmpDown + 2;
			printTmpDown = tmpUp - 2;
			printIndex = flag;
			printCenFreq = cenFreq;

			ret = sfDxNewProcessing(omniMat, orienMat, (int)(tmpDown + 2), (int)(tmpUp - 2), flag, useRow, listAngle, listAmQ, listAmD, listAmC, listFP, GroupCntNumDx);

			LOG_DEBUG("GroupCntNumDx=%d,FW1=%0.0f,FW2=%0.0f,LevelAngleCen1=%0.0f,WarnningStates=%d,tmpDown=%0.0f,tmpUp=%0.0f,flag=%d,cenFreq=%0.0f\r\n"
					, GroupCntNumDx, statusFW1, statusFW2, LevelAngleCen1, OrientateStates, tmpDown, tmpUp, flag, cenFreq);

			if (GroupCntNumDx >= ORIENTATION_CNT_MAX)
			{
				OrientateDebugPrintf();
			}
			if (GroupCntNumDx < ORIENTATION_CNT_MAX)
			{
				GroupCntNumDx++;
			}

		}
		else
		{
			firstFlag = 0;
		}

		if (OrientateStep == STEP_IF_ENTER_DIR)
		{

			ifDirCircFreqFlag = 1;
			SetSysWorkingMode(eWORK_MODE_ELIGIBLE_ENTRY_DIR);

			if (EligibleEntryOrientationFlag == 1)
			{
				Freq = pDroneReportResult->ReportResult[uav_number].DroneResult.freq[0];
				AmpDatOmni = pDroneReportResult->ReportResult[uav_number].DroneResult.amp;
				bw = pDroneReportResult->ReportResult[uav_number].DroneResult.bw;
				flag = pDroneReportResult->ReportResult[uav_number].DroneResult.flag;

				AppropriateFreq = ifDirCircFreq(Freq);
				if (AppropriateFreq)
				{
					SYS_SetWorkMode(SPECTRUM_DX);

					JudgeIfDirMiniSe();
//					ifDirCircPitchflag = ifDirCircPitch(PitchAngle[0], GroupCnt);

//					FreqItem = pOutsideInterface->pGetDirFreqItem(Freq);
					IfChangeAntenna = (ifDirCircFreq(SYS_GetCenterFreq(FreqItem)));

					LOG_DEBUG("IfChangeAntenna %d\r\n", IfChangeAntenna);
//					if (ifDirCircPitchflag != 0)

					axi_write_data(PL_I_FPGA_TEMP3_DIR, 1);
					SetDirDroneInfo();
					SetLevelAngleCen(0x7FFFFFFF);
					SetPitchAngleCen(0x7FFFFFFF);
					OrientateStep = STEP_IF_AZIMUTH_ENOUGH;

					SYS_SetDefinedFreqParamByItem(FreqItem);
					SYS_SetDetectionMode(DEFINED_FREQ_MODE);

					SetOrientateDetMode(MODE_LEVELDETECTE);//进入水平
					SetAntennaMode(IfChangeAntenna);//切换天线
					GroupCnt = 0;
					SetSysWorkingMode(eWORK_MODE_SCANNING_HORI);
					Flag = MODE_LEVELDETECTE;
					ClearFw1AndFw2();
					DirGainCfg(AmpDatOmni);
				}
				else
				{
					SYS_SetWorkMode(SPECTRUM_QX);
					ifDirCircFreqFlag = 0;
					SetSysWorkingMode(eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR);
					LOG_DEBUG("FreqItem %d, get_rx_freq %d\r\n", FreqItem, get_rx_freq(FreqItem));
				}
			}
			else
			{
				SYS_SetWorkMode(SPECTRUM_QX);
				ifDirCircFreqFlag = 0;
				SetSysWorkingMode(eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR);
				LOG_DEBUG("FreqItem %d, get_rx_freq %d\r\n", FreqItem, get_rx_freq(FreqItem));
			}
		}
		else if (OrientateStep == STEP_IF_AZIMUTH_ENOUGH)
		{
			// 修改第一处 20230704
//			sfDxIfAzimuthEnough(&listAngleAz[0], GroupCntNumDx, &statusFW1, &statusFW2);
			if (sfDxIfAzimuthEnough(&listAngleAz[0], GroupCntNumDx, &statusFW1, &statusFW2) == 1)//判断水平扫描范围是否足够（超过360度）
			{

				ret =  sfDxIfLegal(listAngleAz, listAngleEl, listAmQ, listAmD, listAmC, listFP, GroupCntNumDx);
				if (ret == 0)
				{
					OrientateStep = STEP_FAULT_ANGLE_NOT_ENOUGH;
					LOG_DEBUG("sfDxIfLegal() ret =%d\r\n", ret);
				}
				else if (ret == (-1))
				{
					OrientateStep = STEP_FAULT_UNIDENTIFIED_TARGET;
					LOG_DEBUG("sfDxIfLegal() ret =%d\r\n", ret);
				}
				else if (ret == (-2))
				{
					OrientateStep = STEP_FAULT_ANGLE_NOT_ENOUGH;
					LOG_DEBUG("sfDxIfLegal() ret =%d\r\n", ret);
				}
				else
				{
					// 修改第二处 20230704
					int rret = 0;
					LOG_DEBUG("[dd]before sfDxDirCalc +++++++++++++\r\n");
					vTaskDelay(5);
					OrientateDebugPrintf();
					rret = sfDxDirCalc(&listAmD[0], &listAngleAz[0], GroupCntNumDx, &LevelAmpMax1, &LevelAngleCen1, (AppropriateFreq-1));
					LOG_DEBUG("LevelDetecte:  ampMax1 %f, angleCen1:%f\r\n", LevelAmpMax1, LevelAngleCen1);
					LOG_DEBUG("[dd]after sfDxDirCalc -------------\r\n");
					vTaskDelay(5);
					OrientateDebugPrintf();
					LOG_DEBUG("[dd]end ===================\r\n");
					if (DroneLocationMode == 1)
					{
						LevelAngleCen = DroneAngle;
						LOG_DEBUG("LevelDetecte:  ampMax %d, angleCen:%d\r\n", LevelAmpMax, LevelAngleCen);
					}

					if (rret == 0)
					{
						OrientateStep = STEP_FAULT_UNIDENTIFIED_TARGET;
						LOG_DEBUG("sfDxDirCalc rret=%d\r\n", rret);
					}
					else
					{
						SetLevelAngleCen(LevelAngleCen1*100);
						SetPitchAngleCen(0x7FFFFFFF);
						OrientateStep = STEP_IF_AZIMUTH_AIMED;
						GroupCnt = 0;
						SetSysWorkingMode(eWORK_MODE_HORI_AIMING);
						allowExitLevelOrientFlag = 0;
						OrientateDebugPrintf();
					}
				}
			}
		}
		else if (OrientateStep == STEP_IF_AZIMUTH_AIMED)
		{
			// 修改第三处，20230704
//			sfDxIfAimed(float azimuthNow, float elevationNow, float azimuthCen);
//			if (ifAzimuthAimed(Angle, LevelAngleCen) == 1)//判断是否已经水平瞄准成功
			if (sfDxIfAimed((float)Angle, (float)Pitch, LevelAngleCen1)  == 1)//判断是否已经水平瞄准成功且俯仰瞄准成功
			{
//				OrientateStep = STEP_ALREADY_AIMED;
				SetSysWorkingMode(eWORK_MODE_AIM);
				GroupCnt = LEVEL_CNT_MAX;//瞄准后不打，设置超时
				LOG_DEBUG("[dd]end normal(aimed)========\r\n");
				vTaskDelay(5);
				OrientateDebugPrintf();
			}
		}
		else if (OrientateStep == STEP_FAULT_ANGLE_NOT_ENOUGH)
		{
			SetSysWorkingMode(eWORK_MODE_SCANNING_PITCH); // 定向异常旋转角度不足90°

			GroupCnt = LEVEL_CNT_MAX; // 异常等待，设置超时
			LOG_DEBUG("[dd]end default(angle not enougn)==============\r\n");
			vTaskDelay(1);
			OrientateDebugPrintf();
			OrientateStep = STEP_WAIT_TIMEOUT;
		}

		else if (OrientateStep == STEP_FAULT_UNIDENTIFIED_TARGET)
		{
			SetSysWorkingMode(eWORK_MODE_PITCH_AIMING);	// 定向异常 无法识别目标方向
			GroupCnt = LEVEL_CNT_MAX; // 异常等待，设置超时
			LOG_DEBUG("[dd]end default(unidentify targe)===========\r\n");
			vTaskDelay(1);
			OrientateDebugPrintf();
			OrientateStep = STEP_WAIT_TIMEOUT;
		}
	}

	else
	{
		ifDirCircFreqFlag = 0;
		OrientateStates = ORIENTATION_STATUS_NONE;
	}
	LOG_DEBUG("FNo:%d Step:%d Angle:%d Pitch:%d AngleCen:%d PitchCen:%d tA:%d GCnt:%d\r\n",
			FrameNo, OrientateStep, Angle, Pitch , LevelAngleCen, PitchAngleCen, TotalAzimuth, GroupCnt);

	if (Flag == MODE_LEVELDETECTE)
	{
		if (GroupCnt < LEVEL_CNT_MAX)
		{
			LevelDetecte[DATA_ANGLE][GroupCnt] = Angle;
			LevelDetecte[DATA_DIR_AMP][GroupCnt] = Amp;
			LevelDetecte[DATA_OMNI_AMP][GroupCnt] = AmpDatOmni;
			GroupCnt++;
		}
		else
		{
			SetSysWorkingMode(eWORK_MODE_DETECT);
			EnterHitModeClearDir();
		}
	}
	else if (Flag == MODE_PITCHANGLE)
	{
		if (GroupCnt < PITCH_CNT_MAX)
		{
			PitchAngle[DATA_ANGLE][GroupCnt] = Pitch;
			PitchAngle[DATA_DIR_AMP][GroupCnt] = Amp;
			PitchAngle[DATA_OMNI_AMP][GroupCnt] = AmpDatOmni;
			GroupCnt++;
		}
		else
		{
			SetSysWorkingMode(eWORK_MODE_DETECT);
			EnterHitModeClearDir();
		}
	}
	else if (Flag == MODE_IF_ENTER_DIR)
	{
		if (GroupCnt < PITCH_CNT_MAX)
		{
			PitchAngle[DATA_ANGLE][GroupCnt] = Pitch;
			GroupCnt++;
		}
		else
		{
			GroupCnt = 0;
		}
	}
	else
	{
		GroupCnt = 0;
	}

	if (ModeChangeFlag == 1)
	{
		ModeChangeFlag = 0;
		if (Flag == MODE_LEVELDETECTE)
		{
			sfDxDirCalc(&listAmD[0], &listAngleAz[0], GroupCntNumDx, &LevelAmpMax1, &LevelAngleCen1, (AppropriateFreq-1));
			LOG_DEBUG("LevelDetecte:  ampMax %d, angleCen:%d\r\n", LevelAmpMax, LevelAngleCen);
		}
		else if (Flag == MODE_PITCHANGLE)
		{
			pitchCalcMax(PitchAngle[DATA_DIR_AMP], PitchAngle[DATA_ANGLE], GroupCnt, &PitchAmpMax, &PitchAngleCen);
			LOG_DEBUG("PitchAngle:  ampMax %d, angleCen:%d\r\n", PitchAmpMax, PitchAngleCen);
		}
		GroupCnt = 0;
	}
	Flag = GetOrientateDetMode();
    if (OrientationMutex)
    {
    	xSemaphoreGive(OrientationMutex);
    }
}

/* CenterFreq: MHz */
void ORI_OrientateProcessPost(uint32_t TotalFrameNo, int32_t Amp, uint8_t FreqItem)
{
	ORI_OrientateDetParam_t lOrientateDetParam;

	lOrientateDetParam.FrameNo = TotalFrameNo;

	lOrientateDetParam.Amp = Amp;
	lOrientateDetParam.FreqItem = FreqItem;
	lOrientateDetParam.CenterFreq = 0; //CenterFreq;

	if (xOrientateDetParamQueue != NULL)
	{
		xQueueSend(xOrientateDetParamQueue, &lOrientateDetParam, 10);
	}
}

static int32_t ORI_OrientateProcessPend(ORI_OrientateDetParam_t *pOrientateDetParam, uint32_t timeout)
{
	int32_t result = -1;

	if (xOrientateDetParamQueue != NULL)
	{
		if (xQueueReceive(xOrientateDetParamQueue, (void *)pOrientateDetParam, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void ORI_OrientateProcessTask(void *p_arg)
{
	uint32_t framenum;
	uint32_t WaitTime = 2000;
	int32_t CycleNo = 0;

	float roll;
	float pitch;
	float yaw;
	float amp = 0;

	TickType_t xLastWakeTime;

	uint8_t DetFreqItemCurr = 0xFF;

	ORI_OrientateDetParam_t lOrientateDetParam;

//	DET_OutsideInterface_t *pOutsideInterface = NULL;
//	pOutsideInterface = DET_GetOutsideInterface();

	xLastWakeTime = xTaskGetTickCount ();
	while (1)
	{
		if (ORI_OrientateProcessPend(&lOrientateDetParam, WaitTime) == 0)
		{

//				framenum++;
			framenum = lOrientateDetParam.FrameNo;
			DetFreqItemCurr = lOrientateDetParam.FreqItem;
			amp = lOrientateDetParam.Amp;

			GetAngleInfo(&roll, &pitch, &yaw);

			/* for test */
//			CycleNo = (framenum%72) - 72;
//			yaw = CycleNo * 5;
//			pitch = (framenum*84) + 6;

			xLastWakeTime = xTaskGetTickCount ();

//			OrientateProcess(framenum, yaw*100, pitch*100, amp, amp, DetFreqItemCurr);
		}
		else
		{

		}
	}
}

static void TimerFuncKeyCallback(void)
{
	EnterFuncKeyFlag = 0;
}

int32_t ORI_Init()
{
	int32_t retVal = -1;

	xOrientateDetParamQueue = xQueueCreate(5, sizeof(ORI_OrientateDetParam_t));
	if (xOrientateDetParamQueue == NULL)
	{
		while(1);
	}

//	if (xTaskCreate(ORI_OrientateProcessTask, "ORI_OrientateProcessTask", TASK_STACK_SIZE_DETECTPROCESS, NULL, TASK_PRI_ORIENTATEPPROCESS, NULL) == 1)
//	{
//		retVal = 0;
//	}
//	else
//	{
//		SetAeagStatus(AEAG_SYSTEM_ERROR, ERRCODE_FUNC_CREATE_TASK_ORIENTATION);
//	}
//
//	if (retVal == 0)
	{
		TimerFuncKeyHandle = xTimerCreate((const char *)"Timer",
										 (TickType_t)500,
										 (UBaseType_t)pdFALSE,
										 (void *)1,
										 (TimerCallbackFunction_t)TimerFuncKeyCallback);
	}

	OrientationMutex = xSemaphoreCreateMutex();

	if (TimerFuncKeyHandle == NULL)
	{
		retVal = -1;
	}

	return retVal;
}
