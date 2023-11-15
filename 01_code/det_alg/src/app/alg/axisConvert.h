#ifndef _AXISCONVERT_H__
#define _AXISCONVERT_H__

#define UnitAngle 0.01
#define PI_BY_180 0.01745329252
#define R_EARTH 6360000.0
#define DisDefault 1500.0
//#define LatGunDefault 0.0
//#define LngGunDefault 0.0
//#define HGunDefault 0.0

int axisPolarToGpsAz(int azimuth, double latGun, double lngGun, double *latDrone, double *lngDrone);
int axisPolarToGps(int azimuth, int pitch, double latGun, double lngGun, double hGun, double *latDrone, double *lngDrone, double *hDrone);

#endif


