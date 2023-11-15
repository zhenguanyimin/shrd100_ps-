/*
 * orientation.h
 *
 *  Created on: 2023-03-02
 *      Author: A19199
 */
#ifndef ORIENTATION_H
#define ORIENTATION_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../hal/output/output.h"
#include "../../app/sys_status_data/detection_target.h"
enum ORIENTATION_MODE
{
	MODE_NONE = 0x00,
	MODE_IF_ENTER_DIR,
	MODE_LEVELDETECTE,
	MODE_PITCHANGLE,
};

enum ANTENNAMODE_MODE
{
	ANTENNA_ALL = 0x00,
	ANTENNA_2_4 = 0x01,
	ANTENNA_5_8 = 0x02,
	DISABLE_DIR_ANT = 0x03,
};

enum ORIENTATION_STEP
{
	STEP_IF_ENTER_DIR,
	STEP_IF_AZIMUTH_ENOUGH,
	STEP_IF_AZIMUTH_AIMED,
//	STEP_IF_PITCH_ENOUGH,
//	STEP_IF_PITCH_AIMED,
	STEP_FAULT_ANGLE_NOT_ENOUGH,
	STEP_FAULT_UNIDENTIFIED_TARGET,
	STEP_TRACKING,

	STEP_WAIT_TIMEOUT,
};
typedef enum ORIENTATION_STATUS
{
	ORIENTATION_STATUS_NONE,
	ORIENTATION_STATUS_WITIN_THE_RANGE_LEVEL_1,
	ORIENTATION_STATUS_WITIN_THE_RANGE_LEVEL_2,
	ORIENTATION_STATUS_AIM
} ORI_orientationStatus_e;
enum WORK_MODE
{
	eWORK_MODE_STANDBY = 0x00,
	eWORK_MODE_DETECT = 0x01,
	eWORK_MODE_HIT_REMOTE_IMG = 0x02,
	eWORK_MODE_HIT_GNSS = 0x03,
	eWORK_MODE_HIT_ALL = 0x04,
	eWORK_MODE_SCANNING_HORI = 0x05,//正在水平扫描中
	eWORK_MODE_HORI_AIMING = 0x06,//请水平瞄准
	eWORK_MODE_SCANNING_PITCH = 0x07,//正在俯仰扫描中
	eWORK_MODE_PITCH_AIMING = 0x08,//请俯仰瞄准
	eWORK_MODE_AIM = 0x09,//已经瞄准
	eWORK_MODE_ELIGIBLE_ENTRY_DIR = 0x0A,//允许进入定向
	eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR = 0x0B,//频率不在范围不允许进入定向
};

typedef struct ORI_OrientateDetParam
{
	uint32_t FrameNo;

	int32_t Amp;
	uint8_t FreqItem;
	/* CenterFreq: MHz */
	uint32_t CenterFreq;
}ORI_OrientateDetParam_t;

enum DATA_TYPE
{
	DATA_ANGLE = 0x00,
	DATA_DIR_AMP,
	DATA_OMNI_AMP,
};
void KeyScanf(void);
void OrientateProcess(uint32_t FrameNo, float Angle, float Pitch, int32_t Amp, uint8_t FreqItem, unsigned short ***omniMat, unsigned short ***orienMat,
		uint8_t CurrentFreqItem, float fs, DroneReportResult_t *pDroneReportResult);
//void OrientateProcessV2(uint32_t FrameNo, int32_t Angle, int32_t Pitch, int32_t Amp, int32_t AmpDatOmni, uint8_t FreqItem, uint32_t Freq, unsigned short ***omniMat, unsigned short ***orienMat,
//		float bw, uint8_t CurrentFreqItem, float fs, char flag);
void OrientateProcessV2(uint32_t FrameNo, float Angle, float Pitch, int32_t Amp, uint8_t FreqItem, unsigned short ***omniMat, unsigned short ***orienMat,
		uint8_t CurrentFreqItem, float fs, DroneReportResult_t *pDroneReportResult);

void SetOrientateDetMode(uint8_t mode);

void SetManualOrientateMode(uint8_t mode, int32_t Angle, int32_t Pitch, int32_t Amp, uint8_t FreqItem);

void EnterHitModeClearDir(void);

void SetLevelAngleCen(int32_t data);

int32_t GetLevelAngleCen(void);

void SetPitchAngleCen(int32_t data);

int32_t GetPitchAngleCen(void);

void ORI_OrientateProcessPost(uint32_t TotalFrameNo, int32_t Amp, uint8_t FreqItem);
int32_t ORI_Init();

uint8_t GetEligibleEntryOrientationFlag();

void SetEnterManualStrategyMode(uint8_t value);
uint8_t GetEnterManualStrategyMode(void);

void SetEligibleEntryOrientationFlag(uint8_t flag);
void SetUavNumber(uint8_t flag);

uint8_t GetJudgeIfDirMiniSe(void);

ORI_orientationStatus_e GetOrientationStatus(void);
void SetOrientationStatus(ORI_orientationStatus_e status);

uint16_t GetAzimuthStartAngle(void);

uint16_t GetAzimuthEndAngle(void);
uint8_t GetUavNumber();
void get_dir_para( uint16_t* qQxPower ,uint16_t* qDxPower ,uint32_t* qdDxAngle  );

#endif /* ORIENTATION_H */
