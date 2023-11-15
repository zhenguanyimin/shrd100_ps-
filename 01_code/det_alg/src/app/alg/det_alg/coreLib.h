#ifndef _CORELIB_H__
#define _CORELIB_H__
#include "feature.h"
#include "upLoadSniffer.h"
#include "math.h"

#define MaxDeepCount 500
#define NCh 1
#define MaxPulseParam 2000
#define MaxCtrPulse 2000
#define MaxNumCtrT 100



float getThresh(float *s, int n, float dT, int maxIter);
int threshSegm(float *s, int n, float thresh, int delt, int minw, int *q1, int *w, float *colSnr);
void matchPulseT(int **match, float *t2, int *index, int n, float *pulseT, int nPulseT, float pulseTErr);

char search(float **mat, int ix, int iy, int **matMark, int label, int nRows, int nCols, int *count, float ctrSNR);
int markD(float **mat, int **matMark, int nRows, int nCols, int n1, int n2, float ctrSNR);
int mark2p(struct ctrPulse *pulse, int **matMark, float ***specMat, int nRows, int nCols, int n1, int n2, int label, float dt, float dF, int antIndex, float cenFreq);
char isMatch(float t1, int nt1, int t1Index, float t2, int nt2, int t2Index, float *pulseT, int nPulseT, float pulseTErr);

int getLib(struct UAVLib **UAVtypes);

#endif

