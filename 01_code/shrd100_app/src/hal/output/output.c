#include "output.h"
#include "../../srv/log/log.h"
#define PL_DEVICE_MAX		10
static XGpioPs PsGpio;
static XGpio PlGpio[PL_DEVICE_MAX] = {0};  /* The Instance of the AXI GPIO Driver */

XGpioPs* GetPsGpioDev(void)
{
	return &PsGpio;
}

XGpio* GetPlGpioDev(uint8_t id)
{
	return &PlGpio[id];
}

const  OUTPUT_ARG OutputArg[OUTPUT_NAME_MAX] =
{

#if RF_T3 == 1
		[EMIO_RF_EN]     		= { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 76, IO_VOH},//RF_GPIO_4
		[EMIO_RF_RESET]     	= { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 77, IO_VOL},//RF_GPIO_5
		[EMIO_RF_CTLA_1]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 1, IO_VOL},//RF_CTLA_0
		[EMIO_RF_CTLA_2]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 2, IO_VOL},//RF_CTLA_1
		[EMIO_RF_CTLA_3]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 3, IO_VOL},//RF_CTLA_2
		[EMIO_RF_CTLB_1]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 4, IO_VOL},//RF_CTLA_3
		[EMIO_RF_CTLB_2]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 5, IO_VOL},//RF_CTLB_0
		[EMIO_RF_CTLB_3]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 6, IO_VOL},//RF_CTLB_1
		[EMIO_RF_CTLC_1]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 18, IO_VOL},//RF_GPIO_0
		[EMIO_RF_CTLC_2]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 73, IO_VOL},//RF_GPIO_1
		[EMIO_RF_CTLC_3]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 7, IO_VOL},//RF_CTLB_2
		[EMIO_RF_CTLD_1]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 8, IO_VOL},//RF_CTLB_3
		[EMIO_RF_CTLD_2]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 74, IO_VOL},//RF_GPIO_2
		[EMIO_RF_CTLD_3]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 75, IO_VOL},//RF_GPIO_3
#else
		[EMIO_RF_CTLA_0]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 1, IO_VOH},
		[EMIO_RF_CTLA_1]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 2, IO_VOH},
		[EMIO_RF_CTLA_2]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 3, IO_VOH},
		[EMIO_RF_CTLA_3]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 4, IO_VOH},
		[EMIO_RF_CTLB_0]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 5, IO_VOH},
		[EMIO_RF_CTLB_1]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 6, IO_VOH},
		[EMIO_RF_CTLB_2]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 7, IO_VOH},
		[EMIO_RF_CTLB_3]        = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 8, IO_VOH},
#endif

		[EMIO_SWITCH_5V_EN]   	= { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 9, IO_VOH},

		[EMIO_SYS_LED2]       	= { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 16, IO_VOH},
		[EMIO_SYS_LED3]       	= { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 15, IO_VOL},

		[EMIO_FAN_EN]        	= { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 17, IO_VOH},
		[EMIO_BUZZER_EN]     	= { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 18, IO_VOL},
		[EMIO_VBRATOR_EN]    	= { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 19, IO_VOL},

		[EMIO_RG450_BT_EN]    	= { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 21, IO_VOH},
		[EMIO_VCC_GYRO_EN]    	= { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 27, IO_VOH},

		[MIO_PS_WD]           = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 6, IO_VOL},
		[MIO_PS_ENET_RESET]   = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 9, IO_VOH},
		[MIO_PS_USB_RESET]    = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 10, IO_VOH},
		[MIO_PS_USB_CS]       = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 11, IO_VOH},
		[MIO_EMMC0_SD0_RSTN]  = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 23, IO_VOH},
		[MIO_V3_1]            = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 26, IO_VOL},
		[MIO_V2_1]            = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 27, IO_VOL},
		[MIO_V1_1]            = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 28, IO_VOL},
		[MIO_V3_2]            = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 29, IO_VOL},
		[MIO_V2_2]            = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 30, IO_VOL},
		[MIO_V1_2]            = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 31, IO_VOL},
		[MIO_GPIO0_DISPLAY]   = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 36, IO_VOL},
		[MIO_GPIO1_DISPLAY]   = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 37, IO_VOL},
		[MIO_USB_5V_EN]       = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 38, IO_VOH},
		[MIO_VBUS_ON]            = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 39, IO_VOH},
		[MIO_EN_2]            = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 40, IO_VOH},
		[MIO_ETH_VCC_1V2_EN]  = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 41, IO_VOH},
		[MIO_VDD_3V3_EN]      = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 42, IO_VOH},
		[MIO_VDD_1V8_EN]      = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 43, IO_VOH},
		[MIO_VCC_5V_EN]       = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 44, IO_VOH},
		[MIO_SYS_LED0]        = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 24, IO_VOH},
		[MIO_SYS_LED1]        = { ENABLE, MIO, GPIO_DEVICE_ID, 0, 25, IO_VOL},

		[EMIO_WIFI_WAKEUP_IN_OUT]    = { ENABLE, EMIO, GPIO_DEVICE_ID, 0, EMIO_OFFSET + 64, IO_VOL},
};

/**
 * The output pin is initialized
 */
