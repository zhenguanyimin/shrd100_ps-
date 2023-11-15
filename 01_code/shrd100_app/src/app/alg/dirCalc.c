#include <stdlib.h>

#include "dirCalc.h"
#include "math.h"

int dirCalc(int *ampDat, int *angleDat, int nAngle, int *ampMax, int *angleCen)
{
	// if fail
	if (nAngle <= 0)
	{
		return(0);
	}
	if (nAngle > MAXnAngle)
	{
		return(0);
	}
	
	// if only 1 data
	if (nAngle == 1)
	{
		*ampMax = ampDat[0];
		*angleCen = angleDat[0];
		return(1);
	}

	// initialization
	// int *angleDatSort = (int *)malloc(nAngle * sizeof(int));
	// int *ampDatSort = (int *)malloc(nAngle * sizeof(int));
	// int *angleDatDiff = (int *)malloc(nAngle * sizeof(int));
	// int *wCoef = (int *)malloc(nAngle * sizeof(int));
	static int angleDatSort[MAXnAngle];
	static int ampDatSort[MAXnAngle];
	static int angleDatDiff[MAXnAngle];
	static int wCoef[MAXnAngle];
	int ampDatTmp = 0, angleDatTmp = 0, ampDatMin = 0;
	int iAngle = 0, jAngle = 0;
	int maxAngleDiff = 0, iMaxAngleDiff = 0;
	int tmpAngleDiff = 0, sumAngle = 0;
	double sumNorth = 0.0, sumWest = 0.0;

	// sort angleDat
	*ampMax = 0;
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		angleDatSort[iAngle] = angleDat[iAngle];
		ampDatSort[iAngle] = ampDat[iAngle];

		// find min amp
		if (ampDat[iAngle] > 0)
		{
			if (ampDat[iAngle] < ampDatMin || ampDatMin == 0)
			ampDatMin = ampDat[iAngle];
		}

		// find max amp
		if (ampDat[iAngle] > *ampMax)
		{
			*ampMax = ampDat[iAngle];
		}
	}
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		if (ampDatSort[iAngle] > 0)
		{
			ampDatSort[iAngle] -= ampDatMin;
		}
	}
	// max - 6dB
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		if (*ampMax - ampDatMin > 510)
		{
			if (ampDatSort[iAngle] > *ampMax - ampDatMin - 510)
			{
				ampDatSort[iAngle] -= *ampMax - ampDatMin - 510;
			}
			else
			{
				ampDatSort[iAngle] = 0;
			}
		}
	}

	jAngle = nAngle - 1;
	while (jAngle > 0)
	{
		for (iAngle = 0; iAngle < jAngle; iAngle++)
		{
			if (angleDatSort[iAngle] > angleDatSort[iAngle + 1])
			{
				angleDatTmp = angleDatSort[iAngle + 1];
				angleDatSort[iAngle + 1] = angleDatSort[iAngle];
				angleDatSort[iAngle] = angleDatTmp;
				ampDatTmp = ampDatSort[iAngle + 1];
				ampDatSort[iAngle + 1] = ampDatSort[iAngle];
				ampDatSort[iAngle] = ampDatTmp;
			}
		}
		jAngle--;
	}

	// calculate angle diff
	for (iAngle = 0; iAngle < nAngle - 1; iAngle++)
	{
		angleDatDiff[iAngle] = angleDatSort[iAngle + 1] - angleDatSort[iAngle];
	}
	angleDatDiff[nAngle - 1] = angleDatSort[0] - angleDatSort[nAngle - 1] + (int)(360.0 / UnitAngle);

	// calculate max angle diff
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		if (angleDatDiff[iAngle] > maxAngleDiff)
		{
			maxAngleDiff = angleDatDiff[iAngle];
			iMaxAngleDiff = iAngle;
		}
	}

	// calculate weighting coefficient
	wCoef[0] = (angleDatDiff[0] + angleDatDiff[nAngle - 1]) / 2;
	for (iAngle = 1; iAngle < nAngle; iAngle++)
	{
		wCoef[iAngle] = (angleDatDiff[iAngle] + angleDatDiff[iAngle - 1]) / 2;
	}
	/*
	if (iMaxAngleDiff == 0)
	{
		wCoef[0] = angleDatDiff[nAngle - 1];
		wCoef[1] = angleDatDiff[1];
	}
	else if (iMaxAngleDiff == nAngle - 1)
	{
		wCoef[nAngle - 1] = angleDatDiff[nAngle - 2];
		wCoef[0] = angleDatDiff[0];
	}
	else
	{
		wCoef[iMaxAngleDiff] = angleDatDiff[iMaxAngleDiff - 1];
		wCoef[iMaxAngleDiff + 1] = angleDatDiff[iMaxAngleDiff + 1];
	}
	*/

	// calculate centrol angle
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		sumNorth += cos((double)(angleDatSort[iAngle]) * UnitAngle * PI_BY_180)
				  * (double)(ampDatSort[iAngle]) * (double)(wCoef[iAngle]);
		sumWest += sin((double)(angleDatSort[iAngle]) * UnitAngle * PI_BY_180)
			     * (double)(ampDatSort[iAngle]) * (double)(wCoef[iAngle]);
	}
	*angleCen = (int)(atan2(sumWest, sumNorth) / PI_BY_180 / UnitAngle);

	for (iAngle = 0; iAngle < nAngle - 1; iAngle++)
	{
		tmpAngleDiff = angleDat[iAngle + 1] - angleDat[iAngle];

		if (tmpAngleDiff >(int)(180.0 / UnitAngle))
		{
			tmpAngleDiff -= (int)(360.0 / UnitAngle);
		}
		if (tmpAngleDiff < (int)(-180.0 / UnitAngle))
		{
			tmpAngleDiff += (int)(360.0 / UnitAngle);
		}

		sumAngle += tmpAngleDiff;
	}
