/*
 * detection_param.h
 *
 *  Created on: 2023年1月29日
 *      Author: A21001
 */
#ifndef SRC_APP_SYS_STATUS_DATA_DETECTION_PARAM_H_
#define SRC_APP_SYS_STATUS_DATA_DETECTION_PARAM_H_

#include <stdint.h>

#include "../alg/det_alg/droneSniffer.h"

typedef enum SYS_WorkMode
{
	DRONEID_MODE = 1,
	SPECTRUM_QX,
	SPECTRUM_DX,
} SYS_WorkMode_t;

typedef enum SYS_DetectionMode
{
	DEFINED_FREQ_MODE = 0,
	DEFINED_SWEEP_FREQ_MODE,
	AUTO_SWEEP_FREQ_MODE
} SYS_DetectionMode_t;

typedef struct SYS_SampleParam
{
	/* Unit: KHz */
	uint32_t SampleFreq;
	/* Unit: KHz */
	uint32_t Band;
	/* Unit: dB */
	uint32_t Gain;
} SYS_SampleParam_t;

typedef struct SYS_AutoSweepParam
{
	/* Unit: MHz */
	uint32_t CurCenterFreq;

	/* Unit: MHz */
	uint32_t StartCenterFreq;
	/* Unit: MHz */
	uint32_t EndCenterFreq;
	/* Unit: MHz */
	uint32_t CenterFreqInc;
} SYS_AutoSweepParam_t;

typedef struct SYS_DefinedSweepParam
{
	uint32_t DefinedFreqValideNo;
	uint32_t DefinedFreqTotalNo;

	uint32_t CurFreqIndex;
	/* Unit: MHz */
	float DefinedFreq[40];
} SYS_DefinedSweepParam_t;

typedef struct SYS_DefinedFreqParam
{
	uint8_t DefinedFreqIndex;
	/* Unit: MHz */
	uint32_t DefinedFreq;
} SYS_DefinedFreqParam_t;

typedef struct SYS_Detection_Param
{
	SYS_DetectionMode_t DetectionMode;

	/* Unit: MHz */
	uint32_t CenterFreq;
	/* CenterFreq index */
	uint8_t CenterFreqItem;

	/* parameter for different Detection Mode */
	SYS_DefinedFreqParam_t DefinedFreqParam;
	SYS_AutoSweepParam_t AutoSweepParam;
	SYS_DefinedSweepParam_t DefinedSweepParam;

	SYS_SampleParam_t SampleParam;
}SYS_DetectionParam_t ;

//typedef struct SYS_DroneResult
//{
//    int ID;             //
//    char name[50];      //
//    char flag;          //
//    float freq[50];     //
//    float burstTime[50];//
//    float width[50];    //
//    float bw;           //
//    float amp;          //
//    float range;        //
//} SYS_DroneResult_t;

void SYS_SetWorkMode( SYS_WorkMode_t mode );
SYS_WorkMode_t SYS_GetWorkMode();
void SYS_SwitchWorkMode( SYS_WorkMode_t mode );

void SYS_SetDetectionMode(SYS_DetectionMode_t SetDetecionMode);
SYS_DetectionMode_t SYS_GetDetectionMode(void);

void SYS_SetDetectionAlgStatus(uint32_t DetRunCoreID, uint32_t SetDetecionAlgStaus);
uint32_t SYS_GetDetectionAlgStatus(uint32_t DetRunCoreID);

int32_t SYS_SetDefinedFreqParamByItem(uint8_t FreqItem);
uint8_t SYS_GetDetectFreqItem(void);
uint32_t SYS_GetDetectFreq(void);

int32_t SYS_SetDetectionParam(const SYS_DetectionParam_t *pDetParam);
int32_t SYS_GetDetectionParam(SYS_DetectionParam_t *pDetParam);
float SYS_GetCenterFreq(uint32_t CurFreqIndex);
void SYS_RefreshDetectionParam();

#endif /* SRC_APP_SYS_STATUS_DATA_DETECTION_PARAM_H_ */
