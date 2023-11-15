/*
 * temperature_iic.c
 *
 *  Created on: 2023年3月22日
 *      Author: A22745
 */


/***************************** Include Files **********************************/
#include "iic_pl.h"
#include "sleep.h"
#include "xparameters.h"
#include "xiic.h"
#include "xiic_i.h"
#include "xil_exception.h"

#include "../../hal/output/output.h"
#include "../../srv/log/log.h"

/************************** Constant Definitions ******************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */


/**************************** Type Definitions ********************************/


/************************** Function Prototypes *******************************/

/************************** Variable Definitions ******************************/


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


XIic Iic; /* The driver instance for IIC Device */
static SemaphoreHandle_t g_i2c_pl_Semaphore = NULL ;


int DeviceI2cPlInitDrv(u16 IicDeviceId)
{
	int Status;
	static int Initialized = FALSE;
	XIic_Config *ConfigPtr;	/* Pointer to configuration data */

	if (!Initialized) {
		Initialized = TRUE;

		/*
		 * Initialize the IIC driver so that it is ready to use.
		 */
		ConfigPtr = XIic_LookupConfig(IicDeviceId);
		if (ConfigPtr == NULL) {
			return XST_FAILURE;
		}

		Status = XIic_CfgInitialize(&Iic, ConfigPtr,
						ConfigPtr->BaseAddress);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		/*
		 * Start the IIC device.
		 */
		Status = XIic_Start(&Iic);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

	g_i2c_pl_Semaphore = xSemaphoreCreateMutex();
	if (g_i2c_pl_Semaphore == NULL)
	{
		LOG_ERROR("%s[:%d] semaphore create fail\r\n", __FUNCTION__, __LINE__);
	}
	LOG_DEBUG("DeviceI2cInitDrv success\r\n");
	return XST_SUCCESS;
}

int8_t I2cPl_SemaphoreTake(uint64_t delayTime)
{
	if (g_i2c_pl_Semaphore != NULL)
	{
		xSemaphoreTake( g_i2c_pl_Semaphore , portMAX_DELAY );
		return XST_SUCCESS;
	}
	else
		return XST_FAILURE;
}

int8_t I2cPl_SemaphoreGive(void)
{
	if (g_i2c_pl_Semaphore != NULL)
	{
		xSemaphoreGive( g_i2c_pl_Semaphore );
		return XST_SUCCESS;
	}
	else
		return XST_FAILURE;
}

static unsigned RecvData(UINTPTR BaseAddress, u8 *BufferPtr,
			 unsigned ByteCount, u8 Option)
{
	u32 CntlReg;
	u32 IntrStatusMask;
	u32 IntrStatus;

	/* Attempt to receive the specified number of bytes on the IIC bus */

	while (ByteCount > 0) {
		/* Setup the mask to use for checking errors because when
		 * receiving one byte OR the last byte of a multibyte message an
		 * error naturally occurs when the no ack is done to tell the
		 * slave the last byte
		 */
		if (ByteCount == 1) {
			IntrStatusMask =
				XIIC_INTR_ARB_LOST_MASK | XIIC_INTR_BNB_MASK;
		} else {
			IntrStatusMask =
				XIIC_INTR_ARB_LOST_MASK |
				XIIC_INTR_TX_ERROR_MASK | XIIC_INTR_BNB_MASK;
		}

		/* Wait for the previous transmit and the 1st receive to
		 * complete by checking the interrupt status register of the
		 * IPIF
		 */
		while (1) {
			IntrStatus = XIic_ReadIisr(BaseAddress);
			if (IntrStatus & XIIC_INTR_RX_FULL_MASK) {
				break;
			}
			/* Check the transmit error after the receive full
			 * because when sending only one byte transmit error
			 * will occur because of the no ack to indicate the end
			 * of the data
			 */
			if (IntrStatus & IntrStatusMask) {
				return ByteCount;
			}
		}

		CntlReg = XIic_ReadReg(BaseAddress,  XIIC_CR_REG_OFFSET);

		/* Special conditions exist for the last two bytes so check for
		 * them. Note that the control register must be setup for these
		 * conditions before the data byte which was already received is
		 * read from the receive FIFO (while the bus is throttled
		 */
		if (ByteCount == 1) {
			if (Option == XIIC_STOP) {

				/* If the Option is to release the bus after the
				 * last data byte, it has already been read and
				 * no ack has been done, so clear MSMS while
				 * leaving the device enabled so it can get off
				 * the IIC bus appropriately with a stop
				 */
				XIic_WriteReg(BaseAddress,  XIIC_CR_REG_OFFSET,
					 XIIC_CR_ENABLE_DEVICE_MASK);
			}
		}

		/* Before the last byte is received, set NOACK to tell the slave
		 * IIC device that it is the end, this must be done before
		 * reading the byte from the FIFO
		 */
		if (ByteCount == 2) {
			/* Write control reg with NO ACK allowing last byte to
			 * have the No ack set to indicate to slave last byte
			 * read
			 */
			XIic_WriteReg(BaseAddress,  XIIC_CR_REG_OFFSET,
				 CntlReg | XIIC_CR_NO_ACK_MASK);
		}

		/* Read in data from the FIFO and unthrottle the bus such that
		 * the next byte is read from the IIC bus
		 */
		IntrStatus = (u8) XIic_ReadReg(BaseAddress,
				  XIIC_DRR_REG_OFFSET);
		*BufferPtr++ = IntrStatus;

		if ((ByteCount == 1) && (Option == XIIC_REPEATED_START)) {

			/* RSTA bit should be set only when the FIFO is
			 * completely Empty.
			 */
			XIic_WriteReg(BaseAddress,  XIIC_CR_REG_OFFSET,
				 XIIC_CR_ENABLE_DEVICE_MASK | XIIC_CR_MSMS_MASK
				 | XIIC_CR_REPEATED_START_MASK);

		}

		/* Clear the latched interrupt status so that it will be updated
		 * with the new state when it changes, this must be done after
		 * the receive register is read
		 */
		XIic_ClearIisr(BaseAddress, XIIC_INTR_RX_FULL_MASK |
				XIIC_INTR_TX_ERROR_MASK |
				XIIC_INTR_ARB_LOST_MASK);
		ByteCount--;
		usleep(1000);
	}

	if (Option == XIIC_STOP) {

		/* If the Option is to release the bus after Reception of data,
		 * wait for the bus to transition to not busy before returning,
		 * the IIC device cannot be disabled until this occurs. It
		 * should transition as the MSMS bit of the control register was
		 * cleared before the last byte was read from the FIFO
		 */
		while (1) {
			if (XIic_ReadIisr(BaseAddress) & XIIC_INTR_BNB_MASK) {
				break;
			}
		}
	}

	return ByteCount;
}
int I2cPl_write(u8 *MsgPtr, s32 ByteCount, u16 SlaveAddr)
{

	ret_code_t ret = RET_OK;
	XIic *InstancePtr =&Iic;
	u8 *BufferPtr = MsgPtr;
	volatile u32 StatusReg;


	I2cPl_SemaphoreTake(portMAX_DELAY);

	XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET, XIIC_CR_TX_FIFO_RESET_MASK );
	XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET, XIIC_CR_ENABLE_DEVICE_MASK );

	XIic_DynSend7BitAddress(InstancePtr->BaseAddress,
			SlaveAddr, XIIC_WRITE_OPERATION);

	for( uint32_t i = 0 ; i < ByteCount ; i++ )
	{
		XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_DTR_REG_OFFSET, BufferPtr[i]);
	}