//	*angleCen += (int)((float)sumAngle / (float)nAngle / 0.210 * 0.200); // T = 210ms, Delay = 200ms
	*angleCen += (int)((float)sumAngle / (float)nAngle / 0.270 * 0.150); // T = 210ms, Delay = 200ms
	if (*angleCen < 0)
	{
		*angleCen += (int)(360.0 / UnitAngle);
	}

	return(1);
}

// search azimuth angle, with duplex channel
int dirCalcDup(int *ampDat, int *ampDatOmni, int *angleDat, int nAngle, int *ampMax, int *angleCen)
{
	// if fail
	if (nAngle <= 0)
	{
		return(0);
	}
	if (nAngle > MAXnAngle)
	{
		return(0);
	}

	// if only 1 data
	if (nAngle == 1)
	{
		*ampMax = ampDat[0];
		*angleCen = angleDat[0];
		return(1);
	}

	// initialization
	// int *angleDatSort = (int *)malloc(nAngle * sizeof(int));
	// int *ampDatSort = (int *)malloc(nAngle * sizeof(int));
	// int *ampDatOmniSort = (int *)malloc(nAngle * sizeof(int));
	// int *angleDatDiff = (int *)malloc(nAngle * sizeof(int));
	// int *wCoef = (int *)malloc(nAngle * sizeof(int));
	static int angleDatSort[MAXnAngle];
	static int ampDatSort[MAXnAngle];
	static int ampDatOmniSort[MAXnAngle];
	static int angleDatDiff[MAXnAngle];
	static int wCoef[MAXnAngle];
	int ampDatTmp = 0, ampDatOmniTmp = 0, angleDatTmp = 0;
	int ampDatMin = 0, ampDatOmniMin = 0;
	int ampDatMax = 0, ampDatOmniMax = 0;
	int iAngle = 0, jAngle = 0;
	int maxAngleDiff = 0, iMaxAngleDiff = 0;
	int tmpAngleDiff = 0, sumAngle = 0;
	double sumNorth = 0.0, sumWest = 0.0;

	// sort angleDat
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		angleDatSort[iAngle] = angleDat[iAngle];
		ampDatSort[iAngle] = ampDat[iAngle];
		ampDatOmniSort[iAngle] = ampDatOmni[iAngle];

		// find min amp
		if (ampDat[iAngle] > 0)
		{
			if (ampDat[iAngle] < ampDatMin || ampDatMin == 0)
				ampDatMin = ampDat[iAngle];
		}
		if (ampDatOmni[iAngle] > 0)
		{
			if (ampDatOmni[iAngle] < ampDatOmniMin || ampDatOmniMin == 0)
				ampDatOmniMin = ampDatOmni[iAngle];
		}

		// find max amp
		if (ampDat[iAngle] > ampDatMax)
		{
			ampDatMax = ampDat[iAngle];
		}
		if (ampDatOmni[iAngle] > ampDatOmniMax)
		{
			ampDatOmniMax = ampDatOmni[iAngle];
		}
	}
	*ampMax = ampDatMax;
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		if (ampDatSort[iAngle] > 0)
		{
			ampDatSort[iAngle] -= ampDatMin;
		}
		if (ampDatOmniSort[iAngle] > 0)
		{
			ampDatOmniSort[iAngle] -= ampDatOmniMin;
		}

		ampDatSort[iAngle] += (ampDatOmniMax - ampDatOmniMin - ampDatOmniSort[iAngle]) / 2;
	}

	// find max/min amp again
	ampDatMax = 0;
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		if (ampDatSort[iAngle] > ampDatMax)
		{
			ampDatMax = ampDatSort[iAngle];
		}
	}
	ampDatMin = ampDatMax;
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		if (ampDatSort[iAngle] < ampDatMin)
		{
			ampDatMin = ampDatSort[iAngle];
		}
	}
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		ampDatSort[iAngle] -= ampDatMin;
	}

	// max - 6dB
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		if (ampDatMax - ampDatMin > 510)
		{
			if (ampDatSort[iAngle] > ampDatMax - ampDatMin - 510)
			{
				ampDatSort[iAngle] -= ampDatMax - ampDatMin - 510;
			}
			else
			{
				ampDatSort[iAngle] = 0;
			}
		}
	}

	jAngle = nAngle - 1;
	while (jAngle > 0)
	{
		for (iAngle = 0; iAngle < jAngle; iAngle++)
		{
			if (angleDatSort[iAngle] > angleDatSort[iAngle + 1])
			{
				angleDatTmp = angleDatSort[iAngle + 1];
				angleDatSort[iAngle + 1] = angleDatSort[iAngle];
				angleDatSort[iAngle] = angleDatTmp;
				ampDatTmp = ampDatSort[iAngle + 1];
				ampDatSort[iAngle + 1] = ampDatSort[iAngle];
				ampDatSort[iAngle] = ampDatTmp;
			}
		}
		jAngle--;
	}

	// calculate angle diff
	for (iAngle = 0; iAngle < nAngle - 1; iAngle++)
	{
		angleDatDiff[iAngle] = angleDatSort[iAngle + 1] - angleDatSort[iAngle];
	}
	angleDatDiff[nAngle - 1] = angleDatSort[0] - angleDatSort[nAngle - 1] + (int)(360.0 / UnitAngle);

	// calculate max angle diff
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		if (angleDatDiff[iAngle] > maxAngleDiff)
		{
			maxAngleDiff = angleDatDiff[iAngle];
			iMaxAngleDiff = iAngle;
		}
	}

	// calculate weighting coefficient
	wCoef[0] = (angleDatDiff[0] + angleDatDiff[nAngle - 1]) / 2;
	for (iAngle = 1; iAngle < nAngle; iAngle++)
	{
		wCoef[iAngle] = (angleDatDiff[iAngle] + angleDatDiff[iAngle - 1]) / 2;
	}
	/*
	if (iMaxAngleDiff == 0)
	{
	wCoef[0] = angleDatDiff[nAngle - 1];
	wCoef[1] = angleDatDiff[1];
	}
	else if (iMaxAngleDiff == nAngle - 1)
	{
	wCoef[nAngle - 1] = angleDatDiff[nAngle - 2];
	wCoef[0] = angleDatDiff[0];
	}
	else
	{
	wCoef[iMaxAngleDiff] = angleDatDiff[iMaxAngleDiff - 1];
	wCoef[iMaxAngleDiff + 1] = angleDatDiff[iMaxAngleDiff + 1];
	}
	*/

	// calculate centrol angle
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		sumNorth += cos((double)(angleDatSort[iAngle]) * UnitAngle * PI_BY_180)
			* (double)(ampDatSort[iAngle]) * (double)(wCoef[iAngle]);
		sumWest += sin((double)(angleDatSort[iAngle]) * UnitAngle * PI_BY_180)
			* (double)(ampDatSort[iAngle]) * (double)(wCoef[iAngle]);
	}
	*angleCen = (int)(atan2(sumWest, sumNorth) / PI_BY_180 / UnitAngle);

	for (iAngle = 0; iAngle < nAngle - 1; iAngle++)
	{
		tmpAngleDiff = angleDat[iAngle + 1] - angleDat[iAngle];

		if (tmpAngleDiff >(int)(180.0 / UnitAngle))
		{
			tmpAngleDiff -= (int)(360.0 / UnitAngle);
		}
		if (tmpAngleDiff < (int)(-180.0 / UnitAngle))
		{
			tmpAngleDiff += (int)(360.0 / UnitAngle);
		}

		sumAngle += tmpAngleDiff;
	}
	*angleCen += (int)((float)sumAngle / (float)nAngle / 0.210 * 0.500); // T = 210ms, Delay = 500ms
	if (*angleCen < 0)
	{
		*angleCen += (int)(360.0 / UnitAngle);
	}

	return(1);
}

