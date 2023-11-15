/******************************************************************************
* Copyright (C) 2018 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
 ******************************************************************************/

/****************************************************************************/
/**
 *
 * @file xusb_freertos_massstorage.c
 *
 * This file implements the mass storage class example.
 *
 * @setup requirement
 * zcu102 board in usb device mode, connected with host using USB 3.0 cable
 *
 * Following other files require to run this example
 *  o xusb_ch9.c, xusb_ch9.h
 *  o xusb_freertos_ch9_storage.c, xusb_freerots_ch9_storage.h
 *  o xusb_freertos_class_storage.c, xusb_freertos_class_storage.h
 *
 * @validation
 *  o on success example will be detected as mass storage on host
 *  o do IN and OUT transfer of the same size
 *  o must not get diff between IN and OUT files
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -------------------------------------------------------
 * 1.0   rb   22/03/18 First release
 * 1.5   vak  13/02/19 Added support for versal
 * 1.5   vak  03/25/19 Fixed incorrect data_alignment pragma directive for IAR
 *
 * </pre>
 *
 *****************************************************************************/

/***************************** Include Files ********************************/
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "xparameters.h"
#include "xusb_ch9_cdc_acm.h"
#include "xusb_class_cdc_acm.h"
#include "xusb_cdc_acm_api.h"
#include "xusb_cdc_config.h"
#include "../../srv/alink/alink_msg_proc.h"
#include "../../inc/common.h"
#include <timers.h>
#include "../../srv/log/log.h"
#include "../../srv/cli/serial.h"
#include "../../hal/output/output.h"
#include "../../srv/cli/cli_if.h"

/************************** Constant Definitions ****************************/


/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/
#define USB_INTR_ID			XPAR_XUSBPS_0_INTR

/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/
struct Usb_DevData UsbInstance;
alink_msg_t psAns;

SemaphoreHandle_t  UsbSeamphore;

#define UART_BUFFER_SIZE 4096
//alink_msg_t sAlinkSend_msg;
static alink_msg_t sAlinkRecv_msg;
//uint8_t RecvBuffer[UART_BUFFER_SIZE];
static uint8_t uRecvHeadCnt = 0;
static uint8_t uRecvBufCnt = 0;
static uint16_t uLen = 0;

static TimerHandle_t Timer_Handle = NULL;
static TimerHandle_t Timer_UsbConnectCheck_Handle = NULL;

#define USB_DEV_SEND_DMA_BUFFER_SIZE (128) /** base on the USB end point buffer size */
#define USB_DEV_RECV_DMA_BUFFER_SIZE (512) /** base on the USB end point buffer size */


#define USB_DMA_BUFFER_ALIGNED __attribute__((aligned(sizeof(int))))

#define USB_RECV_DMA_BUFFER_CNT (2)

typedef struct 
{
	/** just for USB EP DMA **/
	USB_DMA_BUFFER_ALIGNED uint8_t send_dma_buffer[USB_DEV_SEND_DMA_BUFFER_SIZE];
	USB_DMA_BUFFER_ALIGNED uint8_t recv_dma_buffer[USB_RECV_DMA_BUFFER_CNT][USB_DEV_RECV_DMA_BUFFER_SIZE];

	uint32_t recv_dma_buffer_index;

	uint8_t send_fifo[USB_DEV_SEND_FIFO_SIZE]; /* the send data fifo buffer */
	uint32_t send_fifo_windex;
	uint32_t send_fifo_rindex;

	uint8_t recv_fifo[USB_DEV_RECV_FIFO_SIZE]; /* save all data to recv fifo */
	uint32_t recv_fifo_windex;
	uint32_t recv_fifo_rindex;
}Usb_dev_cdc_buffer_t;

/**
 * USB devcie CDC
 */
typedef struct 
{
	int opened;
	int binded_ep_num; //the usb end point that this cdc device used
	int send_dma_working;
	
	USB_dev_sent_cb_t sent_isr_cb;
	USB_dev_recv_cb_t recv_isr_cb;
	void *sent_cb_param;
	void *recv_cb_param;

	Usb_dev_cdc_buffer_t cdc_buffer;

	SemaphoreHandle_t api_mutex;
}USB_dev_cdc_t;

static USB_dev_cdc_t cdc_devices[USB_DEVICE_CHANNEL_LIMIT];
volatile static int USB_cdc_connected = 0;
static uint8_t check_usb_disconnect_flag = 0;

void SetUsbCdcConnected(uint8_t flag)
{
	USB_cdc_connected = flag;
}

static s32 SetConfiguration(struct Usb_DevData *InstancePtr, SetupPacket *Ctrl);


/* Initialize a DFU data structure */
static USBCH9_DATA cdc_acm_data = {
	.ch9_func = {
		.Usb_Ch9SetupDevDescReply = Usb_Ch9SetupDevDescReply,
		.Usb_Ch9SetupCfgDescReply = Usb_Ch9SetupCfgDescReply,
		.Usb_Ch9SetupBosDescReply = Usb_Ch9SetupBosDescReply,
		.Usb_Ch9SetupStrDescReply = Usb_Ch9SetupStrDescReply,
		.Usb_SetConfiguration = Usb_SetConfiguration,
		.Usb_SetConfigurationApp = SetConfiguration,///Usb_SetConfigurationApp,
		.Usb_ClassReq = ClassReq,
	},
	.data_ptr = cdc_devices,
};