//	 usleep(200);
	 do
	 {
		StatusReg = XIic_ReadReg(InstancePtr->BaseAddress, XIIC_SR_REG_OFFSET);
	}while ((StatusReg & XIIC_SR_BUS_BUSY_MASK) == 0);

	 do
	 {
		 usleep(10);
		StatusReg = XIic_ReadReg(InstancePtr->BaseAddress, XIIC_SR_REG_OFFSET);
	}while ((StatusReg & XIIC_SR_TX_FIFO_EMPTY_MASK) == XIIC_SR_TX_FIFO_EMPTY_MASK);
	usleep(100);
	XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET, 0 );
	I2cPl_SemaphoreGive();

	return ret;
}
int I2cPl_read(u8 *MsgPtr, s32 ByteCount, u16 SlaveAddr)
{

	XIic *InstancePtr =&Iic;
	u32 CntlReg;
	volatile u32 StatusReg;
	u32 RecCnt=0;
	u32 IntrStatusMask;
	u32 IntrStatus;
	u8 Temp;

	I2cPl_SemaphoreTake(portMAX_DELAY);
	if (ByteCount == 1) {
		Temp = 0;
	} else {
		if (ByteCount <= IIC_RX_FIFO_DEPTH) {
			Temp = ByteCount - 2;
		} else {
			Temp = IIC_RX_FIFO_DEPTH - 1;
		}
	}
	XIic_WriteReg(InstancePtr->BaseAddress, XIIC_RFD_REG_OFFSET,(u32) Temp);

		XIic_Send7BitAddress(InstancePtr->BaseAddress, SlaveAddr,
				XIIC_READ_OPERATION);

	CntlReg = XIIC_CR_MSMS_MASK |
			 XIIC_CR_ENABLE_DEVICE_MASK|
				XIIC_CR_REPEATED_START_MASK;
	XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET, CntlReg );


	while (ByteCount > 0) {

			if (ByteCount == 1) {
				IntrStatusMask =
					XIIC_INTR_ARB_LOST_MASK | XIIC_INTR_BNB_MASK;
			} else {
				IntrStatusMask =
					XIIC_INTR_ARB_LOST_MASK |
					XIIC_INTR_TX_ERROR_MASK | XIIC_INTR_BNB_MASK;

				while (1) {
					IntrStatus = XIic_ReadIisr(InstancePtr->BaseAddress);
					if (IntrStatus & XIIC_INTR_RX_FULL_MASK) {
						break;
					}
				}
			}

			if (ByteCount <= 3) {

				CntlReg = XIic_ReadReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET);
				XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET,
					 CntlReg | XIIC_CR_NO_ACK_MASK);
			}

			*MsgPtr++ = (u8) XIic_ReadReg(InstancePtr->BaseAddress,
							  XIIC_DRR_REG_OFFSET);

			XIic_ClearIisr(InstancePtr->BaseAddress, XIIC_INTR_RX_FULL_MASK |
					XIIC_INTR_TX_ERROR_MASK |
					XIIC_INTR_ARB_LOST_MASK);
			ByteCount--;
		}


	XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET, 0 );

	I2cPl_SemaphoreGive();

	return ByteCount;
}