int pitchCalc(int *ampDat, int *pitchDat, int nPitch, int *ampMax, int *pitchCen)
{
	// if fail
	if (nPitch <= 0)
	{
		return(0);
	}
	if (nPitch > MAXnAngle)
	{
		return(0);
	}

	// if only 1 data
	if (nPitch == 1)
	{
		*ampMax = ampDat[0];
		*pitchCen = pitchDat[0];
		return(1);
	}

	// initialization
	// int *pitchDatSort = (int *)malloc(nPitch * sizeof(int));
	// int *ampDatSort = (int *)malloc(nPitch * sizeof(int));
	// int *pitchDatDiff = (int *)malloc(nPitch * sizeof(int));
	// int *wCoef = (int *)malloc(nPitch * sizeof(int));
	static int pitchDatSort[MAXnAngle];
	static int ampDatSort[MAXnAngle];
	static int pitchDatDiff[MAXnAngle];
	static int wCoef[MAXnAngle];
	int ampDatTmp = 0, pitchDatTmp = 0, ampDatMin = 0;
	int iPitch = 0, jPitch = 0;
	int maxPitchDiff = 0, iMaxPitchDiff = 0;
	double sumFront = 0.0, sumUp = 0.0;

	// sort angleDat
	*ampMax = 0;
	for (iPitch = 0; iPitch < nPitch; iPitch++)
	{
		pitchDatSort[iPitch] = pitchDat[iPitch];
		ampDatSort[iPitch] = ampDat[iPitch];

		// find min amp
		if (ampDat[iPitch] > 0)
		{
			if (ampDat[iPitch] < ampDatMin || ampDatMin == 0)
				ampDatMin = ampDat[iPitch];
		}

		// find max amp
		if (ampDat[iPitch] > *ampMax)
		{
			*ampMax = ampDat[iPitch];
		}
	}
	for (iPitch = 0; iPitch < nPitch; iPitch++)
	{
		if (ampDatSort[iPitch] > 0)
		{
			ampDatSort[iPitch] -= ampDatMin;
		}
	}

	jPitch = nPitch - 1;
	while (jPitch > 0)
	{
		for (iPitch = 0; iPitch < jPitch; iPitch++)
		{
			if (pitchDatSort[iPitch] > pitchDatSort[iPitch + 1])
			{
				pitchDatTmp = pitchDatSort[iPitch + 1];
				pitchDatSort[iPitch + 1] = pitchDatSort[iPitch];
				pitchDatSort[iPitch] = pitchDatTmp;
				ampDatTmp = ampDatSort[iPitch + 1];
				ampDatSort[iPitch + 1] = ampDatSort[iPitch];
				ampDatSort[iPitch] = ampDatTmp;
			}
		}
		jPitch--;
	}

	// calculate angle diff
	for (iPitch = 0; iPitch < nPitch - 1; iPitch++)
	{
		pitchDatDiff[iPitch] = pitchDatSort[iPitch + 1] - pitchDatSort[iPitch];
	}


	// calculate weighting coefficient
	wCoef[0] = (pitchDatDiff[0]) / 2;
	for (iPitch = 1; iPitch < nPitch - 1; iPitch++)
	{
		wCoef[iPitch] = (pitchDatDiff[iPitch] + pitchDatDiff[iPitch - 1]) / 2;
	}
	wCoef[nPitch - 1] = (pitchDatDiff[nPitch - 2]) / 2;

	// calculate centrol angle
	for (iPitch = 0; iPitch < nPitch; iPitch++)
	{
		if (pitchDatSort[iPitch] < (int)(PitchCircMin / UnitAngle))
		{
			continue;
		}
		if (pitchDatSort[iPitch] > (int)(PitchCircMax / UnitAngle))
		{
			continue;
		}
		sumFront += cos((double)(pitchDatSort[iPitch]) * UnitAngle * PI_BY_180)
			* (double)(ampDatSort[iPitch]) * (double)(wCoef[iPitch]);
		sumUp += sin((double)(pitchDatSort[iPitch]) * UnitAngle * PI_BY_180)
			* (double)(ampDatSort[iPitch]) * (double)(wCoef[iPitch]);
	}
	*pitchCen = (int)(atan2(sumUp, sumFront) / PI_BY_180 / UnitAngle);
	if (*pitchCen < (int)(PitchCircMin / UnitAngle))
	{
		*pitchCen = (int)(PitchCircMin / UnitAngle);
	}
	if (*pitchCen > (int)(PitchCircMax / UnitAngle))
	{
		*pitchCen = (int)(PitchCircMax / UnitAngle);
	}

	return(1);
}

