/* Standard Include Files. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "droneID_utils.h"


/* global data declaration */
uint8_t uAlgorithmRunFlag = 0;
int32_t g_freqOffsetHz = 0;
uint64_t g_droneIDSigFreqHz = 0;
int32_t g_algDebugPrintMode = PRINT_DRONEID_RIGHT_AND_WRONG;
int32_t g_droneIDOutputSnMode = ALL;
int32_t g_droneIDOutputListMode = DRONEID_OUTPUT_LIST_PREDICT;
int32_t g_droneIDRemoveThre = DEFAULT_DRONEID_REMOVE_THRE;
int32_t g_droneIDRemoveThreLostDur = DEFAULT_DRONEID_REMOVE_THRE_LOST_DUR;
int32_t g_droneIDTurboDecIterNum = DEFAULT_TURBO_ITERATIONS;
uint32_t g_droneIDProcFrameCnt = 0;
uint32_t g_droneIDCrcErrorCnt = 0;


void SetAlgorithmRunFlag(uint8_t value)
{
	uAlgorithmRunFlag = value;
}

uint8_t GetAlgorithmRunFlag(void)
{
	return uAlgorithmRunFlag;
}


/* set frequence offset, unit Hz */
void SetFreqOffsetHz(int32_t freqOffset)
{
	g_freqOffsetHz = freqOffset;
}

/* get frequence offset, unit Hz */
int32_t GetFreqOffsetHz(void)
{
	return g_freqOffsetHz;
}


/* set droneid signal frequence, unit Hz */
void SetDroneIDSigFreqHz(uint64_t freq)
{
	g_droneIDSigFreqHz = freq;
}

/* get droneid signal frequence, unit Hz */
uint64_t GetDroneIDSigFreqHz(void)
{
	return g_droneIDSigFreqHz;
}


/* set algorithm debug print mode */
void SetAlgDebugPrintMode(int32_t printMode)
{
	g_algDebugPrintMode = printMode;
}

/* get algorithm debug print mode */
int32_t GetAlgDebugPrintMode(void)
{
	return g_algDebugPrintMode;
}


/* set droneID output sn mode */
void SetDroneIDOutputSnMode(int32_t snMode)
{
	g_droneIDOutputSnMode = snMode;
}

/* get droneID output sn mode */
int32_t GetDroneIDOutputSnMode(void)
{
	return g_droneIDOutputSnMode;
}


/* set droneID output list mode */
void SetDroneIDOutputListMode(int32_t listMode)
{
	g_droneIDOutputListMode = listMode;
}

/* get droneID output list mode */
int32_t GetDroneIDOutputListMode(void)
{
	return g_droneIDOutputListMode;
}


/* set droneID remove threshold value */
void SetDroneIDRemoveThre(int32_t threValue)
{
	g_droneIDRemoveThre = threValue;
}

/* get droneID remove threshold value */
int32_t GetDroneIDRemoveThre(void)
{
	return g_droneIDRemoveThre;
}

/* set droneID remove threshold value, lost duration */
void SetDroneIDRemoveThreLostDur(int32_t threValue)
{
	g_droneIDRemoveThreLostDur = threValue;
}

/* get droneID remove threshold value, lost duration */
int32_t GetDroneIDRemoveThreLostDur(void)
{
	return g_droneIDRemoveThreLostDur;
}

/* set droneID turbo decode iteration num */
void SetDroneIDTurboDecIterNum(int32_t iterNum)
{
	g_droneIDTurboDecIterNum = iterNum;
}

/* get turbo decode iteration num */
int32_t GetDroneIDTurboDecIterNum(void)
{
	return g_droneIDTurboDecIterNum;
}

/* reset droneID process frame count to 0 */
void ResetDroneIDProcFrameCnt(void)
{
	g_droneIDProcFrameCnt = 0;
}

/* get droneID process frame count */
uint32_t GetDroneIDProcFrameCnt(void)
{
	return g_droneIDProcFrameCnt;
}

/* reset crc check error count to 0 */
void ResetDroneIDCrcErrorCnt(void)
{
	g_droneIDCrcErrorCnt = 0;
}