static void BulkInHandler(void *CallBackRef, u32 RequestedBytes, u32 BytesTxed, USB_dev_cdc_t *dev);

static void UsbDisconnectCb(struct Usb_DevData *InstancePtr)
{
	USB_cdc_connected = 0;
//	LOG_ERROR("usb disconnect\r\n");
}

static s32 SetConfiguration(struct Usb_DevData *InstancePtr, SetupPacket *Ctrl)
{
	s32 ret;
	int i;
	USB_dev_cdc_t *pdev;
	
	ret = Usb_SetConfigurationApp(InstancePtr, Ctrl);
	if (XST_SUCCESS == ret)
	{
		if (GetConfigDone(InstancePtr->PrivateData))
		{
			USB_cdc_connected = 1;

			/** continue to send data **/
//			LOG_DEBUG("continue to send data\r\n");
			for (i = 0; i < USB_DEVICE_CHANNEL_LIMIT; i++)
			{
				pdev = &cdc_devices[i];
				if (pdev->opened)
				{
					BulkInHandler(InstancePtr, 0, 0, pdev);
				}
			}
//			LOG_DEBUG("%s[%d] usb connected\r\n", __func__, __LINE__);
		}
		else
		{
			USB_cdc_connected = 0;
		}
	}

	return 0;
}


//static Usb_dev_cdc_buffer_t cdc_buffers[USB_DEVICE_CHANNEL_LIMIT];

static USB_dev_cdc_t *get_cdc_device(int epnum)
{
	int i;
	
	for (i = 0; i < USB_DEVICE_CHANNEL_LIMIT; i++)
	{
		if (cdc_devices[i].binded_ep_num == epnum)
		{
			break;
		}
	}

	if (i >= USB_DEVICE_CHANNEL_LIMIT)
	{
		return NULL;
	}
	
	return &cdc_devices[i];
}


void USB_dev_set_recv_dma_buffer(void *InstancePtr, int epnum)
{	
	USB_dev_cdc_t *pcdc;

	pcdc = get_cdc_device(epnum);
	if (NULL == pcdc)
	{
//		LOG_ERROR("ERROR!!!!!\r\nepnum %d has no map to any cdc device\r\n", epnum);
	}
	else
	{
		pcdc->cdc_buffer.recv_dma_buffer_index = pcdc->cdc_buffer.recv_dma_buffer_index % USB_RECV_DMA_BUFFER_CNT;
		EpBufferRecv(InstancePtr, epnum, (u8 *)pcdc->cdc_buffer.recv_dma_buffer[pcdc->cdc_buffer.recv_dma_buffer_index], USB_DEV_RECV_DMA_BUFFER_SIZE);
//		LOG_DEBUG("MUST!! init ep%d recv DMA in\r\n", epnum);
	}
}


/****************************************************************************/
/**
* This function is Bulk Out Endpoint handler/Callback called by driver when
* data is received.
*
* @param	CallBackRef is pointer to Usb_DevData instance.
* @param	RequestedBytes is number of bytes requested for reception.
* @param	BytesTxed is actual number of bytes received from Host.
*
* @return	None
*
* @note		None.
*
*****************************************************************************/
static void BulkOutHandler(void *CallBackRef, u32 RequestedBytes, u32 BytesRxed, USB_dev_cdc_t *dev)
{
	struct Usb_DevData *InstancePtr = CallBackRef;
	Usb_dev_cdc_buffer_t *pbuffer = &dev->cdc_buffer;
	int i;
	int index;
	uint8_t *pdma_buffer;

	//xil_printf("%s in,ep:%d, got:%d/%d bytes\r\n", __func__, dev->binded_ep_num, RequestedBytes, BytesRxed);
	index = pbuffer->recv_dma_buffer_index % USB_RECV_DMA_BUFFER_CNT;
	pbuffer->recv_dma_buffer_index = (pbuffer->recv_dma_buffer_index + 1)%USB_RECV_DMA_BUFFER_CNT;
	
	EpBufferRecv(InstancePtr->PrivateData, dev->binded_ep_num, (u8 *)pbuffer->recv_dma_buffer[pbuffer->recv_dma_buffer_index], USB_DEV_RECV_DMA_BUFFER_SIZE);

	if ((BytesRxed > 0) && (dev->opened))
	{
		pdma_buffer = (uint8_t *)pbuffer->recv_dma_buffer[index];
		for (i = 0; i < BytesRxed; i++)
		{
			if (((pbuffer->recv_fifo_windex + 1)%USB_DEV_RECV_FIFO_SIZE) != pbuffer->recv_fifo_rindex)
			{
				pbuffer->recv_fifo[pbuffer->recv_fifo_windex] = pdma_buffer[i];
				pbuffer->recv_fifo_windex = (pbuffer->recv_fifo_windex + 1)%USB_DEV_RECV_FIFO_SIZE;
			}
			else
			{
//				LOG_ERROR("usb dev cdc recv fifo buffer is full, drop all recv data\r\n");
				break;
			}
		}
		//xil_printf("save %d byte to recv fifo\r\n", i);

		if (dev->recv_isr_cb)
		{
			dev->recv_isr_cb(dev->recv_cb_param, BytesRxed);
		}
	}
}

