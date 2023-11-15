/*
 * detection_target.c
 *
 *  Created on: 2023骞�4鏈�20鏃�
 *      Author: A21001
 */

#include <stdint.h>
#include "detection_target.h"
#include "../alg/det_alg/droneSniffer.h"
#include "../../srv/log/log.h"
#include "../../srv/heartbeat_packet/heartbeat_packet.h"

static DroneReportResult_t DroneInfoSlideWin = {0};
static ReportResult_t NextDroneResult;

static SemaphoreHandle_t  xSpectrumSeamphore;

void SpectrumSeamphoreCreate(void)
{
	if (xSpectrumSeamphore == NULL)
	{
		xSpectrumSeamphore = xSemaphoreCreateMutex();
	}

}

void SpectrumSeamphoreTake(void)
{
	if (xSpectrumSeamphore != NULL)
	{
		xSemaphoreTake(xSpectrumSeamphore, 10000);
	}

}

void SpectrumSeamphoreGive(void)
{
	if (xSpectrumSeamphore != NULL)
	{
		xSemaphoreGive(xSpectrumSeamphore);
	}
}

uint8_t PrintfDroneInfo = 0;
void SetPrintfDroneInfo(uint8_t value)
{
	PrintfDroneInfo = value;
}

void ClearDroneInfoSlideWin(void)
{
	DroneInfoSlideWin.DroneCnt = 0;
}

void SetDirDroneInfo(void)
{
	DroneInfoSlideWin.DirDroneInfo = DroneInfoSlideWin.ReportResult[0];
}

int32_t SYS_GetDroneTargetInfo(DroneReportResult_t *pDroneReportResult_t)
{
	int32_t Result = 0;
	uint32_t len = 0;

	if (pDroneReportResult_t == NULL)
	{
		Result = 1;
	}
	else
	{
		len = sizeof(DroneReportResult_t);
		memcpy(pDroneReportResult_t, &DroneInfoSlideWin, sizeof(DroneReportResult_t));
	}

	return Result;
}

uint8_t SYS_GetDroneTargetCnt(void)
{
	return DroneInfoSlideWin.DroneCnt;
}

DroneReportResult_t* SYS_GetDroneInfo(void)
{
	return &DroneInfoSlideWin;
}

/* FirstTagetUpdate[in] : when first list target not been detected,
 * 0: not update the first list target info
 * 1: update the first list target info
 */