int32_t OutputInit(void)
{
	int32_t Status;
	XGpioPs_Config *ConfigPtr;
	uint32_t GpioVal;
	uint8_t PlId;

	ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(&PsGpio, ConfigPtr, ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		LOG_ERROR("MIO EMIO config failed!\r\n");
		return Status;
	}

	for(uint16_t i = 0; i < OUTPUT_NAME_MAX; i++)
	{
		if(OutputArg[i].Enable == ENABLE)
		{
			if ((OutputArg[i].GpioMode == MIO) || (OutputArg[i].GpioMode == EMIO))
			{
				XGpioPs_SetDirectionPin(&PsGpio, OutputArg[i].Pin, OUTPUT);
				XGpioPs_SetOutputEnablePin(&PsGpio, OutputArg[i].Pin, OutputArg[i].Enable);
				XGpioPs_WritePin(&PsGpio, OutputArg[i].Pin, OutputArg[i].InitLevel);
			}
			else if ((OutputArg[i].GpioMode == AXI_GPIO) && (OutputArg[i].DeviceId < PL_DEVICE_MAX))
			{
				PlId = OutputArg[i].DeviceId;
				Status = XGpio_Initialize(&PlGpio[PlId], OutputArg[i].DeviceId);
				if (Status != XST_SUCCESS)
				{
					LOG_ERROR("AXI GPIO config failed!\r\n");
					break;
				}
				GpioVal = XGpio_GetDataDirection(&PlGpio[PlId], OutputArg[i].AxiGpioChl);
				GpioVal = GpioVal & (0xFFFFFFFE << OutputArg[i].Pin);
				XGpio_SetDataDirection(&PlGpio[PlId], OutputArg[i].AxiGpioChl, GpioVal);//channel x xbit output

				GpioVal = XGpio_DiscreteRead(&PlGpio[PlId], OutputArg[i].AxiGpioChl);
				GpioVal = GpioVal | (OutputArg[i].InitLevel << OutputArg[i].Pin);
				XGpio_DiscreteWrite(&PlGpio[PlId], OutputArg[i].AxiGpioChl, GpioVal);
			}
			else if (OutputArg[i].GpioMode == AXI_BRAM)
			{
				axi_write_data(OutputArg[i].Pin, OutputArg[i].InitLevel);
			}
		}
	}

	return Status;
}


/**
 * 给指定的IO口输出指定的value 电平信号；
 */
void GPIO_OutputCtrl(OUTPUT_NAME name, uint8_t value)
{
//	axi_write_data(name, value);
	uint32_t GpioVal;
	uint8_t i = (uint8_t)name;
	if ((i < OUTPUT_NAME_MAX) && (OutputArg[i].Enable == ENABLE))
	{
		if ((OutputArg[i].GpioMode == MIO) || (OutputArg[i].GpioMode == EMIO))
		{
			XGpioPs_WritePin(&PsGpio, OutputArg[i].Pin, value);
		}
		else if ((OutputArg[i].GpioMode == AXI_GPIO) && (OutputArg[i].DeviceId < PL_DEVICE_MAX))
		{
			GpioVal = XGpio_DiscreteRead(&PlGpio[OutputArg[i].DeviceId], (unsigned)OutputArg[i].AxiGpioChl);
			GpioVal = GpioVal | (value << OutputArg[i].Pin);
			XGpio_DiscreteWrite(&PlGpio[OutputArg[i].DeviceId], (unsigned)OutputArg[i].AxiGpioChl, GpioVal);
		}
		else if (OutputArg[i].GpioMode == AXI_BRAM)
		{
			axi_write_data(OutputArg[i].Pin, value);
		}
	}
}

/**
 * 读取引脚电平
 * @param name 引脚名字
 * @return	1: 高电平     0: 低电平
 */
uint8_t Output_GetValue(OUTPUT_NAME name)
{
//	return (uint8_t)axi_read_data(name);
	uint32_t GpioVal;
	uint8_t i = (uint8_t)name;
	uint8_t status;

	XGpioPs *pPsGpio;
	XGpio *pPlGpio;

	if ((i < OUTPUT_NAME_MAX) && (OutputArg[i].Enable == ENABLE))
	{
		if ((OutputArg[i].GpioMode == MIO) || (OutputArg[i].GpioMode == EMIO))
		{
			pPsGpio = GetPsGpioDev();
			status = (uint8_t)XGpioPs_ReadPin(pPsGpio, OutputArg[i].Pin);
		}
		else if (OutputArg[i].GpioMode == AXI_GPIO)
		{
			pPlGpio = GetPlGpioDev(OutputArg[i].DeviceId);
			GpioVal = XGpio_DiscreteRead(pPlGpio, OutputArg[i].AxiGpioChl);
			GpioVal = GpioVal & (1 << OutputArg[i].Pin);
			status = (GpioVal == 0);
		}
		else if (OutputArg[i].GpioMode == AXI_BRAM)
		{
			status = (uint8_t)axi_read_data(name);
		}
	}

	return status;
}
/**
 * write register of frequency；
 */
void write_register(uint32_t regAddr, uint32_t value)
{
	axi_write_data(regAddr, value);
}