/****************************************************************************/
/**
* This function is Bulk In Endpoint handler/Callback called by driver when
* data is sent.
*
* @param	CallBackRef is pointer to Usb_DevData instance.
* @param	RequestedBytes is number of bytes requested to send.
* @param	BytesTxed is actual number of bytes sent to Host.
*
* @return	None
*
* @note		None.
*
*****************************************************************************/
static void BulkInHandler(void *CallBackRef, u32 RequestedBytes, u32 BytesTxed, USB_dev_cdc_t *dev)
{
	struct Usb_DevData *InstancePtr = CallBackRef;
	Usb_dev_cdc_buffer_t *pbuffer = &dev->cdc_buffer;
	int i;
	int index;
	int tmp;

	//xil_printf("%s in, ep:%d, %d bytes sent\r\n", __func__, dev->binded_ep_num, BytesTxed);
	if ((BytesTxed > 0) && (dev->opened))
	{
		pbuffer->send_fifo_rindex = (pbuffer->send_fifo_rindex + BytesTxed)%USB_DEV_SEND_FIFO_SIZE;
		
		if (dev->sent_isr_cb)
		{
			dev->sent_isr_cb(dev->sent_cb_param, BytesTxed);
		}
	}

	//xil_printf("send buffer rindex:%d, windex:%d\r\n", pbuffer->send_fifo_rindex, pbuffer->send_fifo_windex);

	#if 0
	for (i = 0, index = pbuffer->send_fifo_rindex; 
			( i < USB_DEV_SEND_DMA_BUFFER_SIZE) && (index != pbuffer->send_fifo_windex);i++)
	{
		pbuffer->send_dma_buffer[i] = pbuffer->send_fifo[index];
		index = (index + 1)%USB_DEV_SEND_FIFO_SIZE;
	}
	#else
	i = 0;
	if (pbuffer->send_fifo_rindex != pbuffer->send_fifo_windex)
	{
		if (pbuffer->send_fifo_rindex < pbuffer->send_fifo_windex)
		{
			i = pbuffer->send_fifo_windex - pbuffer->send_fifo_rindex;
			if (i > USB_DEV_SEND_DMA_BUFFER_SIZE)
			{
				i = USB_DEV_SEND_DMA_BUFFER_SIZE;
			}
			memcpy(pbuffer->send_dma_buffer, &pbuffer->send_fifo[pbuffer->send_fifo_rindex], i);
		}
		else
		{
			i = USB_DEV_SEND_FIFO_SIZE - pbuffer->send_fifo_rindex;
			if (i >= USB_DEV_SEND_DMA_BUFFER_SIZE)
			{
				i = USB_DEV_SEND_DMA_BUFFER_SIZE;
				memcpy(pbuffer->send_dma_buffer, &pbuffer->send_fifo[pbuffer->send_fifo_rindex], i);
			}
			else
			{
				memcpy(pbuffer->send_dma_buffer, &pbuffer->send_fifo[pbuffer->send_fifo_rindex], i);
				tmp = pbuffer->send_fifo_windex;
				if ((tmp + i) > USB_DEV_SEND_DMA_BUFFER_SIZE)
				{
					tmp = USB_DEV_SEND_DMA_BUFFER_SIZE - i;
				}
				memcpy(&pbuffer->send_dma_buffer[i], &pbuffer->send_fifo[0], tmp);
				i += tmp;
			}
		}
	}
	
	#endif

	if (i > 0)
	{
		dev->send_dma_working = 1;
		EpBufferSend(InstancePtr->PrivateData, dev->binded_ep_num, (u8 *)pbuffer->send_dma_buffer, i);
	}
	else
	{
		dev->send_dma_working = 0;
	}
}

static void Channel_InterruptHandler(void *CallBackRef, u32 RequestedBytes, u32 BytesRxed)
{
	//do nothing
}

static void Channel_0_BulkOutHandler(void *CallBackRef, u32 RequestedBytes, u32 BytesRxed)
{
	struct Usb_DevData *InstancePtr = CallBackRef;
	USBCH9_DATA *ch9_ptr = (USBCH9_DATA *) Get_DrvData(InstancePtr->PrivateData);
	USB_dev_cdc_t *dev = (USB_dev_cdc_t *)(ch9_ptr->data_ptr);
	
	BulkOutHandler(CallBackRef, RequestedBytes, BytesRxed, &dev[USB_DEVICE_CHANNEL_0]);
}

static void Channel_0_BulkInHandler(void *CallBackRef, u32 RequestedBytes, u32 BytesTxed)
{
	struct Usb_DevData *InstancePtr = CallBackRef;
	USBCH9_DATA *ch9_ptr = (USBCH9_DATA *) Get_DrvData(InstancePtr->PrivateData);
	USB_dev_cdc_t *dev = (USB_dev_cdc_t *)(ch9_ptr->data_ptr);
	
	BulkInHandler(CallBackRef, RequestedBytes, BytesTxed, &dev[USB_DEVICE_CHANNEL_0]);
}

#if (CONFIG_USB_DEC_CDC_NUM >= 2)
static void Channel_1_BulkOutHandler(void *CallBackRef, u32 RequestedBytes, u32 BytesRxed)
{
	struct Usb_DevData *InstancePtr = CallBackRef;
	USBCH9_DATA *ch9_ptr = (USBCH9_DATA *) Get_DrvData(InstancePtr->PrivateData);
	USB_dev_cdc_t *dev = (USB_dev_cdc_t *)(ch9_ptr->data_ptr);
	
	BulkOutHandler(CallBackRef, RequestedBytes, BytesRxed, &dev[USB_DEVICE_CHANNEL_1]);
}

