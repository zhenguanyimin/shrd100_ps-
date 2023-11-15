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
鏃犱汉鏈虹紪鍙峰畾涔塽int32_t uNumber;
high byte                    low byte
0x   FF     FF     FF      FF
	鍝佺墝           绯诲垪      鍨嬪彿         搴忓彿锛堝涓棤浜烘満锛�
	 01     01     01      01       Autel EVO_II
	 01     01     02      01       Autel EVO II RTK
	 01     01     03      01       Autel EVO II DUAL 640T
	 01     02     01      01       Autel EVO NANO
	 01     03     01      01       Autel EVO LITE
 *******************************************************************************/

#define MAX_SIGNAL_NUM 4096

typedef struct sPL_SignalHandlingCfg_t
{
	float uLocalOscFreq;//鏈尟棰戞鍙� KHZ
	uint32_t uSlidWinPoints;//婊戠獥鐐规暟
	uint32_t uFFT_Points;//FFT鐐规暟
	uint32_t uDwellTime;//椹荤暀鏃堕棿
	int16_t iAmpThreshold;//骞呭害闂ㄩ檺
	uint16_t uSignalNum;//淇″彿缂栧彿
	int16_t iAmplitude;//淇″彿骞呭害
	uint32_t uInterceptionTime;//鎴幏鏃堕棿
	uint32_t uBandwidth;//淇″彿甯﹀
	uint32_t uDuration;//鎸佺画鏃堕棿
} sPL_SignalHandlingCfg;

typedef struct sSignalDescription_t
{
	uint16_t uSignalNum;		// Signal number
	int16_t uAmplitude;		// Signal amplitude
	uint32_t uinterceptionTime; // interception time
	uint32_t uFreq;				// signal frequency
	uint32_t uBandwidth;		// Signal bandwidth
	uint32_t uDuration;			// duration 鎸佺画鏃堕棿
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
	uint32_t uNumber;	   /*drone number 缂栧彿*/
	uint32_t uFreq;		   /*kHz  */
	uint16_t uDistance;	   /* m */
	uint16_t uThreatLevel; /*0:no danger  1~6:The lower the number, the higher the danger level*/
} outputSuspectUAV;

typedef struct output_UAV_List
{
	uint8_t uUAV_Num; /*Number of drones涓暟*/
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
	uint32_t uNumber; /*drone number 缂栧彿*/
	char name[20];
	uint16_t uFlightCtlFeatCnt;
	uint16_t uImageFeatCnt;
	sRadioFeature sflightCtlFeat[20];
	sRadioFeature sImageFeat[10];
} sUAVFeatureLib;

#endif /* COMMON_DEFINE_H */