// search max amp for pitch
int pitchCalcMax(int *ampDat, int *pitchDat, int nPitch, int *ampMax, int *pitchCen)
{
	// if fail
	if (nPitch <= 0)
	{
		return(0);
	}
	if (nPitch > MAXnAngle)
	{
		return(0);
	}

	int ampDatMax = 0;
	int iPitchMax = 0;
	int iPitch = 0;
	int nValid = 0;

	for (iPitch = 0; iPitch < nPitch; iPitch++)
	{
		if (pitchDat[iPitch] < (int)(PitchCircMin / UnitAngle))
		{
			continue;
		}
		if (pitchDat[iPitch] > (int)(PitchCircMax / UnitAngle))
		{
			continue;
		}

		// find max amp
		if (ampDat[iPitch] > ampDatMax)
		{
			iPitchMax = iPitch;
			ampDatMax = ampDat[iPitch];
		}

		nValid++;
	}

	if (ampDatMax > 0)
	{
		*ampMax = ampDatMax;

		*pitchCen = pitchDat[iPitchMax];

		if (*pitchCen < 200)
		{
			*pitchCen = 200;
		}

		if (*pitchCen > 1500)
		{
			*pitchCen = (*pitchCen - 1000) / 4 + 1000;
		}

		return(1);
	}
	else
	{
		*ampMax = 0;
		*pitchCen = 1500;
		return(0);
	}
}

// search max amp for pitch, with duplex channel
int pitchCalcDupMax(int *ampDat, int *ampDatOmni, int *pitchDat, int nPitch, int *ampMax, int *pitchCen)
{
	// if fail
	if (nPitch <= 0)
	{
		return(0);
	}
	if (nPitch > MAXnAngle)
	{
		return(0);
	}

	int ampDatMax = 0;
	int iPitchMax = 0;
	int iPitch = 0;

	for (iPitch = 0; iPitch < nPitch; iPitch++)
	{
		if (pitchDat[iPitch] < (int)(PitchCircMin / UnitAngle))
		{
			continue;
		}
		if (pitchDat[iPitch] > (int)(PitchCircMax / UnitAngle))
		{
			continue;
		}

		//
		if (ampDat[iPitch] == 0)
		{
			continue;
		}
		if (ampDatOmni[iPitch] == 0)
		{
			continue;
		}


		// find max amp
		if (ampDat[iPitch] - ampDatOmni[iPitch] + 16384 > ampDatMax)
		{
			iPitchMax = iPitch;
			ampDatMax = ampDat[iPitch] - ampDatOmni[iPitch] + 16384;
		}
	}

	if (ampDatMax > 0)
	{
		*pitchCen = pitchDat[iPitchMax];
		*ampMax = ampDat[iPitchMax];

		if (*pitchCen < 200)
		{
			*pitchCen = 200;
		}

		return(1);
	}
	else
	{
		*pitchCen = 1500;
		*ampMax = 0;
		return(0);
	}
}



