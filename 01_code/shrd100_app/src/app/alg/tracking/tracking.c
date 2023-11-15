/*
 * tracking.c
 *
 *  Created on: 2023.07.02
 *      Author: A21157
 */

/* Standard Include Files. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "xtime_l.h"

#include "tracking.h"
#include "../droneID/droneID_main.h"
#include "../../../srv/log/log.h"


/**
 *  @description	tracking predict
 *
 *  @param[in]		pDroneList
 *  @param[out]		pDroneListPredict
 *
 *  @return  		null
 */
void TrackingPredict_DroneID(DroneID_OutputInfoList_t *pDroneIdList, DroneID_OutputInfoList_t *pDroneIdListPredict)
{
	int32_t i = 0;

	for (i = 0; i < pDroneIdListPredict->DroneIdNum; i++)
	{
		if (pDroneIdListPredict->DroneIdOutputInfoList[i].lostDuration == 0)
		{
			pDroneIdListPredict->DroneIdPredictCnt[i] = 0;
		}
		else
		{
			PredictPerDroneID(i, &pDroneIdList->DroneIdOutputInfoList[i], &pDroneIdListPredict->DroneIdOutputInfoList[i],
				pDroneIdListPredict->DroneIdPredictCnt[i]);
			pDroneIdListPredict->DroneIdPredictCnt[i]++;
		}
	}
}


void PredictPerDroneID(uint32_t droneIndex, DroneID_ParseAllInfo_t *pDroneIdInfoOrig, DroneID_ParseAllInfo_t *pDroneIdInfoPredict, uint32_t predictCnt)
{
	float x_speed = 0.0f, y_speed = 0.0f, z_speed = 0.0f;
	float droneLong = 0.0f, droneLat = 0.0f, droneHeight = 0.0f;
	float pilotLong = 0.0f, pilotLat = 0.0f, pilotHeight = 0.0f;
	float droneLongPredict = 0.0f, droneLatPredict = 0.0f, droneHeightPredict = 0.0f;
	float xEast = 0.0f, yNorth = 0.0f, zUp = 0.0f;

	x_speed = pDroneIdInfoOrig->droneID.x_speed / 1.e2,
	y_speed = pDroneIdInfoOrig->droneID.y_speed / 1.e2,
	z_speed = pDroneIdInfoOrig->droneID.z_speed / 1.e2,
	droneLong = pDroneIdInfoOrig->droneID.drone_longtitude / 1.e7;
	droneLat = pDroneIdInfoOrig->droneID.drone_latitude / 1.e7;
	droneHeight = pDroneIdInfoOrig->droneID.height / 1.e1;
	pilotLong = pDroneIdInfoOrig->droneID.pilot_longitude / 1.e7;
	pilotLat = pDroneIdInfoOrig->droneID.pilot_latitude / 1.e7;
	pilotHeight = 0;

	GeoDetic_TO_ENU(droneLat, droneLong, droneHeight, pilotLat, pilotLong, pilotHeight, &xEast, &yNorth, &zUp);

	xEast += (y_speed * DEFAULT_DT_S * DEFAULT_V_VARI_COEF * predictCnt);
	yNorth += (x_speed * DEFAULT_DT_S * DEFAULT_V_VARI_COEF * predictCnt);
	zUp += (z_speed * DEFAULT_DT_S * DEFAULT_V_VARI_COEF * predictCnt);

	ENU_TO_GeoDetic(xEast, yNorth, zUp, pilotLat, pilotLong, pilotHeight, &droneLatPredict, &droneLongPredict, &droneHeightPredict);

	if (droneHeightPredict < 0.0f)
	{
		droneHeightPredict = 0.0f;
	}

	pDroneIdInfoPredict->droneID.drone_longtitude = (int32_t)(droneLongPredict * 1.e7);
	pDroneIdInfoPredict->droneID.drone_latitude = (int32_t)(droneLatPredict * 1.e7);
	pDroneIdInfoPredict->droneID.height = (int16_t)(droneHeightPredict * 1.e1);

	if (PRINT_MULTI_DRONEID == GetAlgDebugPrintMode())
	{
		LOG_INFO("[%d] predict info : type:%d(%s), sn:%s, dLng:%.7f, dLat:%.7f, h:%.2f, "
			"yaw:%.2f, spd:%.2f, spdx:%.2f, spdy:%.2f, spdz:%.2f, pLng:%.7f, pLat:%.7f\r\n",
			droneIndex,
			pDroneIdInfoPredict->droneID.type_num,
			pDroneIdInfoPredict->droneID.type_name,
			pDroneIdInfoPredict->droneID.serial_num,
			droneLongPredict / M_PI * 180,
			droneLatPredict / M_PI * 180,
			droneHeightPredict,
			pDroneIdInfoPredict->droneID.yaw_angle / 1.e2,
			pDroneIdInfoPredict->droneID.speed / 1.e2,
			x_speed,
			y_speed,
			z_speed,
			pilotLong / M_PI * 180,
			pilotLat / M_PI * 180);
	}
}