static void Channel_1_BulkInHandler(void *CallBackRef, u32 RequestedBytes, u32 BytesTxed)
{
	struct Usb_DevData *InstancePtr = CallBackRef;
	USBCH9_DATA *ch9_ptr = (USBCH9_DATA *) Get_DrvData(InstancePtr->PrivateData);
	USB_dev_cdc_t *dev = (USB_dev_cdc_t *)(ch9_ptr->data_ptr);
	
	BulkInHandler(CallBackRef, RequestedBytes, BytesTxed, &dev[USB_DEVICE_CHANNEL_1]);
}
#endif


/****************************************************************************/
/**
* This function setups the interrupt system such that interrupts can occur.
* This function is application specific since the actual system may or may not
* have an interrupt controller.  The USB controller could be
* directly connected to a processor without an interrupt controller.
* The user should modify this function to fit the application.
*
* @param	InstancePtr is a pointer to the XUsbPsu instance.
* @param	UsbIntrId is the interrupt ID of the USB controller
*
* @return	None
*
* @note		None.
*
*****************************************************************************/
static void SetupInterruptSystem(struct XUsbPsu *InstancePtr, u16 UsbIntrId)
{
	xPortInstallInterruptHandler(UsbIntrId,
			(XInterruptHandler)XUsbPsu_IntrHandler,
			(void *)InstancePtr);

	XUsbPsu_EnableIntr(InstancePtr, XUSBPSU_DEVTEN_EVNTOVERFLOWEN |
			XUSBPSU_DEVTEN_WKUPEVTEN |
			XUSBPSU_DEVTEN_ULSTCNGEN |
			XUSBPSU_DEVTEN_CONNECTDONEEN |
			XUSBPSU_DEVTEN_USBRSTEN |
			XUSBPSU_DEVTEN_DISCONNEVTEN);

	vPortEnableInterrupt(UsbIntrId);
}

/****************************************************************************/
/**
* This function is implementing USB mass storage example.
*
* @param	UsbInstPtr USB instance pointer.
*		DeviceId USB Device ID
*		UsbIntrId USB Interrupt ID
*
* @return	- XST_SUCCESS if successful,
*		- XST_FAILURE if unsuccessful.
*
* @note		None.
*
*****************************************************************************/
int USB_dev_init(void)
{
	s32 Status;
	Usb_Config *UsbConfigPtr;
	struct Usb_DevData *UsbInstPtr = &UsbInstance;
	u16 DeviceId = USB_DEVICE_ID;
	u16 UsbIntrId = USB_INTR_ID;

	LOG_DEBUG("USB_dev_init...\r\n");
	memset(cdc_devices, 0, sizeof(cdc_devices));

	/* Initialize the USB driver so that it's ready to use,
	 * specify the controller ID that is generated in xparameters.h
	 */
	UsbConfigPtr = LookupConfig(DeviceId);
	if (NULL == UsbConfigPtr) {
//		LOG_ERROR("LookupConfig failed\r\n");
		return XST_FAILURE;
	}

	/* We are passing the physical base address as the third argument
	 * because the physical and virtual base address are the same in our
	 * example.  For systems that support virtual memory, the third
	 * argument needs to be the virtual base address.
	 */
	Status = CfgInitialize(UsbInstPtr, UsbConfigPtr,
			UsbConfigPtr->BaseAddress);
	if (XST_SUCCESS != Status) {
//		LOG_ERROR("CfgInitialize failed: %d\r\n", Status);
		return XST_FAILURE;
	}

	/* hook up chapter9 handler */
	Set_Ch9Handler(UsbInstPtr->PrivateData, Ch9Handler);

	/* Assign the data to usb driver */
	Set_DrvData(UsbInstPtr->PrivateData, &cdc_acm_data);

	Set_Disconnect(UsbInstPtr->PrivateData, UsbDisconnectCb);

	/*
	 * set endpoint handlers
	 * BulkOutHandler - to be called when data is received
	 * BulkInHandler -  to be called when data is sent
	 */
	#if (CONFIG_USB_DEC_CDC_NUM >= 1)
	SetEpHandler(UsbInstPtr->PrivateData, 1, USB_EP_DIR_OUT,
			Channel_0_BulkOutHandler);
	SetEpHandler(UsbInstPtr->PrivateData, 1, USB_EP_DIR_IN,
			Channel_0_BulkInHandler);
	SetEpHandler(UsbInstPtr->PrivateData, 2, USB_EP_DIR_IN,
			Channel_InterruptHandler);
	cdc_devices[USB_DEVICE_CHANNEL_0].binded_ep_num = 1;
	#endif

	#if (CONFIG_USB_DEC_CDC_NUM >= 2)
	SetEpHandler(UsbInstPtr->PrivateData, 3, USB_EP_DIR_OUT,
			Channel_1_BulkOutHandler);
	SetEpHandler(UsbInstPtr->PrivateData, 3, USB_EP_DIR_IN,
			Channel_1_BulkInHandler);
	SetEpHandler(UsbInstPtr->PrivateData, 4, USB_EP_DIR_IN,
			Channel_InterruptHandler);
	cdc_devices[USB_DEVICE_CHANNEL_1].binded_ep_num = 3;
	#endif

	/* setup interrupts */
	SetupInterruptSystem(UsbInstPtr->PrivateData, UsbIntrId);

	/* Start the controller so that Host can see our device */
	Usb_Start(UsbInstPtr->PrivateData);

	return XST_SUCCESS;
}


