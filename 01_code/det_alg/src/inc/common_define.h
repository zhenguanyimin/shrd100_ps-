/*
 * common_define.h
 *
 *  Created on: 2022-09-14
 *      Author: A19199
 */
#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H

#include <stdio.h>

/******************************************************************************
无人机编号定义uint32_t uNumber;
high byte                    low byte
0x   FF     FF     FF      FF
	品牌           系列      型号         序号（多个无人机）
	 01     01     01      01       Autel EVO_II
	 01     01     02      01       Autel EVO II RTK
	 01     01     03      01       Autel EVO II DUAL 640T
	 01     02     01      01       Autel EVO NANO
	 01     03     01      01       Autel EVO LITE
 *******************************************************************************/

#define MAX_SIGNAL_NUM 4096

typedef struct sPL_SignalHandlingCfg_t
{
	uint32_t uLocalOscFreq;//本振频段号 KHZ
	uint32_t uSlidWinPoints;//滑窗点数
	uint32_t uFFT_Points;//FFT点数
	uint32_t uDwellTime;//驻留时间
	int16_t iAmpThreshold;//幅度门限
	uint16_t uSignalNum;//信号编号
	int16_t iAmplitude;//信号幅度
	uint32_t uInterceptionTime;//截获时间
	uint32_t uBandwidth;//信号带宽
	uint32_t uDuration;//持续时间
} sPL_SignalHandlingCfg;

typedef struct sSignalDescription_t
{
	uint16_t uSignalNum;		// Signal number
	uint16_t uAmplitude;		// Signal amplitude
	uint32_t uinterceptionTime; // interception time
	uint32_t uFreq;				// signal frequency
	uint32_t uBandwidth;		// Signal bandwidth
	uint32_t uDuration;			// duration 持续时间
	uint32_t uOccurNum;			// Frequency of signal occurrence (times/s)
} sSignalDescription;

typedef struct sSignalDescriptionList_t
{
	uint32_t uTotalNum;
	sSignalDescription sSignalDesc[MAX_SIGNAL_NUM];
} sSignalDescriptionList;

typedef struct outputUAV_t
{
	uint32_t uNumber;		/*drone number*/
	uint32_t uFreq;			/*kHz  */
	uint16_t uDistance;		/* m */
	uint16_t uDangerLevels; /*0:no danger  1~6:The lower the number, the higher the danger level*/
} outputUAV;

typedef struct outputSuspectUAV_t
{
	uint32_t uNumber;	   /*drone number 编号*/
	uint32_t uFreq;		   /*kHz  */
	uint16_t uDistance;	   /* m */
	uint16_t uThreatLevel; /*0:no danger  1~6:The lower the number, the higher the danger level*/
} outputSuspectUAV;

typedef struct output_UAV_List
{
	uint8_t uUAV_Num; /*Number of drones个数*/
	outputUAV algOutputUAV[MAX_SIGNAL_NUM];
	uint8_t uSuspectUAV_Num; /*Suspected number of drones*/
	outputSuspectUAV algOutputSuspectUAV[6];
} output_UAV_List;

typedef struct sRadioFeature_t
{
	uint32_t uFreq;
	uint32_t uBandwidth;
	uint32_t uDuration;
	uint32_t uOccurNum;
	int16_t uPower;
	uint16_t uNextTimeInterval; /*us*/
	uint8_t uThreatLevel;		/*0*/
} sRadioFeature;

typedef struct sUAVFeatureLib_t
{
	uint32_t uNumber; /*drone number 编号*/
	char name[20];
	uint16_t uFlightCtlFeatCnt;
	uint16_t uImageFeatCnt;
	sRadioFeature sflightCtlFeat[20];
	sRadioFeature sImageFeat[10];
} sUAVFeatureLib;

#endif /* COMMON_DEFINE_H */