/* get droneID crc check error count */
uint32_t GetDroneIDCrcErrorCnt(void)
{
	return g_droneIDCrcErrorCnt;
}


/* int32 type complex data inner product r = x*y' */
Cplxint32_t InnerProductInt32(Cplxint32_t *x, Cplxint32_t *y, uint32_t length)
{
	int32_t ii = 0;
	int32_t real = 0, imag = 0;
	Cplxint32_t result = { 0 };

	for (ii = 0; ii < length; ii++)
	{
		real +=   (x[ii].re*y[ii].re  + x[ii].im*y[ii].im);
		imag += (-(x[ii].re*y[ii].im) + x[ii].im*y[ii].re);
	}

	result.re = real;
	result.im = imag;

	return result;
}


/* float type complex data inner product r = x*y' */
Cplxf_t InnerProductFloat(Cplxf_t *x, Cplxf_t *y, uint32_t length)
{
	int32_t ii = 0;
	float real = 0.0f, imag = 0.0f;
	Cplxf_t result = { 0.0f };

	for (ii = 0; ii < length; ii++)
	{
		real +=   (x[ii].re*y[ii].re  + x[ii].im*y[ii].im);
		imag += (-(x[ii].re*y[ii].im) + x[ii].im*y[ii].re);
	}

	result.re = real;
	result.im = imag;

	return result;
}


/**
 * calculate the mean value of one input Cplxf_t vector.
 *
 * @param *vector, Cplxf_t vector
 * @param length, vector length
 * @return mean value of one input Cplxf_t vector.
 */
Cplxf_t CalcCplxfVectorMean(Cplxf_t *vector, uint32_t length)
{
	uint32_t i = 0;
	Cplxf_t sum = { 0.0f };
	Cplxf_t ret = { 0.0f };

	for (i = 0; i < length; i++)
	{
		sum.re += vector[i].re;
		sum.im += vector[i].im;
	}
	ret.re = sum.re / length;
	ret.im = sum.im / length;

	return ret;
}


float CalcDegToRad(float degree)
{
    return degree * M_PI / 180.0;
}

float GetDistance(float lat1, float lon1, float alt1, float lat2, float lon2, float alt2)
{
    // 将经纬度转换为弧度
    lat1 = CalcDegToRad(lat1);
    lon1 = CalcDegToRad(lon1);
    lat2 = CalcDegToRad(lat2);
    lon2 = CalcDegToRad(lon2);

    // 计算经纬度差
    float delta_lat = lat2 - lat1;
    float delta_lon = lon2 - lon1;

    // 使用Haversine公式计算两点之间的距离
    float a = sinf(delta_lat/2) * sinf(delta_lat/2) + cosf(lat1) * cosf(lat2) * sinf(delta_lon/2) * sinf(delta_lon/2);
    float c = 2 * atan2f(sqrtf(a), sqrtf(1-a));
    float d = EARTH_RADIUS * c;

    // 计算高度差
    float delta_alt = alt2 - alt1;

    // 计算两点之间的直线距离
    float distance = sqrtf(d*d + delta_alt*delta_alt);

    return distance;
}


/**
 * calculate the variance value of one input Cplxf_t vector.
 *
 * @param *vector, Cplxf_t vector
 * @param length, vector length
 * @return variance value of one input Cplxf_t vector.
 */
float CalcCplxfVectorVar(Cplxf_t *vector, uint32_t length)
{
	uint32_t i = 0;
	float mean_real = 0.0;
	float mean_imag = 0.0;
	float variance_real = 0.0;
	float variance_imag = 0.0;

	// 计算实部和虚部的平均值
	for (i = 0; i < length; i++)
	{
		mean_real += vector[i].re;
		mean_imag += vector[i].im;
	}
	mean_real /= length;
	mean_imag /= length;

	// 计算实部和虚部的方差
	for (i = 0; i < length; i++)
	{
		variance_real += powf(vector[i].re - mean_real, 2);
		variance_imag += powf(vector[i].im - mean_imag, 2);
	}
	variance_real /= length;
	variance_imag /= length;

	// 返回复数向量的方差
	return variance_real + variance_imag;
}