USB_dev_handle_t USB_dev_open(int channel)
{
	USB_dev_cdc_t *dev = NULL;
	
	if ((channel < 0) || (channel >= USB_DEVICE_CHANNEL_LIMIT))
	{
		return (USB_dev_handle_t)NULL;
	}

	vPortEnterCritical();
	dev = (USB_dev_cdc_t *)&cdc_devices[channel];

	if (!dev->opened)
	{
		dev->api_mutex = xSemaphoreCreateMutex();
		if (NULL == dev->api_mutex)
		{
			dev = NULL;
		}
		else
		{
			dev->opened = 1;
		}
	}
	
	vPortExitCritical();
	
	return (USB_dev_handle_t)dev;
}

void USB_dev_close(USB_dev_handle_t handle)
{
	USB_dev_cdc_t *dev = (USB_dev_cdc_t *)handle;
	int i;

	if (NULL == dev)
	{
		return;
	}

	for (i = 0; i < 500/portTICK_RATE_MS; i++) //max 500ms delay to wait data send over!
	{
		if (USB_cdc_connected && dev->opened)
		{
			if (dev->cdc_buffer.send_fifo_rindex != dev->cdc_buffer.send_fifo_windex)
			{
				vTaskDelay(1);
			}
		}
		else
		{
			break;
		}
	}

	vPortEnterCritical();
	if (dev->opened)
	{
		vSemaphoreDelete(dev->api_mutex);
		dev->api_mutex = NULL;
		dev->opened = 0;
		dev->sent_isr_cb = NULL;
		dev->sent_cb_param = NULL;
		dev->recv_isr_cb = NULL;
		dev->recv_cb_param = NULL;
		
		memset(&dev->cdc_buffer, 0, sizeof(dev->cdc_buffer));
	}
	vPortExitCritical();
}


int USB_dev_send(USB_dev_handle_t handle, const uint8_t *data, size_t size)
{
	int i;
	USB_dev_cdc_t *dev = (USB_dev_cdc_t *)handle;
	Usb_dev_cdc_buffer_t *pbuffer = &dev->cdc_buffer;
	int len = 0;

	if ((NULL == dev) || (NULL == data) || (size <= 0))
	{
		return -1;
	}
	
	if (!dev->opened)
	{
		return -1;
	}
//	while( dev->send_dma_working)
//	{
//		usleep(50);
//	}
	xSemaphoreTake(dev->api_mutex, portMAX_DELAY);
	for (i = 0; i < size; i++)
	{
		if (((pbuffer->send_fifo_windex + 1) % USB_DEV_SEND_FIFO_SIZE) != pbuffer->send_fifo_rindex)
		{
			pbuffer->send_fifo[pbuffer->send_fifo_windex] = data[i];
			pbuffer->send_fifo_windex = (pbuffer->send_fifo_windex + 1) % USB_DEV_SEND_FIFO_SIZE;
		}
		else
		{
			break;
		}
	}

	//if no data are sending, start to send.
	if (i > 0)
	{
		vPortEnterCritical();
		if (USB_cdc_connected)
		{
			if (!dev->send_dma_working)
			{
				BulkInHandler(&UsbInstance, 0, 0, dev);
			}
		}
		vPortExitCritical();
	}

	xSemaphoreGive(dev->api_mutex);
	
	return i;
}

/**
 * no block
 */
int USB_dev_recv(USB_dev_handle_t handle, uint8_t *data, size_t want_size)
{
	int i;
	USB_dev_cdc_t *dev = (USB_dev_cdc_t *)handle;
	Usb_dev_cdc_buffer_t *pbuffer = &dev->cdc_buffer;

	if ((NULL == dev) || (NULL == data) || (want_size <= 0))
	{
		return -1;
	}
	
	if (!dev->opened)
	{
		return -1;
	}

	xSemaphoreTake(dev->api_mutex, portMAX_DELAY);
	for (i = 0; (i < want_size) && (pbuffer->recv_fifo_rindex != pbuffer->recv_fifo_windex); i++)
	{
		data[i] = pbuffer->recv_fifo[pbuffer->recv_fifo_rindex];
		pbuffer->recv_fifo_rindex = (pbuffer->recv_fifo_rindex + 1) % USB_DEV_RECV_FIFO_SIZE;
	}
	xSemaphoreGive(dev->api_mutex);

	return i;
}

/**
 * the isr_cb run in ISR
 * SO, the call back function DO NOT sleep and run as fast as you can
 */
int USB_dev_set_sent_cb(USB_dev_handle_t handle, USB_dev_sent_cb_t isr_cb, void *cb_param)
{
	USB_dev_cdc_t *dev = (USB_dev_cdc_t *)handle;
	
	if (!dev->opened)
	{
		return -1;
	}

	xSemaphoreTake(dev->api_mutex, portMAX_DELAY);
	dev->sent_isr_cb = isr_cb;
	dev->sent_cb_param = cb_param;
	xSemaphoreGive(dev->api_mutex);

	return 0;
}

