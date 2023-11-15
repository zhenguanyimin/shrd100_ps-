#include <stdlib.h>

#include "axisConvert.h"
#include "math.h"

int axisPolarToGpsAz(int azimuth, double latGun, double lngGun, double *latDrone, double *lngDrone)
{
	if (abs(azimuth) > (int)(180.0 / UnitAngle))
	{
		return(0);
	}
	if (fabs(latGun) >= 90.0)
	{
		return(0);
	}
	if (fabs(lngGun) > 180.0)
	{
		return(0);
	}

	*latDrone = cos((double)(azimuth)* UnitAngle * PI_BY_180) * DisDefault / R_EARTH / PI_BY_180 + latGun;
	*lngDrone = -sin((double)(azimuth)* UnitAngle * PI_BY_180) / cos(latGun * PI_BY_180) * DisDefault / R_EARTH / PI_BY_180 + lngGun;

	return(1);
}

int axisPolarToGps(int azimuth, int pitch, double latGun, double lngGun, double hGun, double *latDrone, double *lngDrone, double *hDrone)
{
	if (abs(azimuth) > (int)(180.0 / UnitAngle))
	{
		return(0);
	}
	if (abs(pitch) > (int)(90.0 / UnitAngle))
	{
		return(0);
	}
	if (fabs(latGun) >= 90.0)
	{
		return(0);
	}
	if (fabs(lngGun) > 180.0)
	{
		return(0);
	}

	*latDrone = cos((double)(azimuth) * UnitAngle * PI_BY_180) * DisDefault / R_EARTH / PI_BY_180 + latGun;
	*lngDrone = -sin((double)(azimuth) * UnitAngle * PI_BY_180) / cos(latGun * PI_BY_180) * DisDefault / R_EARTH / PI_BY_180 + lngGun;
	*hDrone = -sin((double)(pitch)* UnitAngle * PI_BY_180) * DisDefault + hGun;
	
	return(1);
}


