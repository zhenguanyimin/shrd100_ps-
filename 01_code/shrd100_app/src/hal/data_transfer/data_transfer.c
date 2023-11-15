
#include "data_transfer.h"

#include "xgpio.h"

#define RESET_TIMEOUT_COUNTER (10000)

extern void *IntcGetGicInst(void);

static XAxiDma adcDma = {0};
static XAxiDma sRdmDma = {0};
static XAxiDma sFftDma = {0};
static DataTransCallback pAdcCallback = NULL;
static DataTransCallback pRdmCallback = NULL;
static DataTransCallback pWifiCallback = NULL;
static DataTransCallback pFftCallback = NULL;

static s32 status = 0;
static XGpio sADCDMAGpio;
static s32 sInit = 0;
// XGpio errTri = {0};

static void data_trans_TxIntrHandler(void *Callback)
{
	u32 IrqStatus;
	int TimeOut;
	XAxiDma *pAxiDmaInst = (XAxiDma *)Callback;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_IntrGetIrq(pAxiDmaInst, XAXIDMA_DMA_TO_DEVICE);
	/* Acknowledge pending interrupts */
	XAxiDma_IntrAckIrq(pAxiDmaInst, IrqStatus, XAXIDMA_DMA_TO_DEVICE);
	/*
	 * If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK))
	{
		return;
	}

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK))
	{
		status = 1;

		/*
		 * Reset should never fail for transmit channel
		 */
		XAxiDma_Reset(pAxiDmaInst);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut)
		{
			if (XAxiDma_ResetIsDone(pAxiDmaInst))
			{
				break;
			}
			TimeOut -= 1;
		}

		return;
	}

	/*
	 * If Completion interrupt is asserted, then set the TxDone flag
	 */
	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK))
	{
		//		send tx done event to app
	}
}

int sTimes = 0;
static void data_trans_RxIntrHandler(void *Callback)
{
	u32 IrqStatus;
	int TimeOut;
	XAxiDma *pAxiDmaInst = (XAxiDma *)Callback;
	sTimes++;
	/* Read pending interrupts */
	IrqStatus = XAxiDma_IntrGetIrq(pAxiDmaInst, XAXIDMA_DEVICE_TO_DMA);
	/* Acknowledge pending interrupts */
	XAxiDma_IntrAckIrq(pAxiDmaInst, IrqStatus, XAXIDMA_DEVICE_TO_DMA);
	/*
	 * If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK))
	{
		// XGpio_DiscreteWrite(&errTri ,1, 0 );
		// XGpio_DiscreteWrite(&errTri ,1, 1 );
		// XGpio_DiscreteWrite(&errTri ,1, 0 );
		// XGpio_DiscreteWrite(&errTri ,1, 1 );
		return;
	}

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK))
	{
		status = 1;
		/* Reset could fail and hang
		 * NEED a way to handle this or do not call it??
		 */
		// XGpio_DiscreteWrite(&errTri ,1, 0 );
		XAxiDma_Reset(pAxiDmaInst);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut)
		{
			if (XAxiDma_ResetIsDone(pAxiDmaInst))
			{
				break;
			}
			TimeOut -= 1;
		}

		return;
	}

	/*
	 * If completion interrupt is asserted, then set RxDone flag
	 */
	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK))
	{
		if ((pAxiDmaInst == &adcDma) && (pAdcCallback))
		{
			pAdcCallback(DIR_DATA, ADC_PACKET_LEN);
		}
		if ((pAxiDmaInst == &sRdmDma) && (pRdmCallback))
		{
			pRdmCallback(DET_DATA, DET_DMA_PACKET_LEN);
		}
		if ((pAxiDmaInst == &sFftDma) && (pFftCallback))
		{
			pFftCallback(FFT_DATA, FFT_DMA_PACKET_LEN);
		}
	}
}