static int32_t SYS_DroneWarnSildeWinProcess(struct droneResult *droneInfo, int nDrone, DroneReportResult_t *pDroneInfoSlideWin, uint8_t FreqItem, uint32_t FirstTagetUpdate)
{
	static uint8_t WaringFlag = 0;
	uint8_t NewDrone = 1;
	uint8_t DroneCnt = pDroneInfoSlideWin->DroneCnt;
	uint8_t LastTimeDroneCnt2 = DroneCnt;

	for (uint8_t j = 0; j < SlideWinCntMAX; j++)
	{
		if (FirstTagetUpdate == 0)
		{
			if (j == 0)
			{
				continue;
			}
		}
		if (pDroneInfoSlideWin->ReportResult[j].RemainingTimes > 0)
		{
			pDroneInfoSlideWin->ReportResult[j].RemainingTimes--;
			if (pDroneInfoSlideWin->ReportResult[j].RemainingTimes == 0)
			{
				if (pDroneInfoSlideWin->DroneCnt > 0)
				{
					pDroneInfoSlideWin->DroneCnt--;
				}
			}
		}
	}

	for (uint8_t i = 0; i < nDrone; i++)
	{
		NewDrone = 1;
		for (uint8_t j = 0; j < DroneCnt; j++)
		{
			if (PrintfDroneInfo == 1)
			{
				LOG_DEBUG("memcmp:%d, %d ,%d\r\n", (memcmp(droneInfo[i].name, pDroneInfoSlideWin->ReportResult[j].DroneResult.name, strlen(droneInfo[i].name)) == 0), pDroneInfoSlideWin->ReportResult[j].DroneResult.freq[0],
						pDroneInfoSlideWin->ReportResult[j].RemainingTimes, pDroneInfoSlideWin->DroneCnt > 0);
			}

			if ((memcmp(droneInfo[i].name, pDroneInfoSlideWin->ReportResult[j].DroneResult.name, strlen(droneInfo[i].name)) == 0)
					&& (pDroneInfoSlideWin->ReportResult[j].RemainingTimes > 0) && (pDroneInfoSlideWin->DroneCnt > 0))
			{
				pDroneInfoSlideWin->ReportResult[j].DroneResult = droneInfo[i];
				pDroneInfoSlideWin->ReportResult[j].RemainingTimes = TimeoutCntMAX;
				pDroneInfoSlideWin->ReportResult[j].DetFreqItem = FreqItem;
				LOG_DEBUG("DetFreqItem:%d, freq:%0.0f\r\n", FreqItem, pDroneInfoSlideWin->ReportResult[j].DroneResult.freq[0]);
				NewDrone = 0;
			}
			else
			{

			}
		}
		if (NewDrone == 1)
		{
			pDroneInfoSlideWin->ReportResult[LastTimeDroneCnt2].DroneResult = droneInfo[i];
			pDroneInfoSlideWin->ReportResult[LastTimeDroneCnt2].RemainingTimes = TimeoutCntMAX;
			pDroneInfoSlideWin->ReportResult[LastTimeDroneCnt2].DetFreqItem = FreqItem;
			LOG_DEBUG("DetFreqItem2:%d, freq:%0.0f\r\n", FreqItem, pDroneInfoSlideWin->ReportResult[LastTimeDroneCnt2].DroneResult.freq[0]);
			LastTimeDroneCnt2++;
			pDroneInfoSlideWin->DroneCnt++;
			if (pDroneInfoSlideWin->DroneCnt >= SlideWinCntMAX)
			{
				pDroneInfoSlideWin->DroneCnt = SlideWinCntMAX - 1;
			}
			if (LastTimeDroneCnt2 >= SlideWinCntMAX)
			{
				LastTimeDroneCnt2 = SlideWinCntMAX - 1;
			}
		}
	}

	if (pDroneInfoSlideWin->DroneCnt > 0)
	{
		uint8_t index = 0;
		/************缁撴瀯浣撹皟鏁存帓鍒楋紝涓棿娌℃湁绌虹殑*****************/
		for (uint8_t j = 0; j < SlideWinCntMAX; j++)
		{
			if (pDroneInfoSlideWin->ReportResult[j].RemainingTimes == 0)
			{
				continue;
			}
			else
			{
				if (index < j)
				{
					pDroneInfoSlideWin->ReportResult[index] = pDroneInfoSlideWin->ReportResult[j];
					pDroneInfoSlideWin->ReportResult[j].RemainingTimes = 0;
				}
				index ++;
			}
		}

		if (FirstTagetUpdate == 0)
		{
		}
		else
		{
			/*************缁撴瀯浣撴寜amp鏈�澶у�兼帓鍒�****************/
			for (int i = pDroneInfoSlideWin->DroneCnt - 2; i >= 0 ; i--)
			{
				if (pDroneInfoSlideWin->ReportResult[i].DroneResult.amp < pDroneInfoSlideWin->ReportResult[i + 1].DroneResult.amp)
				{
					NextDroneResult = pDroneInfoSlideWin->ReportResult[i];
					pDroneInfoSlideWin->ReportResult[i] = pDroneInfoSlideWin->ReportResult[i + 1];
					pDroneInfoSlideWin->ReportResult[i + 1] = NextDroneResult;
				}
			}
		}

		SetLastDetFreqItem(pDroneInfoSlideWin->ReportResult[0].DetFreqItem);
	}
	else
	{

	}
//	LOG_DEBUG("name:%s, f:%0.0f\r\n", pDroneInfoSlideWin->ReportResult[0].DroneResult.name, pDroneInfoSlideWin->ReportResult[0].DroneResult.freq[0]);
	LOG_DEBUG("DroneCnt:%d 0Times %d fItem:%d\r\n",
			pDroneInfoSlideWin->DroneCnt, pDroneInfoSlideWin->ReportResult[0].RemainingTimes, pDroneInfoSlideWin->ReportResult[0].DetFreqItem);
	return pDroneInfoSlideWin->DroneCnt;
}

/* FirstTagetUpdate[in] : when first list target not been detected,
 * 0: not update the first list target info
 * 1: update the first list target info
 */
int32_t SYS_SetDetectionResult(struct droneResult *pDroneResult, int nDrone, uint8_t FreqItem,  uint32_t FirstTagetUpdate)
{
	int32_t Result = 0;

	if (pDroneResult == NULL)
	{
		Result = 1;
	}
	else
	{
		SpectrumSeamphoreTake();
		Result = SYS_DroneWarnSildeWinProcess(pDroneResult, nDrone, &DroneInfoSlideWin, FreqItem, FirstTagetUpdate);
		SpectrumSeamphoreGive();
		if (Result != 0)
		{
			Result = 2;
		}

	}

	return Result;
}
