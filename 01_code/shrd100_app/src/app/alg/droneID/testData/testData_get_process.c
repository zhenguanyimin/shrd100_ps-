//
///* Standard Include Files. */
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <stdint.h>
//#include <string.h>
//
//#include "testData_get_process.h"
//
//
//float testData_adc_real[LENGTH_ADC_DATA] =
//{
//	#include "testData_adc_real.txt"
//};
//
//float testData_adc_imag[LENGTH_ADC_DATA] =
//{
//	#include "testData_adc_imag.txt"
//};
//
//float testData_burst_real[LENGTH_PADDED_BURST] =
//{
//	#include "testData_burst_real.txt"
//};
//
//float testData_burst_imag[LENGTH_PADDED_BURST] =
//{
//	#include "testData_burst_imag.txt"
//};
//
//int8_t testData_bits[LENGTH_DEMAP_OUT_BIT] =
//{
//	#include "testData_bits.txt"
//};
//
//
//void GetTestDataAdc(Cplxf_t *fAdcData, Cplxint32_t *iAdcData)
//{
//	int32_t i = 0;
//	for (i = 0; i < LENGTH_ADC_DATA; i++)
//	{
//		fAdcData[i].re = testData_adc_real[i];
//		fAdcData[i].im = testData_adc_imag[i];
//
//		if (fAdcData[i].re > 0.0f)
//		{
//			iAdcData[i].re = 1;
//		}
//		else
//		{
//			iAdcData[i].re = -1;
//		}
//
//		if (fAdcData[i].im > 0.0f)
//		{
//			iAdcData[i].im = 1;
//		}
//		else
//		{
//			iAdcData[i].im = -1;
//		}
//	}
//}
////void GetTestDataAdc(Cplxf_t *adcData)
////{
////	int32_t i = 0;
////	for (i = 0; i < LENGTH_ADC_DATA; i++)
////	{
////		adcData[i].re = testData_adc_real[i];
////		adcData[i].im = testData_adc_imag[i];
////	}
////}
//
//void GetTestDataBurst(Cplxf_t *padBurst)
//{
//	int32_t i = 0;
//	for (i = 0; i < LENGTH_PADDED_BURST; i++)
//	{
//		padBurst[i].re = testData_burst_real[i];
//		padBurst[i].im = testData_burst_imag[i];
//	}
//}
//
//void GetTestDataBits(int8_t *data)
//{
//	memcpy(data, testData_bits, sizeof(int8_t) * LENGTH_DEMAP_OUT_BIT);
//}
