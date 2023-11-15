#ifndef DEV_INFO_PROCESS_H
#define DEV_INFO_PROCESS_H


#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "xtime_l.h"
#include "xscugic.h"
#include "../../srv/protocol/protocol_system.h"


enum DEV_INFO {
	DEV_INFO_PRINTF_GPS = 0,
	DEV_INFO_PRINTF_GPS_STR,
};
typedef struct imu_info {

	float roll;
	float pitch;
	float yaw;
}imu_info_t;


extern XScuGic xInterruptController;

extern SemaphoreHandle_t imu_timer_Sem;

extern uint8_t  GetGyroStatus(void);
extern void init_device_info();
extern int32_t InitImuJy901Process_Task();
extern void ImuProc(uint8_t cRxedChar);
void SetPrintGpsInfoFlag(uint8_t value);
void GetAngleInfo(float *roll, float *pitch, float *yaw);

#endif
