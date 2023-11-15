/*
 * uartps_intr.c
 *
 * Created on: 2019��05��02��
 * www.osrc.cn
 * copyright by cz123 msxbo
*/
#include "../uart_init/uartps_intr.h"

volatile int TotalReceivedCount;
volatile int TotalSentCount;
static int TotalErrorCount;
static uint8_t UartPsSendBuffer[TEST_BUFFER_SIZE];	/* Buffer for Transmitting Data */
static uint8_t UartPsRecvBuffer[TEST_BUFFER_SIZE];	/* Buffer for Receiving Data */
XUartPs UartPs;//uart
extern int UartPsRevProcess(uint8_t *databuff, uint32_t len);

int Init_UartPsIntr(XUartPs *UartInstPtr,u16 DeviceId )
{
	int Status;
	XUartPs_Config *Config;
	u32 IntrMask;

	if (XGetPlatform_Info() == XPLAT_ZYNQ_ULTRA_MP) {
#ifdef XPAR_XUARTPS_1_DEVICE_ID
//		DeviceId = XPAR_XUARTPS_1_DEVICE_ID;
#endif
	}

	Config = XUartPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(UartInstPtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/*
	 * Setup the handlers for the UART that will be called from the
	 * interrupt context when data has been sent and received, specify
	 * a pointer to the UART driver instance as the callback reference
	 * so the handlers are able to access the instance data
	 */
	XUartPs_SetHandler(UartInstPtr, (XUartPs_Handler)UartPs_Intr_Handler, UartInstPtr);

	/*
	 * Enable the interrupt of the UART so interrupts will occur, setup
	 * a local loopback so data that is sent will be received.
	 */
	IntrMask =
		XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY | XUARTPS_IXR_FRAMING |
		XUARTPS_IXR_OVER | XUARTPS_IXR_TXEMPTY | XUARTPS_IXR_RXFULL |
		XUARTPS_IXR_RXOVR;

	if (UartInstPtr->Platform == XPLAT_ZYNQ_ULTRA_MP) {
		IntrMask |= XUARTPS_IXR_RBRK;
	}

	XUartPs_SetInterruptMask(UartInstPtr, IntrMask);

	XUartPs_SetRecvTimeout(UartInstPtr, 16);

	return XST_SUCCESS;
}

static uint8_t test_data[] = {"hello world!"};
void test_uart_send()
{
	XUartPs_Send(&UartPs, test_data, strlen(test_data));
}


void UartPs_Intr_Handler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	int i = 0;
	/* All of the data has been sent */
	if (Event == XUARTPS_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/* All of the data has been received */
	if (Event == XUARTPS_EVENT_RECV_DATA) {
		TotalReceivedCount = EventData;
		if(TotalReceivedCount == TEST_BUFFER_SIZE) {
			for(i=0;i<TotalReceivedCount;i++)
				UartPsSendBuffer[i] = UartPsRecvBuffer[i];

			XUartPs_Send(&UartPs, UartPsSendBuffer, TotalReceivedCount);
			XUartPs_Recv(&UartPs, UartPsRecvBuffer, TEST_BUFFER_SIZE);
			TotalReceivedCount=0;
		}

	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 8 character times
	 */
	if (Event == XUARTPS_EVENT_RECV_TOUT) {
		TotalReceivedCount = EventData;
//			for(i=0;i<TotalReceivedCount;i++)
//				UartPsSendBuffer[i] = UartPsRecvBuffer[i];
		UartPsRevProcess(UartPsRecvBuffer, EventData);
//		XUartPs_Send(&UartPs, UartPsRecvBuffer, TotalReceivedCount);
		XUartPs_Recv(&UartPs, UartPsRecvBuffer, TEST_BUFFER_SIZE);
		TotalReceivedCount=0;
	}

	/*
	 * Data was received with an error, keep the data but determine
	 * what kind of errors occurred
	 */
	if (Event == XUARTPS_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
	}

	/*
	 * Data was received with an parity or frame or break error, keep the data
	 * but determine what kind of errors occurred. Specific to Zynq Ultrascale+
	 * MP.
	 */
	if (Event == XUARTPS_EVENT_PARE_FRAME_BRKE) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
	}

	/*
	 * Data was received with an overrun error, keep the data but determine
	 * what kind of errors occurred. Specific to Zynq Ultrascale+ MP.
	 */
	if (Event == XUARTPS_EVENT_RECV_ORERR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
	}
}

int UartPs_Setup_IntrSystem(XScuGic *IntcInstancePtr,XUartPs *UartInstancePtr,u16 UartIntrId)
{
	int Status;

	XScuGic_Config *IntcConfig; /* Config for interrupt controller */

	/* Initialize the interrupt controller driver */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*
	 * interrupt for the device occurs, the device driver handler
	 * performs the specific interrupt processing for the device
	 */
	Status = XScuGic_Connect(IntcInstancePtr, UartIntrId,
				  (Xil_ExceptionHandler) XUartPs_InterruptHandler,
				  (void *) UartInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Enable the interrupt for the device */
	XScuGic_Enable(IntcInstancePtr, UartIntrId);

	return XST_SUCCESS;
}

uint8_t* Get_BufferPointer(void)
{
	return UartPsRecvBuffer;
}