int USB_dev_set_recv_cb(USB_dev_handle_t handle, USB_dev_recv_cb_t isr_cb, void *cb_param)
{
	USB_dev_cdc_t *dev = (USB_dev_cdc_t *)handle;
	
	if (!dev->opened)
	{
		return -1;
	}

	xSemaphoreTake(dev->api_mutex, portMAX_DELAY);
	dev->recv_isr_cb = isr_cb;
	dev->recv_cb_param = cb_param;
	xSemaphoreGive(dev->api_mutex);
	
	return 0;
}





/****************************************************************************/
/**
* This task implements mass storage functionality
*
* @param	pvParameters private parameters.
*
* @note		None.
*
*****************************************************************************/

static void recv_cb(void *param, int size)
{
//	LOG_DEBUG("usb%d recv %d\r\n", *(int *)param, size);
}

static void sent_cb(void *param, int size)
{
//	LOG_DEBUG("usb%d sent %d\r\n", *(int *)param, size);
}


static void speed_cb(void *param, int size)
{
	*(int *)param += size;
}

static void USB_SendSpeedTest(int channel)
{
	USB_dev_handle_t handle;
	TickType_t time;
	static uint8_t data[1024*4];
	int len;
	uint64_t all;
	int send_bytes = 0;
	
	LOG_DEBUG("USB SendSpeedTest\r\n");
	
	handle = USB_dev_open(channel);
	sprintf(data, "send any key to start the test");
	USB_dev_send(handle, data, strlen(data));

	while(1)
	{
		len = USB_dev_recv(handle, data, 1);
		if (len > 0)
		{
			LOG_DEBUG("USB SendSpeedTest start...\r\n");
			break;
		}

		vTaskDelay(1);
	}

	memset(data, 'A', sizeof(data));
	all = 0;
	USB_dev_set_sent_cb(handle, speed_cb, &send_bytes);
	
	time = xTaskGetTickCount();
	while(1)
	{
		len = USB_dev_send(handle, data, sizeof(data));
		if (len > 0)
		{
			all += len;
			//xil_printf("send %d/%d\r\n", len, all);
		}
		else
		{
			//xil_printf("send fifo full\r\n");
		}

		if ((xTaskGetTickCount() - time) >= 10*100)
		{
			break;
		}

		//vTaskDelay(10);
	}

	USB_dev_close(handle);	
	LOG_DEBUG("send %d Bytes, %dMpbs,real send:%d\r\n", all, all*8/1000/1000/10, send_bytes);
}

static void USB_RecvSpeedTest(int channel)
{
	USB_dev_handle_t handle;
	TickType_t time;
	static uint8_t data[1024*4];
	int len;
	uint64_t all;
	
	LOG_DEBUG("USB RecvSpeedTest\r\n");
	
	handle = USB_dev_open(channel);
	sprintf(data, "send any key to start the test");
	USB_dev_send(handle, data, strlen(data));

	while(1)
	{
		len = USB_dev_recv(handle, data, 1);
		if (len > 0)
		{
			LOG_DEBUG("USB RecvSpeedTest start...\r\n");
			break;
		}

		vTaskDelay(1);
	}

	all = 1;
	
	time = xTaskGetTickCount();
	while(1)
	{
		len = USB_dev_recv(handle, data, sizeof(data));
		if (len > 0)
		{
			all += len;
			//xil_printf("send %d/%d\r\n", len, all);
		}

		if ((xTaskGetTickCount() - time) >= 10*100)
		{
			break;
		}

		//vTaskDelay(10);
	}

	USB_dev_close(handle);	
	LOG_DEBUG("recv %d Bytes, %dMpbs\r\n", all, all*8/1000/1000/10);
}

//alink_msg_t *GetAlinkSend_msg(void)
//{
//	return &sAlinkSend_msg;
//}

static void Timer_Callback(void)
{
	uRecvHeadCnt = 0;
	uRecvBufCnt = 0;
	uLen = 0;
}

static void Timer_UsbConnectCheck_Callback(void)
{
	static uint16_t tim_count_5s_per = 0;
	if (USB_cdc_connected == 0)
	{
		check_usb_disconnect_flag = 1;
	}
//	tim_count_5s_per++;
//	if (tim_count_5s_per >= 3)
//	{
//		tim_count_5s_per = 0;
//		GoToBootload();
//	}
}

static void UsbCloseAndResetAndReInit(void)
{
	USB_dev_handle_t *g_handle = NULL;
	g_handle = get_dev_handele_p();
	USB_dev_close(*g_handle);

	GPIO_OutputCtrl(MIO_PS_USB_RESET, IO_VOL);
	vTaskDelay(100);
	GPIO_OutputCtrl(MIO_PS_USB_RESET, IO_VOH);
	vTaskDelay(100);
	USB_dev_init();
	*g_handle = USB_dev_open(0);
}

static USB_dev_handle_t handle[USB_DEVICE_CHANNEL_LIMIT];

USB_dev_handle_t *get_dev_handele_p(void)
{
	return &handle[0];
}