static s32 data_trans_DmaInit(XAxiDma *pAxiDma, u32 deviceId)
{
	int Status;
	XAxiDma_Config *Config = NULL;

	Config = XAxiDma_LookupConfig(deviceId);
	if (!Config)
	{
		xil_printf("No config found for %d\r\n", deviceId);
		return XST_FAILURE;
	}

	/* Initialize DMA engine */
	Status = XAxiDma_CfgInitialize(pAxiDma, Config);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Initialization failed %d\r\n", Status);
		return XST_FAILURE;
	}

	if (XAxiDma_HasSg(pAxiDma))
	{
		xil_printf("Device configured as SG mode \r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

static s32 data_trans_DmaEnableInt(XAxiDma *pAxiDma)
{
	/* Disable all interrupts before setup */

	XAxiDma_IntrDisable(pAxiDma, (u32)XAXIDMA_IRQ_ALL_MASK,
			(u32)XAXIDMA_DMA_TO_DEVICE);

	XAxiDma_IntrDisable(pAxiDma, (u32)XAXIDMA_IRQ_ALL_MASK,
			(u32)XAXIDMA_DEVICE_TO_DMA);

	/* Enable tx interrupt */
	XAxiDma_IntrEnable(pAxiDma, XAXIDMA_IRQ_ALL_MASK,
					   XAXIDMA_DMA_TO_DEVICE);
	/* Enable tx interrupt */
	XAxiDma_IntrEnable(pAxiDma, XAXIDMA_IRQ_ALL_MASK,
					   XAXIDMA_DEVICE_TO_DMA);
	return XST_SUCCESS;
}

static s32 data_trans_DmaRxStart(XAxiDma *pAxiDma, u8 *pBuf, s32 len)
{

	int status = XST_FAILURE;

	status = XAxiDma_SimpleTransfer(pAxiDma, (s32)pBuf, len, XAXIDMA_DEVICE_TO_DMA);

	return status;
}

static s32 data_trans_DmaTxStart(XAxiDma *pAxiDma, u8 *pBuf, s32 len)
{

	int status = XST_FAILURE;

	//	Xil_DCacheFlushRange((u32)pBuf, len);
	status = XAxiDma_SimpleTransfer(pAxiDma, (u32)pBuf,
									len, XAXIDMA_DMA_TO_DEVICE);

	return status;
}

static s32 data_trans_EnGuicInt(XScuGic *pIntcInstance, XAxiDma *pAxiDma, u32 IntID, Xil_InterruptHandler handler)
{
	int Status;

	// Priority:supported with a step of 8 as 0, 8, 16, 32, 40 ..., 248
	// Trigger:0x3 for Rising edge
	if (pAxiDma == &adcDma)
		XScuGic_SetPriorityTriggerType(pIntcInstance, IntID, 0xA0, 0x3);
	else if (pAxiDma == &sRdmDma)
		XScuGic_SetPriorityTriggerType(pIntcInstance, IntID, 0xA8, 0x3);
	//	XScuGic_SetPriorityTriggerType(pIntcInstance, IntID, 16, 0x3);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(pIntcInstance, IntID,
							 (Xil_InterruptHandler)handler,
							 pAxiDma);
	if (Status != XST_SUCCESS)
	{
		return Status;
	}

	XScuGic_Enable(pIntcInstance, IntID);

	return XST_SUCCESS;
}

s32 data_trans_init(s32 type)
{
	s32 Status = XST_FAILURE;

	if (DIR_DATA == type)
	{
		Status = data_trans_DmaInit(&adcDma, XPAR_AXIDMA_1_DEVICE_ID);
		if (!Status)
		{
			XGpio_Initialize(&sADCDMAGpio, XPAR_AXI_GPIO_DMA_DEVICE_ID);
			XGpio_SetDataDirection(&sADCDMAGpio, 1, 0);

			// XGpio_Initialize(&errTri,XPAR_AXI_GPIO_0_DEVICE_ID);
			// XGpio_SetDataDirection(&errTri, 1, 0);
			XGpio_DiscreteWrite(&sADCDMAGpio, 1, 1);
			Status = data_trans_DmaEnableInt(&adcDma);
		}
	}
	else if (DET_DATA == type)
	{
		Status = data_trans_DmaInit(&sRdmDma, XPAR_AXIDMA_1_DEVICE_ID);
		if (!Status)
		{
			XGpio_Initialize(&sADCDMAGpio, XPAR_AXI_GPIO_DMA_DEVICE_ID);
			XGpio_SetDataDirection(&sADCDMAGpio, 1, 0);
			XGpio_DiscreteWrite(&sADCDMAGpio, 1, 1);
			Status = data_trans_DmaEnableInt(&sRdmDma);
		}
	}
	else if (FFT_DATA == type)
	{
		Status = data_trans_DmaInit(&sFftDma, XPAR_AXI_DMA_FFT_DEVICE_ID);
		if (!Status)
		{
			XGpio_Initialize(&sADCDMAGpio, XPAR_AXI_GPIO_DMA_DEVICE_ID);
			XGpio_SetDataDirection(&sADCDMAGpio, 1, 0);
			XGpio_DiscreteWrite(&sADCDMAGpio, 1, 1);
			Status = data_trans_DmaEnableInt(&sFftDma);
		}
	}


	return Status;
}

void data_trans_deInit(s32 type)
{
	return;
}
s32 data_trans_set_callback(s32 type, DataTransCallback cb)
{
	s32 status = XST_FAILURE, intID = 0;
	XScuGic *pGic = NULL;
	XAxiDma *pAxiDma = NULL;
	Xil_InterruptHandler handler = NULL;

	pGic = (XScuGic *)IntcGetGicInst();

	if ((DIR_DATA == type) && pGic)
	{
		pAxiDma = &adcDma;
		intID = XPAR_FABRIC_AXI_DMA_ADC_S2MM_INTROUT_INTR;
		handler = data_trans_RxIntrHandler;
		pAdcCallback = cb;
	}
	else if ((DET_DATA == type) && pGic)
	{
		pAxiDma = &sRdmDma;
		intID = XPAR_FABRIC_AXI_DMA_OTHERS_S2MM_INTROUT_INTR;
		handler = data_trans_RxIntrHandler;
		pRdmCallback = cb;
	}
	else if ((FFT_DATA == type) && pGic)
	{
		pAxiDma = &sFftDma;
		intID = XPAR_FABRIC_AXI_DMA_FFT_S2MM_INTROUT_INTR;
		handler = data_trans_RxIntrHandler;
		pFftCallback = cb;
	}

	status = data_trans_EnGuicInt(pGic, pAxiDma, intID, handler);

	return status;
}

s32 data_trans_start(s32 type, u8 *pBuf, s32 len)
{
	s32 status = 0;

	if (DIR_DATA == type)
	{
		status = data_trans_DmaRxStart(&adcDma, pBuf, len);
	}
	else if (DET_DATA == type)
	{
		status = data_trans_DmaRxStart(&sRdmDma, pBuf, len);
	}
	else if (FFT_DATA == type)
	{
		status = data_trans_DmaRxStart(&sFftDma, pBuf, len);
	}

	if (!sInit)
	{
		XGpio_DiscreteWrite(&sADCDMAGpio, 1, 3);
		// XGpio_DiscreteWrite(&errTri ,1, 1 );
	}

	sInit = 1;

	return status;
}

s32 data_trans_stop(s32 type)
{
	return 0;
}

s32 data_trans_status(s32 type)
{
	s32 st = 0;

	if (DIR_DATA == type)
	{
		st = status & 0x01;
	}

	return st;
}

s32 data_trans_reset(s32 type)
{
	if (DIR_DATA == type)
	{
		XGpio_DiscreteWrite(&sADCDMAGpio, 1, 0);
		sInit = 0;
	}

	return 0;
}

void data_trans_CacheInvalidate(u8 *pBuf, u32 len)
{
	Xil_DCacheInvalidateRange((INTPTR)pBuf, len);
}

void data_trans_CacheFlush(u8 *pBuf, s32 len)
{
}
