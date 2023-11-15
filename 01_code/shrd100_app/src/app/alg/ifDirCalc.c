#include <stdlib.h>

#include "ifDirCalc.h"
#include "math.h"

/*
int ifDirCirc(int pitchNow, float freq)
{
	if (pitchNow < (int)(DirCircPitchMin / UnitAngle))
	{
		return(0);
	}
	if (pitchNow > (int)(DirCircPitchMax / UnitAngle))
	{
		return(0);
	}

	if (freq >= 2400.0 && freq <= 2483.5)
	{
		return(1);
	}

	if (freq >= 5715.0 && freq <= 5850.0)
	{
		return(2);
	}

	return(0);
}
*/

int ifDirCircFreq(float freq)
{
	if (freq >= 2400.0 && freq <= 2483.5)
	{
		return(1);
	}

//	if (freq >= 5710.0 && freq <= 5850.0)
	if (freq >= 5710.0 && freq <= 5885.0)
	{
		return(2);
	}

	return(0);
}

int ifDirCircPitch(int *pitchDat, int nPitch)
{
	if (nPitch < 2)
	{
		return(0);
	}

	int iPitch = 0;

	if (pitchDat[(nPitch - 1) % MAXnAngle] > (int)(ifDirCircPitchMax/UnitAngle) )
	{
		iPitch = nPitch - 2;
		while (iPitch >= 0 && nPitch - iPitch <= ifDirCircPitchFrameMax)
		{
			if (pitchDat[iPitch % MAXnAngle] < (int)(ifDirCircPitchMin/UnitAngle) )
			{
				return(1);
			}
			
			iPitch--;
		}
	}
	return(0);
}

int ifAzimuthEnough(int *azimuthDat, int nAzimuth, int *totalAzimuth)
{
	if (nAzimuth < 2)
	{
		return(0);
	}

	if (nAzimuth > MAXnAngle)
	{
		return(0);
	}

	int sumAzimuth = 0, diffAzimuth = 0;
	int iAzimuth = 0;

	for (iAzimuth = 0; iAzimuth < nAzimuth - 1; iAzimuth++)
	{
		diffAzimuth = azimuthDat[iAzimuth + 1] - azimuthDat[iAzimuth];

		if (diffAzimuth >(int)(180.0 / UnitAngle))
		{
			diffAzimuth -= (int)(360.0 / UnitAngle);
		}
		if (diffAzimuth < (int)(-180.0 / UnitAngle))
		{
			diffAzimuth += (int)(360.0 / UnitAngle);
		}

		sumAzimuth += diffAzimuth;
	}

	*totalAzimuth = sumAzimuth;

	if (abs(sumAzimuth) < (int)(AzimuthScanWidth / UnitAngle))
	{
		return(0);
	}
	else
	{
		return(1);
	}
}

int ifAzimuthAimed(int azimuthNow, int azimuthCen)
{
	int azimuthValidHalf = (int)(AzimuthBeamWidth / 2.0 / UnitAngle);
	int diffAzimuth = 0;

	diffAzimuth = azimuthNow - azimuthCen;
	if (diffAzimuth > (int)(180.0 / UnitAngle))
	{
		diffAzimuth -= (int)(360.0 / UnitAngle);
	}
	if (diffAzimuth < (int)(-180.0 / UnitAngle))
	{
		diffAzimuth += (int)(360.0 / UnitAngle);
	}

	if (abs(diffAzimuth) <= azimuthValidHalf)
	{
		return(1);
	}
	else
	{
		return(0);
	}
}

int ifPitchEnough(int *pitchDat, int nPitch)
{
	if (nPitch < 2)
	{
		return(0);
	}

	if (nPitch > MAXnAngle)
	{
		return(0);
	}

	int minPitch = (int)(90.0 / UnitAngle);
	int maxPitch = (int)(-90.0 / UnitAngle);
	int iPitch = 0;

	for (iPitch = 0; iPitch < nPitch; iPitch++)
	{
		if (pitchDat[iPitch] < minPitch)
		{
			minPitch = pitchDat[iPitch];
		}
		if (pitchDat[iPitch] > maxPitch)
		{
			maxPitch = pitchDat[iPitch];
		}
	}

	if ((minPitch < (int)(PitchScanMin / UnitAngle)) && (maxPitch > (int)(PitchScanMax / UnitAngle)))
	{
		return(1);
	}
	else
	{
		return(0);
	}
}

/*
int ifPitchEnoughS(int *pitchDat, int nPitch)
{
	if (nPitch < 2)
	{
		return(0);
	}
	
	if (nPitch >= MAXnAngle)
	{
		return(0);
	}

	int sumPitch = 0, diffPitch = 0;
	int iPitch = 0;

	for (iPitch = 0; iPitch < nPitch - 1; iPitch++)
	{
		diffPitch = pitchDat[iPitch + 1] - pitchDat[iPitch];

		sumPitch += diffPitch;
	}

	if (abs(sumPitch) < (int)(PitchScanWidth / UnitAngle))
	{
		return(0);
	}
	else
	{
		return(1);
	}
}
*/

int ifPitchAimed(int pitchNow, int pitchCen)
{
	int pitchValidHalf = (int)(PitchBeamWidth / 2.0 / UnitAngle);
	int diffPitch = 0;

	diffPitch = pitchNow - pitchCen;

	if (abs(diffPitch) <= pitchValidHalf)
	{
		return(1);
	}
	else
	{
		return(0);
	}
}