static uint8_t buffer_data[1024 * 8];
static void usb_data_process(void)
{
//	int recv_param[USB_DEVICE_CHANNEL_LIMIT];
//	int sent_param[USB_DEVICE_CHANNEL_LIMIT];
	int len;
	int i;
	uint8_t ConnectedFlag = 0;

	for (i = 0; i < USB_DEVICE_CHANNEL_LIMIT; i++)
	{
		handle[i] = USB_dev_open(USB_DEVICE_CHANNEL_0+i);
//		recv_param[i] = i;
//		USB_dev_set_recv_cb(handle[i], recv_cb, &recv_param[i]);

//		sent_param[i] = i;
//		USB_dev_set_sent_cb(handle[i], sent_cb, &sent_param[i]);
	}
	while(1)
	{
		for (i = 0; i < USB_DEVICE_CHANNEL_LIMIT; i++)
		{
			len = USB_dev_recv(handle[i], buffer_data, sizeof(buffer_data));
			if (len <= 0)
			{
				vTaskDelay(1);
			}
			else
			{
				LOG_DEBUG("usb recv:%d\r\n", len);
/*******************************A20469*********************************************/
				usb_recv_proc(handle[i], buffer_data, len);
/*******************************A20469*********************************************/
			}

			if (check_usb_disconnect_flag == 1)
			{
				LOG_PRINTF("(5 seconds per cycle) detect usb unconnected, usb will reset and reinit\r\n ");
				UsbCloseAndResetAndReInit();
				check_usb_disconnect_flag = 0;
			}
		}

		if (USB_cdc_connected != ConnectedFlag)
		{
			ConnectedFlag = USB_cdc_connected;
			LOG_DEBUG("USB ConnectedFlag:%d\r\n", ConnectedFlag);
		}
	}

	for (i = 0; i < USB_DEVICE_CHANNEL_LIMIT; i++)
	{
		USB_dev_close(handle[i]);
	}
}

int usb_recv_proc(USB_dev_handle_t handle, uint8_t *buffdata, uint32_t EventData)
{
	sUsbCommand_msg_t msg;
	uint16_t crc;
	// todo  加互斥锁
	if (EventData > 0)
	{
		if (buffdata[0] == 0xfd)
		{
			if (uRecvHeadCnt < sizeof(alink_msg_head_t))
			{
				if ((EventData + uRecvHeadCnt) >= sizeof(alink_msg_head_t))
				{
					uint8_t len = sizeof(alink_msg_head_t) - uRecvHeadCnt;// header len
					uint8_t *buf = (uint8_t *)&sAlinkRecv_msg.msg_head;
					memcpy(buf + uRecvHeadCnt, buffdata, len);
					uRecvHeadCnt = sizeof(alink_msg_head_t);

					if (AlinkMsgProc_IsHeaderValid(&sAlinkRecv_msg.msg_head))
					{
						uLen = ((sAlinkRecv_msg.msg_head.len_hi << 8) + sAlinkRecv_msg.msg_head.len_lo);
						if ((EventData - len) > 0)
						{
							xTimerStart(Timer_Handle, 0);
							uRecvBufCnt = EventData - len;
							memcpy(sAlinkRecv_msg.buffer, buffdata + len, uRecvBufCnt);
							if (uRecvBufCnt >= uLen)
							{
								crc = (sAlinkRecv_msg.buffer[uLen + 1] << 8) + sAlinkRecv_msg.buffer[uLen];
								if (AlinkMsg_IsCrcValid((uint8_t *)&sAlinkRecv_msg, uLen + uRecvHeadCnt + 2, crc))
								{
									AlinkMsgProcTask_Post((uint32_t)&sAlinkRecv_msg);
									SetAlinkInterface(TYPE_C);
								}
								uRecvHeadCnt = 0;
								uRecvBufCnt = 0;
							}
						}
					}
					else
					{
						uRecvHeadCnt = 0;
						uRecvBufCnt = 0;
					}
				}
				else
				{
					uint8_t *buf = (uint8_t *)&sAlinkRecv_msg.msg_head;
					memcpy(buf + uRecvHeadCnt, buffdata, EventData);
					uRecvHeadCnt += EventData;
					xTimerStart(Timer_Handle, 0);
				}
			}
			else if (uRecvBufCnt < (uLen + ALINK_NUM_CHECKSUM_BYTES))
			{
				uint8_t *buf = (uint8_t *)&sAlinkRecv_msg.buffer;
				memcpy(buf + uRecvBufCnt, buffdata, EventData);
				uRecvBufCnt += EventData;
				xTimerStart(Timer_Handle, 0);
				if (uRecvBufCnt >= (uLen + ALINK_NUM_CHECKSUM_BYTES))
				{
					crc = (sAlinkRecv_msg.buffer[uLen + 1] << 8) + sAlinkRecv_msg.buffer[uLen];
					if (AlinkMsg_IsCrcValid((uint8_t *)&sAlinkRecv_msg, uLen + uRecvHeadCnt + 2, crc))
					{
						AlinkMsgProcTask_Post((uint32_t)&sAlinkRecv_msg);
						SetAlinkInterface(TYPE_C);
					}
					uRecvHeadCnt = 0;
					uRecvBufCnt = 0;
				}
			}
		}
		else
		{
			if (EventData < sizeof(msg.buf))
			{
				msg.length = EventData;
				memset(msg.buf, 0, sizeof(msg.buf));
				memcpy(msg.buf, buffdata, EventData);
//				uint8_t aa = strlen((char)msg.buf);
//				if ((EventData - strlen((char)msg.buf)) < 10)
				{
					UsbCommandServer_Post(&msg);
				}
			}
		}
	}
	return 0;
}

