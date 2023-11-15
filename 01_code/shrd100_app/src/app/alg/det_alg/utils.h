#ifndef _UTILS_H__
#define _UTILS_H__


float ***createMat(int m, int n, int k);
void freeMat(float ***p, int m, int n);
void conv2(float **matIn, int m, int n, float **matOut, float *mask, int h, int w, int *nSpan);
float amp2dist(float amp, float freq, float gain);

float ***createMat1(int m, int n, int k);
float ***createMat2(int m, int n, int k);
float ***createMat3(int m, int n, int k);

unsigned short ***createUSMat(int m, int n, int k);
void freeUSMat(unsigned short ***p, int m, int n);

#endif