/**
 *  @description	地理坐标系转东北天坐标系，输入为目标的经纬高，局部坐标系原点的经纬高，输出为东北天的x y z
 *
 *  @param[in]		lat		in radian
 *  @param[in]		lon		in radian
 *  @param[in]		h		in m
 *  @param[in]		lat0	in radian
 *  @param[in]		lon0	in radian
 *  @param[in]		h0		in m
 *  @param[out]		xEast	in m
 *  @param[out]		yNorth	in m
 *  @param[out]		zUp		in m
 *
 *  @return  		null
 */
void GeoDetic_TO_ENU(float lat, float lon, float h, float lat0, float lon0, float h0, float *xEast, float *yNorth, float *zUp)
{
	float lamb = 0.f, phi = 0.f, s = 0.f, N = 0.f;
	float sin_lambda = 0.f, cos_lambda = 0.f, sin_phi = 0.f, cos_phi = 0.f;
	float x = 0.f, y = 0.f, z = 0.f;
	float lamb0 = 0.f, phi0 = 0.f, s0 = 0.f, N0 = 0.f;
	float sin_lambda0 = 0.f, cos_lambda0 = 0.f, sin_phi0 = 0.f, cos_phi0 = 0.f;
	float x0 = 0.f, y0 = 0.f, z0 = 0.f;
	float xd = 0.f, yd = 0.f, zd = 0.f, t = 0.f;

	lamb = lat;
	phi = lon;
	s = sinf(lamb);
	N = DEFAULT_A / sqrtf(1 - (DEFAULT_E_SQ * s * s));

	sin_lambda = sinf(lamb);
	cos_lambda = cosf(lamb);
	sin_phi = sinf(phi);
	cos_phi = cosf(phi);

	x = (h + N) * cos_lambda * cos_phi;
	y = (h + N) * cos_lambda * sin_phi;
	z = (h + (1 - DEFAULT_E_SQ) * N) * sin_lambda;

	lamb0 = lat0;
	phi0 = lon0;
	s0 = sinf(lamb0);
	N0 = DEFAULT_A / sqrtf(1 - DEFAULT_E_SQ * s0 * s0);

	sin_lambda0 = sinf(lamb0);
	cos_lambda0 = cosf(lamb0);
	sin_phi0 = sinf(phi0);
	cos_phi0 = cosf(phi0);

	x0 = (h0 + N0) * cos_lambda0 * cos_phi0;
	y0 = (h0 + N0) * cos_lambda0 * sin_phi0;
	z0 = (h0 + (1 - DEFAULT_E_SQ) * N0) * sin_lambda0;

	xd = x - x0;
	yd = y - y0;
	zd = z - z0;
	t = -cos_phi0 * xd - sin_phi0 * yd;

	*xEast = -sin_phi0 * xd + cos_phi0 * yd;
	*yNorth = t * sin_lambda0 + cos_lambda0 * zd;
	*zUp = cos_lambda0 * cos_phi0 * xd + cos_lambda0 * sin_phi0 * yd + sin_lambda0 * zd;
}


/**
 *  @description	东北天坐标系转地理坐标系，输入为目标在东北天中的xyz，原点的经纬高，输出为目标的经纬高
 *
 *  @param[in]		xEast	in m
 *  @param[in]		yNorth	in m
 *  @param[in]		zUp		in m
 *  @param[in]		lat0	in radian
 *  @param[in]		lon0	in radian
 *  @param[in]		h0		in m
 *  @param[out]		lat		in radian
 *  @param[out]		lon		in radian
 *  @param[out]		h		in m
 *
 *  @return  		null
 */
void ENU_TO_GeoDetic(float xEast, float yNorth, float zUp, float lat0, float lon0, float h0, float *lat, float *lon, float *h)
{
	float x = 0.f, y = 0.f, z = 0.f;

	ENU_TO_ECEF(xEast, yNorth, zUp, lat0, lon0, h0, &x, &y, &z);
	ECEF_TO_GeoDetic(x, y, z, lat, lon, h);
}


