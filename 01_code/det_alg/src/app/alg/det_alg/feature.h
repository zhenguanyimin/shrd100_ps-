#ifndef _FEATURE_H__
#define _FEATURE_H__

#define maxPulseWNumInLib 5  //

//�����⣬�����ֶν���ͼ����Ч�������ֶν���ң����Ч����������ֲ�Ϊ����������
struct DroneLib
{
    char  name[50];					//����
	int   psbID[10];				//���ܵĻ������
	int   nPsbID;					//������ſ���������
    char  downOrUp;					//ͼ����ң�أ�����down��ͼ��������up��ң��
    float freqPoints[60];			//Ƶ�㡣��Ӧͼ�����ŵ���ң�ص���Ƶ�㣬MHz
    int   nfreq;					//Ƶ�����
    char  isFixedFreq;				//freqPoints����Ƶ�η�Χ����Ƶ�㣬����0��ʾ��ӦƵ�������ޣ�����1���ӦƵ��
    int   hoppType;					//��Ƶ���ͣ��ò�������ң����Ч��0�ϸ���Ƶ������Ӧʱ����ӦƵ�㷢���źţ�1����������Ƶ�Ҵ󲿷�Ƶ�ʲ��䣬2ֻ��Ҫ������Ƶ����
    float pulseW[maxPulseWNumInLib];//������������ms��
    int   nPulseW;					//�����������
    char  isFixedPulseW;			//�����Ƿ�̶���0���̶���1�̶�
    float pulseT[5];				//���ڣ�ms
    int   nPulseT;					//��������
    float pulseBW[5];				//�����С��MHz
    int   nPulseBW;					//��������
    float pulseWErr[maxPulseWNumInLib];//������������ޣ�ms
    int   meetPulseW[maxPulseWNumInLib];//���������ȵ���ʹ���
    float pulseTErr;				//����ʱ���������ms
    int   hoppCnt;					//���������Ƶ����ʹ���
    float freqErr;					//Ƶ���������,MHz
    int   method;					//ʹ�÷�����0��Ӧͼ����1��Ӧң��
    float SNR;						//�����������ޣ�dB
    float duty;						//�ŵ�ռ�ձ�
};

#endif