//static void prvMainTask(void *pvParameters)
//{
//	s32 Status;
//
//	Status = USB_dev_init();
//	if (Status == XST_FAILURE) {
//		xil_printf("USB_dev_init failed\r\n");
//		vTaskDelete(NULL);
//	}
//
//	while(1)
//	{
//		//USB_SendSpeedTest(USB_DEVICE_CHANNEL_0);
//		//USB_RecvSpeedTest(USB_DEVICE_CHANNEL_0);
//		usb_echo_test();
//		vTaskDelay(1);
//	}
//
////	while(1)
////	{
////		vTaskDelay(1);
////	}
//}

static void UsbProcessTask(void *pvParameters)
{
	s32 Status;

	Status = USB_dev_init();
	if (Status == XST_FAILURE) {
//		LOG_ERROR("USB_dev_init failed\r\n");
		vTaskDelete(NULL);
	}
	xTimerStart(Timer_UsbConnectCheck_Handle, 0);
	while(1)
	{
		usb_data_process();
		vTaskDelay(1);
	}

}

/****************************************************************************/
/**
* This function is the main function of the USB mass storage example.
*
* @param	None.
*
* @return
*		- XST_SUCCESS if successful,
*		- XST_FAILURE if unsuccessful.
*
* @note		None.
*
*
*****************************************************************************/
//int main(void)
//{
//	CacheInit();
//
//	xTaskCreate(prvMainTask, (const char *) "Main Task",
//			configMINIMAL_STACK_SIZE*10, NULL, tskIDLE_PRIORITY + 2,
//			NULL);
//
//	/* Start the tasks and timer running. */
//	vTaskStartScheduler();
//
//	return XST_SUCCESS;
//}

int Usb_Init_process(void)
{
	int32_t ret = RET_OK;
	int32_t *pvTimerID = NULL;

	CacheInit();

	UsbSeamphore = xSemaphoreCreateMutex();
	pvTimerID = (int32_t *)1;
	Timer_Handle = xTimerCreate((const char *)"Timer",
								(TickType_t)10,
								(UBaseType_t)pdFALSE,
								(void *)pvTimerID,
								(TimerCallbackFunction_t)Timer_Callback);
	if (Timer_Handle == NULL)
	{
//		LOG_ERROR("create Timer error\r\n");
		ret = -1;
		return ret;
	}

	Timer_UsbConnectCheck_Handle = xTimerCreate((const char *)"Timer",
								(TickType_t)5000,
								(UBaseType_t)pdTRUE,
								(void *)pvTimerID,
								(TimerCallbackFunction_t)Timer_UsbConnectCheck_Callback);
	if (Timer_UsbConnectCheck_Handle == NULL)
	{
//		LOG_ERROR("create Timer error\r\n");
		ret = -1;
		return ret;
	}


	if (xTaskCreate(UsbProcessTask, (const char *) "Usb_Task",
			TASK_STACK_SIZE_USB_DATA_ROCESS, NULL, TASK_PRI_USB,
			NULL) != pdPASS)
	{
	}


	/* Start the tasks and timer running. */
//	vTaskStartScheduler();

	return XST_SUCCESS;
}

void UsbPwrInitSet(void)
{
	GPIO_OutputCtrl(MIO_USB_5V_EN, IO_VOH);
	GPIO_OutputCtrl(MIO_PS_USB_CS, IO_VOH);
	GPIO_OutputCtrl(MIO_PS_USB_RESET, IO_VOH);
}

int InitUsbProcess_Task(void)
{
	UsbPwrInitSet();
	Usb_Init_process();
	return 1;
}

uint8_t UsbDataPrintf = 0;
void SetUsbDataPrintf(uint8_t value)
{
	UsbDataPrintf = value;
}
uint8_t GetUsbDataPrintf(void)
{
	return UsbDataPrintf;
}

int Usb_Send_data_func(uint8_t *data, uint32_t len)
{
	int i;
	int ret = 0;
	uint32_t CntLen = 0;
	uint32_t SendCnt = 0;
	uint32_t timeout = 0xFFFFFFFF;

	if (USB_cdc_connected == 0)
	{
		return ret;
	}
	if (UsbSeamphore != NULL)
	{
		xSemaphoreTake(UsbSeamphore,portMAX_DELAY);
	}
	for (i = 0; i < USB_DEVICE_CHANNEL_LIMIT; i++)
	{
		while(timeout--)
		{
			CntLen = USB_dev_send(handle[i], (const uint8_t *)data + SendCnt, len - SendCnt);
			SendCnt += CntLen;
			if (SendCnt >= len)
			{
				break;
			}
		}
	}
	if (UsbSeamphore != NULL)
	{
		xSemaphoreGive(UsbSeamphore);
	}
	return ret;
}

int UsbSendDataFunc(uint8_t *data, uint32_t len)
{
	int ret = 0;

	if ((UsbDataPrintf & 0x0F) == 1)
	{
		LOG_PRINTF("USB_dev_send:%d\r\n", USB_cdc_connected);
		for (uint16_t i = 0; i < len; i++)
		{
			LOG_PRINTF("%02X ", data[i]);
		}
		LOG_PRINTF("\r\n");
	}

	if ((UsbDataPrintf & 0xF0) == 0)
	{
		Usb_Send_data_func(data, len);
	}

	return ret;
}
