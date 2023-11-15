#include "input.h"
#include "../../srv/log/log.h"

const  INPUT_ARG InputArg[OUTPUT_NAME_MAX] =
{

		[MIO_MUTE_SWITCH]		= { ENABLE, MIO, GPIO_DEVICE_ID, 0, 37},
		[MIO_SWITCH_MODE0]		= { ENABLE, MIO, GPIO_DEVICE_ID, 0, 38},
		[MIO_SWITCH_MODE1]		= { ENABLE, MIO, GPIO_DEVICE_ID, 0, 40},
		[MIO_SWITCH_MODE2]		= { ENABLE, MIO, GPIO_DEVICE_ID, 0, 43},
		[MIO_SWITCH_MODE3]		= { ENABLE, MIO, GPIO_DEVICE_ID, 0, 8},

		[EMIO_CONFIG_SW]         = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 43},
		[EMIO_GPS_SW]            = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 61},
		[EMIO_TRIGGER]           = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 67},
		[EMIO_VCC0_21V_OUT_INT]  = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 62},
		[EMIO_FAN_SENSOR]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 54},
		[EMIO_GYRO_INT2_DRDY]    = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 11},
//		[EMIO_GYRO_INT1]         = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 10},
//		[EMIO_ACC_MAGINT]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 23},
//		[EMIO_ACC_INT2_XL]       = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 22},
//		[EMIO_ACC_INT1_XL]       = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 21},
		[EMIO_WIFI_NET_STATUS]   = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 66},
		[EMIO_WIFI_WAKEUP_IN]    = { DISABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 64},
		[EMIO_WIFI_RX1]          = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 68},
		[EMIO_CTRL_OUT0]         = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 44},
		[EMIO_CTRL_OUT1]         = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 45},
		[EMIO_CTRL_OUT2]         = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 46},
		[EMIO_CTRL_OUT3]         = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 47},
		[EMIO_CTRL_OUT4]         = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 48},
		[EMIO_CTRL_OUT5]         = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 49},
//		[EMIO_CTRL_OUT6]         = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 50},
//		[EMIO_CTRL_OUT7]         = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 51},
//		[EMIO_GPS_PPS]           = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 8},
		[EMIO_VCC28V_PG_1V8]     = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 35},
		[EMIO_VCC32V_PG_1V8]     = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 34},
		[EMIO_VCC28V_PG_1V8]     = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 35},
		[EMIO_RF_1Q]     = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 78},//RF_GPIO_6
		[EMIO_RF_2Q]     = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 79},//RF_GPIO_7

};

/**
 * The output pin is initialized
 */
int32_t InputInit(void)
{
	int32_t Status = 0;
	uint32_t GpioVal;
	XGpioPs *pPsGpio;
	XGpio *pPlGpio;

	for(uint16_t i = 0; i < INPUT_NAME_MAX; i++)
	{
		if(InputArg[i].Enable == ENABLE)
		{
			if ((InputArg[i].GpioMode == MIO) || (InputArg[i].GpioMode == EMIO))
			{
				pPsGpio = GetPsGpioDev();
				XGpioPs_SetDirectionPin(pPsGpio, InputArg[i].Pin, INPUT);
			}
			else if (InputArg[i].GpioMode == AXI_GPIO)
			{
				pPlGpio = GetPlGpioDev(InputArg[i].DeviceId);
				if (pPlGpio->IsReady == 0)
				{
					Status = XGpio_Initialize(pPlGpio, InputArg[i].DeviceId);
					if (Status != XST_SUCCESS)
					{
						LOG_ERROR("AXI GPIO config failed!\r\n");
						break;
					}
				}
				GpioVal = XGpio_GetDataDirection(pPlGpio, InputArg[i].AxiGpioChl);
				GpioVal = GpioVal | (1 << InputArg[i].Pin);
				XGpio_SetDataDirection(pPlGpio, InputArg[i].AxiGpioChl, GpioVal);//channel x xbit output
			}
			else if (InputArg[i].GpioMode == AXI_BRAM)
			{
			}
		}
		uint8_t aa = Input_GetValue(i);
		uint16_t ii = 0;
	}

	return Status;
}





/**
 * 读取引脚电平
 * @param name 引脚名字
 * @return	1: 高电平     0: 低电平
 */
uint8_t Input_GetValue(INPUT_NAME name)
{
//	return (uint8_t)axi_read_data(name);
	uint32_t GpioVal;
	uint8_t i = (uint8_t)name;
	uint8_t status;

	XGpioPs *pPsGpio;
	XGpio *pPlGpio;

	if(InputArg[i].Enable == ENABLE)
	{
		if ((InputArg[i].GpioMode == MIO) || (InputArg[i].GpioMode == EMIO))
		{
			pPsGpio = GetPsGpioDev();
			status = (uint8_t)XGpioPs_ReadPin(pPsGpio, InputArg[i].Pin);
		}
		else if (InputArg[i].GpioMode == AXI_GPIO)
		{
			pPlGpio = GetPlGpioDev(InputArg[i].DeviceId);
			GpioVal = XGpio_DiscreteRead(pPlGpio, InputArg[i].AxiGpioChl);
			GpioVal = GpioVal & (1 << InputArg[i].Pin);
			status = (GpioVal == 0);
		}
		else if (InputArg[i].GpioMode == AXI_BRAM)
		{
			status = (uint8_t)axi_read_data(InputArg[i].Pin);
		}
	}

	return status;
}