int sfDxIfAzimuthEnough(float *listAngleAz, int numDx, float *oStart, float *oEnd)
{
	if (numDx < 2)
	{
		return(0);
	}
	if (numDx > MAXnAngle)
	{
		return(0);
	}

	float sumAzimuth = 0, diffAzimuth = 0;
	int iAzimuth = 0;
	float startAzimuth = 0, endAzimuth = 0;
	float scanAzimuth = 0;
	startAzimuth = listAngleAz[0];
	endAzimuth = listAngleAz[0]+1;
	for (iAzimuth = 0; iAzimuth < numDx - 1; iAzimuth++)
	{
		diffAzimuth = listAngleAz[iAzimuth + 1] - listAngleAz[iAzimuth];
		if (diffAzimuth > 180)
		{
			diffAzimuth -= 360;
		}
		if (diffAzimuth < -180)
		{
			diffAzimuth += 360;
		}
		sumAzimuth += diffAzimuth;
		// 实时判断范围
		if (listAngleAz[0] + sumAzimuth > endAzimuth)
		{
			endAzimuth = listAngleAz[0] + sumAzimuth;
		}
		else if (listAngleAz[0] + sumAzimuth < startAzimuth)
		{
			startAzimuth = listAngleAz[0] + sumAzimuth;
		}
	}
	// 扫描范围计算
	scanAzimuth = endAzimuth - startAzimuth;
	if (scanAzimuth >= 360)
	{
		endAzimuth = startAzimuth + 359.9;
	}
	// 值域转换
	if (startAzimuth > 360)
	{
		startAzimuth -= 360;
	}
	if (startAzimuth < 0)
	{
		startAzimuth += 360;
	}
	if (endAzimuth > 360)
	{
		endAzimuth -= 360;
	}
	if (endAzimuth < 0)
	{
		endAzimuth += 360;
	}
	*oStart = startAzimuth;
	*oEnd = endAzimuth;

	if (scanAzimuth > 360)
	{
		return(1);
	}
	else
	{
		return(0);
	}
}