/**
 *  @description	东北天坐标系转地心地固坐标系，输入为目标在东北天中的xyz，以及东北天原点的经纬高，输出为输出为地心地固的x y z
 *
 *  @param[in]		xEast	in m
 *  @param[in]		yNorth	in m
 *  @param[in]		zUp		in m
 *  @param[in]		lat0	in radian
 *  @param[in]		lon0	in radian
 *  @param[in]		h0		in m
 *  @param[out]		x		in m
 *  @param[out]		y		in m
 *  @param[out]		z		in m
 *
 *  @return  		null
 */
void ENU_TO_ECEF(float xEast, float yNorth, float zUp, float lat0, float lon0, float h0, float *x, float *y, float *z)
{
	float lamb = 0.f, phi = 0.f, s = 0.f, N = 0.f;
	float sin_lambda = 0.f, cos_lambda = 0.f, sin_phi = 0.f, cos_phi = 0.f;
	float x0 = 0.f, y0 = 0.f, z0 = 0.f, t = 0.f, xd = 0.f, yd = 0.f, zd = 0.f;

	lamb = lat0;
	phi = lon0;
	s = sinf(lamb);
	N = DEFAULT_A / sqrtf(1 - DEFAULT_E_SQ * s * s);

	sin_lambda = sinf(lamb);
	cos_lambda = cosf(lamb);
	sin_phi = sinf(phi);
	cos_phi = cosf(phi);

	x0 = (h0 + N) * cos_lambda * cos_phi;
	y0 = (h0 + N) * cos_lambda * sin_phi;
	z0 = (h0 + (1 - DEFAULT_E_SQ) * N) * sin_lambda;

	t = cos_lambda * zUp - sin_lambda * yNorth;

	zd = sin_lambda * zUp + cos_lambda * yNorth;
	xd = cos_phi * t - sin_phi * xEast;
	yd = sin_phi * t + cos_phi * xEast;

	*x = xd + x0;
	*y = yd + y0;
	*z = zd + z0;
}


/**
 *  @description	地心地固转地理坐标系，输入为地心地固x y z，输出为地理的经纬高
 *
 *  @param[in]		x		in m
 *  @param[in]		y		in m
 *  @param[in]		z		in m
 *  @param[out]		lat		in radian
 *  @param[out]		lon		in radian
 *  @param[out]		h		in m
 *
 *  @return  		null
 */
void ECEF_TO_GeoDetic(float x, float y, float z, float *lat, float *lon, float *h)
{
	float c = 0.f, e = 0.f, ep = 0.f, r = 0.f, s = 0.f, r2 = 0.f;
	float F = 0.f, G = 0.f, P = 0.f, Q = 0.f, U = 0.f, V = 0.f;
	float ro = 0.f, zo = 0.f, tmp = 0.f, temp = 0.f;
	float x2 = 0.f, y2 = 0.f, z2 = 0.f;

	x2 = x * x;
	y2 = y * y;
	z2 = z * z;

	e =  sqrtf(DEFAULT_E2);
	ep = e * (DEFAULT_A / DEFAULT_B);
	r2 = x2 + y2;
	r =  sqrtf(r2);
	F = 54 * DEFAULT_B2 * z2;
	G = r2 + (1 - DEFAULT_E2) * z2 - DEFAULT_E2 * DEFAULT_E2D;
	c = (DEFAULT_E4 * F * r2) / (G * G * G);
	s = powf((1 + c + sqrtf(c * c + 2 * c)), (1 / 3));
	P = F / (3 * powf((s + 1/s + 1), 2) * G * G);
	Q =  sqrtf(1 + 2 * DEFAULT_E4 * P);
	ro = -(P*DEFAULT_E2*r)/(1+Q) + sqrtf((DEFAULT_A2/2)*(1+1/Q) - (P*(1-DEFAULT_E2)*z2)/(Q*(1+Q)) - P*r2/2);
	tmp = powf((r - DEFAULT_E2 * ro), 2);
	U =  sqrtf(tmp + z2);
	V =  sqrtf(tmp + (1 - DEFAULT_E2) * z2);
	zo = (DEFAULT_B2 * z) / (DEFAULT_A * V);

	*h = U * (1 - DEFAULT_B2 / (DEFAULT_A * V));

	*lat = atanf((z + ep * ep * zo) / r);

	temp = atanf(y / x);
	if (x >= 0)
	{
		*lon = temp;
	}
	else
	{
		if ((x < 0) & (y >= 0))
		{
			*lon = M_PI + temp;
		}
		else
		{
			*lon = temp - M_PI;
		}
	}
}

