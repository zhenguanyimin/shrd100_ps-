#include <stdlib.h>
#include "utils.h"
#include <time.h>
#include <math.h>

static float pAlgDataBuff1[2097152];
static float pAlgDataBuff2[2097152];
static float pAlgDataBuff3[2097152];

float ***createMat1(int m, int n, int k)
{
    float ***p = NULL;
    int count = 0;

    p = (float ***)(pAlgDataBuff1);
    count += m;
    for (int i = 0; i < m; i++)
    {
    	p[i] = (float **)(pAlgDataBuff1 + count);
    	count += n;
        for (int j = 0; j < n; j++)
        {
        	p[i][j] = (float *)(pAlgDataBuff1 + count);
        	count += k;
        }
    }
	while(count > 2097152);

    return p;
}

float ***createMat2(int m, int n, int k)
{
    float ***p = NULL;
    int count = 0;

    p = (float ***)(pAlgDataBuff2);
    count += m;
    for (int i = 0; i < m; i++)
    {
    	p[i] = (float **)(pAlgDataBuff2 + count);
    	count += n;
        for (int j = 0; j < n; j++)
        {
        	p[i][j] = (float *)(pAlgDataBuff2 + count);
        	count += k;
        }
    }
    while(count > 2097152);

    return p;
}
float ***createMat3(int m, int n, int k)
{
    float ***p = NULL;
    int count = 0;

    p = (float ***)(pAlgDataBuff3);
    count += m;
    for (int i = 0; i < m; i++)
    {
    	p[i] = (float **)(pAlgDataBuff3 + count);
    	 count += n;
        for (int j = 0; j < n; j++)
        {
        	p[i][j] = (float *)(pAlgDataBuff3 + count);
        	count += k;
        }
    }
    while(count > 2097152);

    return p;
}

//创建三维数组
float ***createMat(int m, int n, int k)
{
    float ***p = NULL;
    p = (float ***)malloc(m * sizeof(float **));
    for (int i = 0; i < m; i++)
    {
        p[i] = (float **)malloc(n * sizeof(float *));
        for (int j = 0; j < n; j++)
        {
            p[i][j] = (float *)malloc(k * sizeof(float));
        }
    }
    return p;
}
//释放三维数组
void freeMat(float ***p, int m, int n)
{
    if (p != NULL)
    {
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                free(p[i][j]);
            }
            free(p[i]);
        }
        free(p);
    }
}

//创建三维数组
unsigned short ***createUSMat(int m, int n, int k)
{
	unsigned short ***p = NULL;
	p = (unsigned short ***)malloc(m * sizeof(unsigned short **));
	for (int i = 0; i < m; i++)
	{
		p[i] = (unsigned short **)malloc(n * sizeof(unsigned short *));
		for (int j = 0; j < n; j++)
		{
			p[i][j] = (unsigned short *)malloc(k * sizeof(unsigned short));
		}
	}
	return p;
}

//释放三维数组
void freeUSMat(unsigned short ***p, int m, int n)
{
	if (p != NULL)
	{
		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < n; j++)
			{
				free(p[i][j]);
			}
			free(p[i]);
		}
		free(p);
	}
}

//二维卷积滤波。卷积核左右和上下对称，行数和列数且是奇数
void conv2(float **matIn, int m, int n, float **matOut, float *mask, int h, int w, int *nSpan)
{
    int i=0,j=0,p=0,q=0;
    int h_half=(h - 1) / 2;
    int w_half=(w - 1) / 2;
    int lcol=w_half,rcol=n-w_half;
    
    if(nSpan[0]>lcol)
        lcol=nSpan[0];
    if(nSpan[1]<rcol)
        rcol=nSpan[1]+1; // dingyu, 20230207
    
    for (i = h_half; i < m-h_half; i++)
    {
        for (j = lcol; j < rcol; j++)
        {
            *(*(matOut + i) + j) = *(*(matIn + i) + j)*mask[h_half*w+w_half];
            
            for(p=1;p<h_half+1;p++)
            {
                *(*(matOut + i) + j) += (*(*(matIn + i-p) + j) + *(*(matIn + i+p) + j))*mask[(h_half-p)*w+w_half];
            }
            
            for(q=1;q<w_half+1;q++)
            {
                *(*(matOut + i) + j) += (*(*(matIn + i) + j-q)+*(*(matIn + i) + j+q))*mask[h_half*w+w_half-q];
            }
            
            for(p=1;p<h_half+1;p++)
            {
                for(q=1;q<w_half+1;q++)
                {
                        *(*(matOut + i) + j) += (
                            *(*(matIn + i-p) + j-q)+
                            *(*(matIn + i+p) + j-q)+
                            *(*(matIn + i-p) + j+q)+
                            *(*(matIn + i+p) + j+q)
                            
                        ) * mask[(h_half-p)*w+w_half-q];
                }
            }
        }
    }
}

//幅度估计距离公式，miu根据实际情况调节。仅做估计用，由于电磁波衰减或者遮挡等不确定，实际可能会有较大出入
float amp2dist(float amp, float freq, float gain)
{
    //printf("amp=%.2f\n", amp);
    static float miu=26.0f;
    float r=pow(10.0, miu - amp / 512.0f);

    return r;
}