int sfDxDirCalc(float *ampDat, float *angleDat, int nAngle, float *ampMax, float *angleCen, int typeD)
{
	// typeD = 0代表选择2.4定向，typeD = 1代表选择5.8定向
	// 角度输入值必须在0-360之间，为前置约束
	// 20230704:修改输入数组均改为float类型

	// 预定义方向图，线性功率方向图
	float pattern24[360] = { 0.9750,0.9638,0.9462,0.9290,0.9078,0.8831,0.8570,0.8293,0.7993,0.7675,0.7340,0.6992,0.6631,0.6260,0.5882,0.5498,0.5112,0.4727,0.4345,0.3969,0.3603,0.3250,0.2913,0.2595,0.2300,0.2031,0.1790,0.1581,0.1405,0.1264,0.1159,0.1091,0.1059,0.1062,0.1099,0.1165,0.1258,0.1373,0.1503,0.1643,0.1786,0.1926,0.2056,0.2169,0.2260,0.2324,0.2357,0.2356,0.2322,0.2254,0.2155,0.2030,0.1884,0.1724,0.1557,0.1392,0.1236,0.1097,0.09820,0.08960,0.08430,0.08250,0.08400,0.08880,0.09630,0.1060,0.1172,0.1292,0.1409,0.1517,0.1609,0.1677,0.1718,0.1727,0.1705,0.1652,0.1570,0.1464,0.1338,0.1198,0.1051,0.09030,0.07590,0.06250,0.05030,0.03980,0.03110,0.02420,0.01920,0.01590,0.01420,0.01390,0.01490,0.01690,0.01980,0.02350,0.02780,0.03270,0.03800,0.04370,0.04970,0.05590,0.06230,0.06870,0.07490,0.08070,0.08610,0.09070,0.09460,0.09740,0.09910,0.09960,0.09890,0.09700,0.09390,0.08970,0.08460,0.07870,0.07220,0.06540,0.05830,0.05130,0.04440,0.03780,0.03180,0.02630,0.02160,0.01750,0.01420,0.01170,0.009900,0.008900,0.008500,0.008800,0.009600,0.01080,0.01250,0.01440,0.01660,0.01900,0.02140,0.02390,0.02630,0.02870,0.03090,0.03300,0.03490,0.03660,0.03800,0.03930,0.04020,0.04100,0.04150,0.04170,0.04180,0.04170,0.04140,0.04090,0.04030,0.03950,0.03870,0.03780,0.03680,0.03580,0.03470,0.03360,0.03250,0.03150,0.03040,0.02940,0.02840,0.02750,0.02660,0.02580,0.02510,0.02440,0.02380,0.02340,0.02290,0.02260,0.02230,0.02220,0.02200,0.02200,0.02190,0.02190,0.02190,0.02190,0.02190,0.02180,0.02170,0.02150,0.02120,0.02080,0.02030,0.01960,0.01880,0.01790,0.01690,0.01580,0.01470,0.01350,0.01230,0.01120,0.01010,0.009200,0.008500,0.008000,0.007800,0.007900,0.008200,0.008900,0.009900,0.01120,0.01270,0.01430,0.01610,0.01780,0.01950,0.02100,0.02230,0.02320,0.02380,0.02390,0.02350,0.02270,0.02150,0.02000,0.01830,0.01650,0.01470,0.01300,0.01160,0.01060,0.01000,0.01000,0.01060,0.01170,0.01320,0.01520,0.01740,0.01970,0.02190,0.02400,0.02580,0.02710,0.02790,0.02830,0.02810,0.02750,0.02660,0.02550,0.02430,0.02320,0.02240,0.02180,0.02170,0.02200,0.02270,0.02380,0.02520,0.02680,0.02860,0.03040,0.03220,0.03400,0.03570,0.03730,0.03900,0.04070,0.04270,0.04480,0.04730,0.05020,0.05340,0.05690,0.06070,0.06450,0.06820,0.07170,0.07480,0.07740,0.07920,0.08030,0.08050,0.07990,0.07870,0.07680,0.07440,0.07170,0.06900,0.06620,0.06360,0.06140,0.05940,0.05790,0.05680,0.05610,0.05570,0.05550,0.05560,0.05580,0.05620,0.05670,0.05740,0.05820,0.05920,0.06060,0.06240,0.06470,0.06760,0.07120,0.07550,0.08070,0.08690,0.09400,0.1021,0.1112,0.1215,0.1328,0.1453,0.1590,0.1740,0.1903,0.2080,0.2271,0.2477,0.2698,0.2934,0.3186,0.3453,0.3735,0.4031,0.4340,0.4660,0.4991,0.5330,0.5673,0.6020,0.6368,0.6714,0.7055,0.7388,0.7711,0.8020,0.8316,0.8590,0.8851,0.9078,0.9290,0.9484,0.9638,0.9772,0.9863,0.9954,1,1,0.9977,0.9931,0.9863 };
	float pattern58[360] = { 0.9931,0.9977,1,0.9954,0.9908,0.9795,0.9661,0.9462,0.9247,0.8954,0.861,0.8202,0.7736,0.7209,0.6631,0.6009,0.5355,0.4686,0.4019,0.3373,0.2766,0.2218,0.1739,0.1342,0.1029,0.0801,0.0650,0.0565,0.0532,0.0536,0.0562,0.0595,0.0628,0.0653,0.0671,0.0682,0.0690,0.0698,0.0706,0.0715,0.0721,0.0718,0.0704,0.0674,0.0628,0.0570,0.0505,0.0443,0.0394,0.0369,0.0380,0.0433,0.0533,0.0683,0.0876,0.110,0.1339,0.1571,0.177,0.1914,0.1987,0.198,0.1894,0.174,0.1538,0.1308,0.1075,0.0857,0.0672,0.0528,0.0433,0.0386,0.0386,0.0427,0.0504,0.0604,0.0718,0.0832,0.0938,0.1028,0.1101,0.1155,0.1195,0.1221,0.1234,0.1231,0.1206,0.1152,0.1067,0.0950,0.0809,0.0655,0.0505,0.0373,0.0271,0.0205,0.0174,0.0172,0.0193,0.0230,0.0281,0.0350,0.0439,0.0552,0.0683,0.0818,0.0936,0.1012,0.1024,0.0966,0.0843,0.0677,0.0499,0.0339,0.0220,0.0151,0.0127,0.0132,0.0149,0.0164,0.0169,0.0171,0.0182,0.0219,0.0294,0.0411,0.0564,0.0733,0.0891,0.1009,0.1064,0.1043,0.0945,0.0788,0.0599,0.0410,0.0252,0.0150,0.0117,0.0153,0.0250,0.0393,0.0562,0.0740,0.0912,0.1067,0.1195,0.1292,0.1353,0.1375,0.1358,0.1303,0.1213,0.1095,0.0958,0.0812,0.0666,0.0530,0.0410,0.0310,0.0232,0.0175,0.0137,0.0116,0.0110,0.0116,0.0132,0.0158,0.0192,0.0232,0.0278,0.0327,0.0374,0.0419,0.0457,0.0485,0.0501,0.0506,0.0500,0.0486,0.0468,0.0451,0.0440,0.0441,0.0456,0.0488,0.0537,0.0598,0.0667,0.0734,0.0791,0.0830,0.0843,0.0827,0.0782,0.0712,0.0626,0.0535,0.0451,0.0384,0.0342,0.0325,0.0328,0.0342,0.0353,0.0351,0.0326,0.0278,0.0212,0.0139,0.00740,0.00310,0.00170,0.00340,0.00770,0.0135,0.0200,0.0266,0.0335,0.0412,0.0499,0.0593,0.0681,0.0742,0.0753,0.0703,0.0599,0.0473,0.0373,0.0349,0.0433,0.0621,0.0874,0.1122,0.1294,0.1342,0.1255,0.1071,0.0850,0.0657,0.0530,0.0475,0.0467,0.0471,0.0463,0.0437,0.0409,0.0393,0.0394,0.0397,0.0380,0.0330,0.0258,0.0200,0.0203,0.0296,0.0475,0.0689,0.0868,0.0944,0.0890,0.0731,0.0535,0.0384,0.0347,0.0443,0.0645,0.0884,0.1082,0.1178,0.1148,0.1008,0.0809,0.0616,0.0483,0.0436,0.0468,0.0544,0.0619,0.0651,0.0624,0.0547,0.0449,0.0365,0.0319,0.0318,0.0349,0.0388,0.0414,0.0413,0.0387,0.0345,0.0300,0.0261,0.0229,0.0198,0.0165,0.0124,0.00800,0.00420,0.00250,0.00440,0.0111,0.0227,0.0385,0.0562,0.0733,0.0868,0.0947,0.0963,0.0925,0.0854,0.0777,0.0715,0.0680,0.0667,0.0663,0.0649,0.0615,0.0559,0.0495,0.0443,0.0427,0.0460,0.0541,0.0656,0.0777,0.0874,0.0922,0.0913,0.0853,0.0768,0.0695,0.0674,0.0741,0.0918,0.1213,0.1617,0.2105,0.2646,0.3208,0.3762,0.4284,0.4763,0.5198,0.5591,0.5955,0.6304,0.6645,0.6989,0.7338,0.7693,0.8046,0.8395,0.873,0.9016,0.929,0.9506,0.9705,0.9840 };

	// 测量数量跳出判定
	if (nAngle <= 0)
	{
		return(0);
	}
	if (nAngle > MAXnAngle) // MAXnAngle定义为256
	{
		return(0);
	}
	// 单个数值赋值情况
	if (nAngle == 1)
	{
		*ampMax = ampDat[0];
		*angleCen = angleDat[0];
		return(1);
	}
	// 数组初始化
	static float angleDatSort[MAXnAngle];
	static float ampDatSort[MAXnAngle];
	static float angleDatDiff[MAXnAngle];
	static float wCoef[MAXnAngle];
	float ampDatTmp = 0, angleDatTmp = 0, ampDatMin = 0;
	int iAngle = 0, jAngle = 0;
	float maxAngleDiff = 0, iMaxAngleDiff = 0;
	float tmpAngleDiff = 0, sumAngle = 0;
	double sumNorth = 0.0, sumWest = 0.0;

	// 查找最大最小值
	*ampMax = 0;
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		angleDatSort[iAngle] = angleDat[iAngle];
		ampDatSort[iAngle] = ampDat[iAngle];

		// 查找最小幅度
		if (ampDat[iAngle] > 0)
		{
			if (ampDat[iAngle] < ampDatMin || ampDatMin == 0)
				ampDatMin = ampDat[iAngle];
		}

		// 查找最大幅度
		if (ampDat[iAngle] > *ampMax)
		{
			*ampMax = ampDat[iAngle];
		}
	}
	// 排列输出的角度数组
	jAngle = nAngle - 1;
	while (jAngle > 0)
	{
		for (iAngle = 0; iAngle < jAngle; iAngle++)
		{
			if (angleDatSort[iAngle] > angleDatSort[iAngle + 1])
			{
				angleDatTmp = angleDatSort[iAngle + 1];
				angleDatSort[iAngle + 1] = angleDatSort[iAngle];
				angleDatSort[iAngle] = angleDatTmp;
				ampDatTmp = ampDatSort[iAngle + 1];
				ampDatSort[iAngle + 1] = ampDatSort[iAngle];
				ampDatSort[iAngle] = ampDatTmp;
			}
		}
		jAngle--;
	}

	// 剔除0幅度计算结果
	static float angleEffect[MAXnAngle];
	static float ampEffect[MAXnAngle];
	int kAngle = 0;
	for (iAngle = 0; iAngle < nAngle; iAngle++)
	{
		if (ampDatSort[iAngle] > 0)
		{
			angleEffect[kAngle] = angleDatSort[iAngle]; // 角度直接转小数
			ampEffect[kAngle] = pow(10, ((ampDatSort[iAngle] - *ampMax)*0.1 / 85)); // 归一化转线性幅度值
			kAngle++;
		}
	}
	// 方向图插值-1度间隔
	static float ampConstruct[360];
	int i, j, k;
	float tmp1 = 0;
	float tmp1_bv = -180;
	float tmp1_tv = 180;
	int tmp1_bi = 0;
	int tmp1_ti = 0;

	for (i = 0; i < 360; i++)
	{
		// 初始化
		tmp1 = 0;
		tmp1_bv = -180;
		tmp1_tv = 180;
		tmp1_bi = 0;
		tmp1_ti = 0;
		// 查找上下边界点
		for (j = 0; j < kAngle; j++)
		{
			tmp1 = angleEffect[j] - i;// 角度插值
			// 解跳周
			if (tmp1 > 180)
				tmp1 = tmp1 - 360;
			else if (tmp1 < -180)
				tmp1 = tmp1 + 360;
			// 计算最近点位置
			if (tmp1 <= 0)
			{
				if (tmp1 > tmp1_bv)
				{
					tmp1_bv = tmp1;
					tmp1_bi = j;
				}
			}
			else
			{
				if (tmp1 < tmp1_tv)
				{
					tmp1_tv = tmp1;
					tmp1_ti = j;
				}
			}
		}
		// 计算插值
		float tmp2 = 0;
		// 如果单边未找到15范围内的角度，则当前位置插值直接赋值0-20230704
		if (tmp1_bv <= -15 || tmp1_tv >= 15)
		{
			tmp2 = 0;
		}
		else
		{
			float tmp2_l = tmp1_tv - tmp1_bv;
			float tmp2_w = 0 - tmp1_bv;
			float tmp2_v = ampEffect[tmp1_ti] - ampEffect[tmp1_bi];
			float tmp2_s = ampEffect[tmp1_bi];
			tmp2 = tmp2_s + tmp2_v * (tmp2_w / tmp2_l);
		}
		// 插值点赋值
		ampConstruct[i] = tmp2;
	}
	// 插值有效判定
	j = 0;
	for(i =0;i<360;i++)
	{
		if (ampConstruct[i] == 0)
		{
			j++;
		}
	}
	if(j == 360)
	{
		*ampMax = 0;
		*angleCen = 0;
		return(0);
	}

	// 与方向图卷积
	float convResult[360];
	float tmp3 = 0;
	int tmp4 = 0;
	float* usePattern;
	float maxValue = 0;
	int maxLocation = 0;

	// 加条件判断，不同频段选择不同的曲线
	if (typeD == 0)
	{
		usePattern = pattern24;
	}
	else
	{
		usePattern = pattern58;
	}
	//usePattern = pattern24;
	// 循环计算卷积结果
	for (i = 0; i < 360; i++)
	{
		// i值即为方向图角度旋转偏移量
		tmp3 = 0;
		for (j = 0; j < 360; j++)
		{
			tmp4 = j + i; //卷积位置
			if (tmp4 >= 360)
			{
				tmp4 = tmp4 - 360;
			}
			tmp3 = tmp3 + usePattern[j] * ampConstruct[tmp4];
		}
		if (tmp3 > maxValue)
		{
			maxValue = tmp3;
			maxLocation = i;
		}
	}
	// 首次赋值
	*angleCen = maxLocation; // 浮点数赋值
	// 计算旋转方向
	for (iAngle = 0; iAngle < nAngle - 1; iAngle++)
	{
		tmpAngleDiff = angleDat[iAngle + 1] - angleDat[iAngle];

		if (tmpAngleDiff > 180)
		{
			tmpAngleDiff -= 360;
		}
		if (tmpAngleDiff < -180)
		{
			tmpAngleDiff += 360;
		}
		sumAngle += tmpAngleDiff;
	}
	// 旋转修正
	*angleCen += sumAngle / (float)nAngle / 0.120 * 0.150; // T = 120ms, Delay = 150ms
	// 值域约束
	if (*angleCen < 0)
	{
		*angleCen += 360;
	}
	else if (*angleCen > 360)
	{
		*angleCen -= 360;
	}
	return(1);
}

