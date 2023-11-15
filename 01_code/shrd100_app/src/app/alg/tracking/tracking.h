/*
 * tracking.h
 *
 *  Created on: 2023.07.02
 *      Author: A21157
 */

#ifndef _TRACKING_H_
#define _TRACKING_H_

#include "../droneID/droneID_system_param.h"
#include "../droneID/droneID_utils.h"
#include <math.h>


/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
#define DEFAULT_DT_S			(0.65f)	/* default delta time, unit = 1s */
#define DEFAULT_LONG_VARI_DEG	(0.000002f)	/* default longitude variation in delta time, unit = 1degree */
#define DEFAULT_LAT_VARI_DEG	(0.000008f)	/* default latitude variation in delta time, unit = 1degree */
#define DEFAULT_A 				(6378137.0)	/* earth radius in meters */
#define DEFAULT_B 				(6356752.3142)	/* earth semiminor in meters */
#define DEFAULT_F 				((DEFAULT_A - DEFAULT_B) / DEFAULT_A)
#define DEFAULT_E_SQ 			(DEFAULT_F * (2 - DEFAULT_F))
#define DEFAULT_A2 				(DEFAULT_A * DEFAULT_A)
#define DEFAULT_B2 				(DEFAULT_B * DEFAULT_B)
#define DEFAULT_E2				(1 - (DEFAULT_B / DEFAULT_A ) * (DEFAULT_B / DEFAULT_A))
#define DEFAULT_E4				(DEFAULT_E2 * DEFAULT_E2)
#define DEFAULT_E2D				(DEFAULT_A2 - DEFAULT_B2)
#define DEFAULT_V_VARI_COEF		(0.5f)	/* default velocity variation coefficient, unit = 1 */


/*=================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/


/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/
void TrackingPredict_DroneID(DroneID_OutputInfoList_t *pDroneList, DroneID_OutputInfoList_t *pDroneListPredict);
void PredictPerDroneID(uint32_t droneIndex, DroneID_ParseAllInfo_t *pDroneIdInfoOrig, DroneID_ParseAllInfo_t *pDroneIdInfoPredict, uint32_t predictCnt);
void GeoDetic_TO_ENU(float lat, float lon, float h, float lat0, float lon0, float h0, float *xEast, float *yNorth, float *zUp);
void ENU_TO_GeoDetic(float xEast, float yNorth, float zUp, float lat0, float lon0, float h0, float *lat, float *lon, float *h);
void ENU_TO_ECEF(float xEast, float yNorth, float zUp, float lat0, float lon0, float h0, float *x, float *y, float *z);
void ECEF_TO_GeoDetic(float x, float y, float z, float *lat, float *lon, float *h);


#endif /* _TRACKING_H_ */
