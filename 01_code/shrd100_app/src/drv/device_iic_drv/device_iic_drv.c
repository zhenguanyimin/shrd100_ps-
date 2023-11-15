/*
 * temperature_iic.c
 *
 *  Created on: 2023年3月22日
 *      Author: A22745
 */


/***************************** Include Files **********************************/
#include "device_iic_drv.h"

#include "xparameters.h"
#include "xiicps.h"
#include "xil_printf.h"

#include "../../hal/output/output.h"
#include "../../srv/log/log.h"

/************************** Constant Definitions ******************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */

/*
 * The slave address to send to and receive from.
 */
//#define TEMPERATURE_IIC_SCLK_RATE		400000
#define TEMPERATURE_IIC_SCLK_RATE		200000

#define COUT_TIMEOUT		(350)

/**************************** Type Definitions ********************************/



/************************** Function Prototypes *******************************/

/************************** Variable Definitions ******************************/

XIicPs TemperatureIic;		/**< Instance of the TemperatureIic Device */

/******************************************************************************/
/**
*
* Main function to call the polled master example.
*
*
* @return	XST_SUCCESS if successful, XST_FAILURE if unsuccessful.
*
* @note		None.
*
*******************************************************************************/
XIicPs g_I2C;
XIicPs *I2C_Ptr = &g_I2C;

static SemaphoreHandle_t g_temperatureSemaphore_1 = NULL ;


int DeviceI2cInitDrv(u16 DeviceId)
{
	int Status;
	XIicPs_Config *Config;

	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table, then initialize it.
	 */
	Config = XIicPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(I2C_Ptr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XIicPsSetTimeOut(I2C_Ptr, 0x08);
	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(I2C_Ptr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(I2C_Ptr, TEMPERATURE_IIC_SCLK_RATE);

	g_temperatureSemaphore_1 = xSemaphoreCreateMutex();
	if (g_temperatureSemaphore_1 == NULL)
	{
		LOG_ERROR("%s[:%d] semaphore create fail\r\n", __FUNCTION__, __LINE__);
	}
	LOG_DEBUG("DeviceI2cInitDrv success\r\n");
	return XST_SUCCESS;
}

int8_t I2c_SemaphoreTake(uint64_t delayTime)
{
	if (g_temperatureSemaphore_1 != NULL)
	{
		xSemaphoreTake( g_temperatureSemaphore_1 , portMAX_DELAY );
		return XST_SUCCESS;
	}
	else
		return XST_FAILURE;
}

int8_t I2c_SemaphoreGive(void)
{
	if (g_temperatureSemaphore_1 != NULL)
	{
		xSemaphoreGive( g_temperatureSemaphore_1 );
		return XST_SUCCESS;
	}
	else
		return XST_FAILURE;
}

static void I2cSensorPowerReset(void)
{
//	GPIO_OutputCtrl(EMIO_VSENSE_3V3_OT_EN, IO_VOL);
	vTaskDelay(20);
//	GPIO_OutputCtrl(EMIO_VSENSE_3V3_OT_EN, IO_VOH);
	vTaskDelay(1);
}

ret_code_t I2cPs_write(u8 *MsgPtr, s32 ByteCount, u16 SlaveAddr)
{
	uint16_t count = 0;
	ret_code_t ret = RET_OK;
	static uint8_t countt = 0;
//	xSemaphoreTake( g_temperatureSemaphore_1 , portMAX_DELAY );
	I2c_SemaphoreTake(portMAX_DELAY);
//	XIicPs_MasterSendPolled(I2C_Ptr, MsgPtr, ByteCount, SlaveAddr);
	if (XIicPs_MasterSendPolled(I2C_Ptr, MsgPtr, ByteCount, SlaveAddr) == XST_FAILURE)
	{
		countt++;
	}
	else
	{
		countt = 0;
	}

	if (countt >= 5)
	{
		I2cSensorPowerReset();
		XIicPs_Reset(I2C_Ptr);
//		XIicPs_ResetHw(I2C_Ptr);
		DeviceI2cInitDrv(XPAR_PSU_I2C_1_DEVICE_ID);
	}

	if (countt  == 0)
	{
		while (XIicPs_BusIsBusy(I2C_Ptr) && (count < COUT_TIMEOUT)) {
			count++;
		}
	}
	vTaskDelay(1);
//	xSemaphoreGive( g_temperatureSemaphore_1 );
	I2c_SemaphoreGive();
	if (count >= COUT_TIMEOUT)
	{
		ret = RET_GENERAL_ERR;
	}
	return ret;
}

ret_code_t I2cPs_read(u8 *MsgPtr, s32 ByteCount, u16 SlaveAddr)
{
	uint16_t count = 0;
	ret_code_t ret = RET_OK;
	static uint8_t countt = 0;
//	xSemaphoreTake( g_temperatureSemaphore_1 , portMAX_DELAY );
	I2c_SemaphoreTake(portMAX_DELAY);
//	XIicPs_MasterRecvPolled(I2C_Ptr, MsgPtr, ByteCount, SlaveAddr);
	if (XIicPs_MasterRecvPolled(I2C_Ptr, MsgPtr, ByteCount, SlaveAddr) == XST_FAILURE)
	{
		countt++;
	}
	else
	{
		countt = 0;
	}

	if (countt >= 5)
	{
		I2cSensorPowerReset();
		XIicPs_Reset(I2C_Ptr);
//		XIicPs_ResetHw(I2C_Ptr);
		DeviceI2cInitDrv(XPAR_PSU_I2C_1_DEVICE_ID);
	}

	if (countt  == 0)
	{
		while (XIicPs_BusIsBusy(I2C_Ptr) && (count < COUT_TIMEOUT)) {
			count++;
		}
	}
	vTaskDelay(1);
//	xSemaphoreGive( g_temperatureSemaphore_1 );
	I2c_SemaphoreGive();
	if (count >= COUT_TIMEOUT)
	{
		ret = RET_GENERAL_ERR;
	}
	return ret;
}

