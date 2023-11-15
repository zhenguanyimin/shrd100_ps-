#ifndef _DRONEID_SYSTEM_PARAM_H_
#define _DRONEID_SYSTEM_PARAM_H_

/* Standard Include Files. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "xtime_l.h"

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
#define EXTRACT_BURST_MODULE_EN		0 // 0:disable, 1:enable

#define DEFAULT_FREQ_OFFSET			(-0.2004f)
#define DEFAULT_SAMPLE_RATE			(30720000.0f)

#define READ_DDR_LEN 			 	(0x10000000)// 0x10000000:256MB, 0x8000000:128MB, 0x800000:8MB,
#define LENGTH_ADC_DATA 		 	(READ_DDR_LEN / 8)	//(1024u)
#define SIZE_FFT                 	(1024u * 2)
#define NUM_SYMBOL_PER_BURST     	(9u)
#define NUM_USE_SYMBOL_PER_BURST 	(6u)
#define LENGTH_LONG_CP           	(80u * 2)
#define LENGTH_SHORT_CP          	(72u * 2)
#define NUM_LONG_CP              	(2u)
#define NUM_SHORT_CP             	(7u)
#define NUM_DATA_CARRIER         	(600u)
#define NUM_FILTER_TAP           	(50u)
#define NUM_PADDING_SAMPLE  	 	(50u * 2 * 2)
#define LENGTH_SUBCARRIER_BUFF   	(NUM_SYMBOL_PER_BURST * SIZE_FFT)
#define LENGTH_DATA_CARRIER_BUFF 	(NUM_USE_SYMBOL_PER_BURST * NUM_DATA_CARRIER)
#define LENGTH_DEMAP_OUT_BIT  	 	(7200u)
#define LENGTH_BURST             	((NUM_SYMBOL_PER_BURST * SIZE_FFT) + \
		(NUM_LONG_CP * LENGTH_LONG_CP) + (NUM_SHORT_CP * LENGTH_SHORT_CP))
#define LENGTH_PADDED_BURST  	 	(LENGTH_BURST + NUM_PADDING_SAMPLE)
#define LENGTH_LFSR				 	(1600 + LENGTH_DEMAP_OUT_BIT + 31)

#define MAX_NUM_DRONE_ID		 	(10u)	//(5u)

#define LENGTH_DECODE_OUT_BYTE   	(176u)
#define LENGTH_DECODE_OUT_BIT   	(176u * 8)
#define LENGTH_DECODE_CRC16		   	(89u)
#define LENGTH_CRC16_TABLE   		(256u)
#define LENGTH_CRC24A_TABLE   		(256u)
#define LENGTH_CRC24A_POLY			(25u)
#define LENGTH_CRC24A_FOR_LOOP		(LENGTH_DECODE_OUT_BIT - LENGTH_CRC24A_POLY)
#define NUM_FIELD_IN_PACKET      	(23u)
#define NUM_WIND_MV_AVG		      	(3u)
#define LENGTH_STORE_PATCH			(119u)

#define ADC_REAL_OFFSET				(0u)							    // 0bit offset
#define ADC_IMAG_OFFSET				(16u)							    // 16bit offset
#define ADC_REAL_MASK				(0x0FFFu)							// 12bit[11:0] valid
#define ADC_IMAG_MASK				(0x0FFFu)							// 12bit[27:16] valid
#define ADC_DATA_BIT				(12u)								// 12bit adc data valid
#define ADC_DATA_BIT_SCOPE			(1 << ADC_DATA_BIT)					// 2^12 = 4096
#define ADC_DATA_MIN				(-1 * (1 << (ADC_DATA_BIT - 1))) 	// -2^(12-1) = -2048
#define ADC_DATA_MAX				((1 << (ADC_DATA_BIT - 1)) - 1)		// 2^(12-1)-1 = 2047

#ifndef PI_VALUE
#define PI_VALUE					(M_PI)
#endif
#ifndef DOUBLE_PI_VALUE
#define DOUBLE_PI_VALUE				(2 * PI_VALUE)
#endif
#ifndef HALF_PI_VALUE
#define HALF_PI_VALUE				(M_PI_2)
#endif

#define EARTH_RADIUS 				(6378.137f)	// the radius of earth
#define PI 							(3.14159265358979323846f)	// pi
#define INV_SQRT_2					(M_SQRT1_2)	// 1/(sqrt(2))


/*=================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/

/* Structure */

/*!
 *  @brief    structure for droneID signal 
 *
 */
typedef struct DroneID_Signal
{
    uint8_t     hex_symbol; 
    uint16_t    packet_type;   
    uint16_t    squence_num;     
    uint16_t    state_info;      
    uint8_t     serial_num[16];  
    int32_t     drone_longtitude;
    int32_t     drone_latitude;  
    int16_t     altitude;        
    int16_t     height;          
    int16_t     x_speed;         
    int16_t     y_speed;         
    int16_t     z_speed;         
    int16_t     yaw_angle;       
    uint64_t    pilot_gps_clock; 
    int32_t     pilot_latitude;  
    int32_t     pilot_longitude; 
    int32_t     home_longitude;  
    int32_t     home_latitude;   
    uint8_t     product_type;    
    uint8_t     uuid_len;        
    uint8_t     uuid[18];          
    uint8_t     zero_symbol;
    uint8_t     crc[3];
} DroneID_Signal_t;

typedef struct DroneID_ParseInfo
{
	uint8_t     type_num;
    uint8_t     type_name[25];
    uint8_t     serial_num[32];
    int32_t     drone_longtitude;
    int32_t     drone_latitude;
    int16_t     height;
    int16_t     yaw_angle;
    int16_t     speed;
    int16_t     x_speed;
    int16_t     y_speed;
    int16_t     z_speed;
    int32_t     pilot_longitude;
    int32_t     pilot_latitude;
    uint16_t    danger_levels;
    uint64_t    drone_Freq;	/* droneid signal frequence, unit Hz */
} DroneID_ParseInfo_t;

typedef struct DroneID_ParseAllInfo
{
	uint32_t	aliveCnt;
	uint32_t	lostCnt;
	int32_t		lostDuration;	// unit = 1 ms
	int32_t		lastUpdateTime;	// unit = 1 ms
	int32_t		removeThreLostDur;	// unit = 1 ms
	DroneID_ParseInfo_t 	droneID;
} DroneID_ParseAllInfo_t;

typedef struct DroneID_OutputInfoList
{
	uint32_t DroneIdNum;
	uint32_t DroneIdPredictCnt[MAX_NUM_DRONE_ID];
	DroneID_ParseAllInfo_t 	DroneIdOutputInfoList[MAX_NUM_DRONE_ID];
} DroneID_OutputInfoList_t;

typedef struct Cplxf
{
    float re;
    float im;
} Cplxf_t;

typedef struct Cplxint32
{
    int32_t re;
    int32_t im;
} Cplxint32_t;


/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/


#endif /* _DRONEID_SYSTEM_PARAM_H_ */
