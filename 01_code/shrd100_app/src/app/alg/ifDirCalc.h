#ifndef _IFDIRCALC_H__
#define _IFDIRCALC_H__

#define MAXnAngle 128
#define UnitAngle 0.01
#define ifDirCircPitchMin 0.0
#define ifDirCircPitchMax 5.0
//#define ifDirCircPitchRange 10.0
#define ifDirCircPitchFrameMax 16
#define DirCircPitchMin 0.0
#define DirCircPitchMax 45.0
#define AzimuthBeamWidth 30.0
#define PitchBeamWidth 15.0
#define AzimuthScanWidth 360.0
//#define PitchScanWidth 45.0
#define PitchScanMin 0.0
#define PitchScanMax 30.0

//int ifDirCirc(int pitchNow, float freq);
int ifDirCircFreq(float freq);
int ifDirCircPitch(int *pitchDat, int nPitch);
int ifAzimuthEnough(int *azimuthDat, int nAzimuth, int *totalAzimuth);
int ifAzimuthAimed(int azimuthNow, int azimuthCen);
int ifPitchEnough(int *pitchDat, int nPitch);
//int ifPitchEnoughS(int *pitchDat, int nPitch);
int ifPitchAimed(int pitchNow, int pitchCen);

#endif