int sfDxIfAimed(float azimuthNow, float elevationNow, float azimuthCen)
{
	float azimuthValidHalf = 15; // 打击天线半波束宽度
	float diffAzimuth = 0;

	diffAzimuth = azimuthNow - azimuthCen;
	if (diffAzimuth > 180)
	{
		diffAzimuth -= 360;
	}
	if (diffAzimuth < -180)
	{
		diffAzimuth += 360;
	}

	if (abs(diffAzimuth) <= azimuthValidHalf && elevationNow >= 5 && elevationNow <= 15)
	{
		return(1);
	}
	else
	{
		return(0);
	}
}

int sfDxIfLegal(float *listAngleAz, float *listAngleEl, float *listAmQ, float *listAmD, float *listAmC, int *listFP, int numDx)
{
	// 测量属性计算
	int flagScanB30 = 0;
	int flagScanA360 = 0;
	int flagAmQAD = 0;
	int arryFullM[360+20] = { 0 };
	int flagFullM = 0;

	// 0:点数过多或者过少
	// -1：扫描范围不足60度
	// -2：定向最大功率小于全向最大功率
	if (numDx < 2)
	{
		return(0);
	}
	if (numDx > MAXnAngle)
	{
		return(0);
	}

	float sumAzimuth = 0, diffAzimuth = 0;
	int iAzimuth = 0;
	float startAzimuth = 0, endAzimuth = 0;
	float scanAzimuth = 0;
	startAzimuth = listAngleAz[0];
	endAzimuth = listAngleAz[0] + 1;
	for (iAzimuth = 0; iAzimuth < numDx - 1; iAzimuth++)
	{
		diffAzimuth = listAngleAz[iAzimuth + 1] - listAngleAz[iAzimuth];
		if (diffAzimuth > 180)
		{
			diffAzimuth -= 360;
		}
		if (diffAzimuth < -180)
		{
			diffAzimuth += 360;
		}
		sumAzimuth += diffAzimuth;
		// 实时判断范围
		if (listAngleAz[0] + sumAzimuth > endAzimuth)
		{
			endAzimuth = listAngleAz[0] + sumAzimuth;
		}
		else if (listAngleAz[0] + sumAzimuth < startAzimuth)
		{
			startAzimuth = listAngleAz[0] + sumAzimuth;
		}
	}
	// 扫描范围计算
	scanAzimuth = endAzimuth - startAzimuth;
	// 标志计算-扫描范围
	if (scanAzimuth < 30)
	{
		flagScanB30 = 1;
	}
	else if (scanAzimuth >= 350)
	{
		scanAzimuth = 360;
		flagScanA360 = 1;
	}
	// 目标信号条件有效识别
	float maxDX = 0;
	float maxQX = 0;
	int i, j, k = 0;
	float tmp1 = 0;

	for (i = 0; i < numDx; i++)
	{
		// 查找全向定向最大值
		if (listAmQ[i] > maxQX)
		{
			maxQX = listAmQ[i];
		}
		if (listAmD[i] > maxDX)
		{
			maxDX = listAmD[i];
		}
		// 有效测量稀疏性计算
		if (listAmC[i] == 1 || listAmC[i] == 2)
		{
			tmp1 = round(listAngleAz[i]);
			j = (int)(tmp1);
			if (j < 0 || j >= 360)
			{
				j = 0;
			}
			arryFullM[j] = 1;
		}
	}
	// 有效测量稀疏性计算
	for (i = 0; i < 20; i++)
	{
		arryFullM[360 + i] = arryFullM[i];
	}
	// 标志计算-定向最大功率高于全向最大功率
	if (maxQX > maxDX)
	{
		flagAmQAD = 1;
	}
	// 有效测量稀疏性计算
	for (i = 0; i < 360; i++)
	{
		if (arryFullM[i] == 1)
		{
			// 前向链接
			for (j = 20; j > 0; j--)
			{
				if (arryFullM[i + j] == 1)
				{
					for (k = 1; k < j; k++)
					{
						arryFullM[i + k] = 1;
					}
					// 调整循环位置
					i = i + j - 1;
					break;
				}
			}

		}
	}
	for (i = 0; i < 20; i++)
	{
		if (arryFullM[i] == 1 || arryFullM[360 + i] == 1)
		{
			arryFullM[i] = 1;
		}
	}
	tmp1 = 0;
	for (i = 0; i < 360; i++)
	{
		if (arryFullM[i] == 1)
		{
			tmp1 = tmp1 + 1;
		}
	}
	tmp1 = tmp1 / scanAzimuth;
	if (tmp1 < 0.7)
	{
		flagFullM = 1;
	}
	// 计算结果至返回值转换
	if (flagScanB30 == 1)
	{
		return 0;
	}
	if (flagAmQAD == 1)
	{
		return -1;
	}
	/*
	if (flagFullM == 1 && flagScanA360 == 1)
	{
		return -2;
	}
	*/
	return 1;
}