int I2cPl_read_bac(u8 *MsgPtr, s32 ByteCount, u16 SlaveAddr)
{

	XIic *InstancePtr =&Iic;
	u32 CntlReg;
	volatile u32 StatusReg;
	u32 RecCnt=0;
	u32 IntrStatusMask;
	u32 IntrStatus;
	u8 Temp;

	I2cPl_SemaphoreTake(portMAX_DELAY);

	if (ByteCount == 1) {
		Temp = 0;
	} else {
		if (ByteCount <= IIC_RX_FIFO_DEPTH) {
			Temp = ByteCount - 2;
		} else {
			Temp = IIC_RX_FIFO_DEPTH - 1;
		}
	}
	XIic_WriteReg(InstancePtr->BaseAddress, XIIC_RFD_REG_OFFSET,(u32) Temp);

		XIic_Send7BitAddress(InstancePtr->BaseAddress, SlaveAddr,
				XIIC_READ_OPERATION);

//	usleep(500);

	CntlReg = XIIC_CR_MSMS_MASK |
			 XIIC_CR_ENABLE_DEVICE_MASK|
				XIIC_CR_REPEATED_START_MASK;
	XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET, CntlReg );

	//	XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET,
	//		 XIIC_CR_ENABLE_DEVICE_MASK);

	//	XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET,
	//		 CntlReg | XIIC_CR_NO_ACK_MASK);

//	*MsgPtr++ = (u8) XIic_ReadReg(InstancePtr->BaseAddress,
//					  XIIC_DRR_REG_OFFSET);
//
//	*MsgPtr++ = (u8) XIic_ReadReg(InstancePtr->BaseAddress,
//					  XIIC_DRR_REG_OFFSET);
//
//	*MsgPtr++ = (u8) XIic_ReadReg(InstancePtr->BaseAddress,
//					  XIIC_DRR_REG_OFFSET);

//	usleep(1000);
	while (ByteCount > 0) {

			if (ByteCount == 1) {
				IntrStatusMask =
					XIIC_INTR_ARB_LOST_MASK | XIIC_INTR_BNB_MASK;
			} else {
				IntrStatusMask =
					XIIC_INTR_ARB_LOST_MASK |
					XIIC_INTR_TX_ERROR_MASK | XIIC_INTR_BNB_MASK;

				while (1) {
					IntrStatus = XIic_ReadIisr(InstancePtr->BaseAddress);
					if (IntrStatus & XIIC_INTR_RX_FULL_MASK) {
						break;
					}
				}
			}

			CntlReg = XIic_ReadReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET);

			if (ByteCount == 2) {
					XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET,
						 XIIC_CR_ENABLE_DEVICE_MASK);
			}

			if (ByteCount == 3) {

				XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET,
					 CntlReg | XIIC_CR_NO_ACK_MASK);
			}

			*MsgPtr++ = (u8) XIic_ReadReg(InstancePtr->BaseAddress,
							  XIIC_DRR_REG_OFFSET);

			XIic_ClearIisr(InstancePtr->BaseAddress, XIIC_INTR_RX_FULL_MASK |
					XIIC_INTR_TX_ERROR_MASK |
					XIIC_INTR_ARB_LOST_MASK);
			ByteCount--;
		}


	XIic_WriteReg(InstancePtr->BaseAddress,  XIIC_CR_REG_OFFSET, 0 );
	usleep(1000);
	I2cPl_SemaphoreGive();

	return ByteCount;
}
