/******************************************************************************
* Copyright (C) 2018 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file xusb_freertos_ch9_storage.c
 *
 * This file contains the implementation of the Mass Storage specific chapter 9
 * code for the example.
 *
 *<pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -------------------------------------------------------
 * 1.0   rb   22/03/18 First release
 * 1.5   vak  03/25/19 Fixed incorrect data_alignment pragma directive for IAR
 *
 *</pre>
 *
 ******************************************************************************/

/***************************** Include Files *********************************/
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "xparameters.h"		/* XPAR parameters */
#include "xusb_ch9_cdc_acm.h"
#include "xusb_class_cdc_acm.h"
#include "xusb_cdc_config.h"
#include "../../srv/log/log.h"

/************************** Constant Definitions *****************************/

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Function Prototypes ******************************/
extern void USB_dev_set_recv_dma_buffer(void *InstancePtr, int epnum);

/************************** Variable Definitions *****************************/

/* Device Descriptors */
#ifdef __ICCARM__
#pragma data_alignment = 16
#endif

#ifdef __ICCARM__
USB_STD_DEV_DESC deviceDesc[] = {
#else
USB_STD_DEV_DESC __attribute__ ((aligned(16))) deviceDesc[] = {
#endif
	{
		/* USB 2.0 */
		sizeof(USB_STD_DEV_DESC),	/* bLength */
		USB_TYPE_DEVICE_DESC,		/* bDescriptorType */
		0x0200,				/* bcdUSB 2.0 */

		#if (CONFIG_USB_DEC_CDC_NUM == 1)
		USB_CLASS_CDC,		/* bDeviceClass */
		0x00,				/* bDeviceSubClass */
		0x00,				/* bDeviceProtocol */
		#else //composite device
		0xEF,		        /* bDeviceClass */
		0x02,				/* bDeviceSubClass */
		0x01,				/* bDeviceProtocol */
		#endif
		
		0x40,				/* bMaxPackedSize0 */
		0x03Fd,				/* idVendor */
		0x0500,				/* idProduct */
		0x0100,				/* bcdDevice */
		0x01,				/* iManufacturer */
		0x02,				/* iProduct */
		0x03,				/* iSerialNumber */
		0x01				/* bNumConfigurations */
	},
	{
		/* USB 3.0 */
		sizeof(USB_STD_DEV_DESC),	/* bLength */
		USB_TYPE_DEVICE_DESC,		/* bDescriptorType */
		0x0300,				/* bcdUSB 3.0 */
		0x00,				/* bDeviceClass */
		0x00,				/* bDeviceSubClass */
		0x00,				/* bDeviceProtocol */
		0x09,				/* bMaxPackedSize0 */
		0x0525,				/* idVendor */
		0xA4A5,				/* idProduct */
		0x0404,				/* bcdDevice */
		0x04,				/* iManufacturer */
		0x05,				/* iProduct */
		0x06,				/* iSerialNumber */
		0x01				/* bNumConfigurations */
	}
};


/* Configuration Descriptors */
#ifdef __ICCARM__
USB30_CONFIG config3 = {
#else
USB30_CONFIG __attribute__ ((aligned(16))) config3 = {
#endif
	{
		/* Std Config */
		sizeof(USB_STD_CFG_DESC),	/* bLength */
		USB_TYPE_CONFIG_DESC,		/* bDescriptorType */
		sizeof(USB30_CONFIG),		/* wTotalLength */
		0x01,				/* bNumInterfaces */
		0x01,				/* bConfigurationValue */
		0x00,				/* iConfiguration */
		0xc0,				/* bmAttribute */
		0x00				/* bMaxPower  */
	},
	{
		/* CDC Standard Interface Descriptor */
		sizeof(USB_STD_IF_DESC),	/* bLength */
		USB_TYPE_INTERFACE_DESC,	/* bDescriptorType */
		0x00,				/* bInterfaceNumber */
		0x00,				/* bAlternateSetting */
		0x02,				/* bNumEndPoints */
		USB_CLASS_CDC,		/* bInterfaceClass */
		0x00,				/* bInterfaceSubClass */
		0x00,				/* bInterfaceProtocol */
		0x01				/* iInterface */
	},
	{
		/* Bulk In Endpoint Config */
		sizeof(USB_STD_EP_DESC),	/* bLength */
		USB_TYPE_ENDPOINT_CFG_DESC,	/* bDescriptorType */
		USB_EP1_IN,			/* bEndpointAddress */
		0x02,				/* bmAttribute  */
		0x00,				/* wMaxPacketSize - LSB */
		0x04,				/* wMaxPacketSize - MSB */
		0x00				/* bInterval */
	},
	{
		/* SS Endpoint companion */
		sizeof(USB_STD_EP_SS_COMP_DESC),/* bLength */
		0x30,				/* bDescriptorType */
		0x0F,				/* bMaxBurst */
		0x00,				/* bmAttributes */
		0x00				/* wBytesPerInterval */
	},
	{
		/* Bulk Out Endpoint Config */
		sizeof(USB_STD_EP_DESC),	/* bLength */
		USB_TYPE_ENDPOINT_CFG_DESC,	/* bDescriptorType */
		USB_EP1_OUT,			/* bEndpointAddress */
		0x02,				/* bmAttribute */
		0x00,				/* wMaxPacketSize - LSB */
		0x04,				/* wMaxPacketSize - MSB */
		0x00				/* bInterval */
	},
	{
		/* SS Endpoint companion */
		sizeof(USB_STD_EP_SS_COMP_DESC),/* bLength */
		0x30,				/* bDescriptorType */
		0x0F,				/* bMaxBurst */
		0x00,				/* bmAttributes */
		0x00				/* wBytesPerInterval */
	}
};

/************************************************/
#define  USB_DESC_TYPE_DEVICE                           0x01U
#define  USB_DESC_TYPE_CONFIGURATION                    0x02U
#define  USB_DESC_TYPE_STRING                           0x03U
#define  USB_DESC_TYPE_INTERFACE                        0x04U
#define  USB_DESC_TYPE_ENDPOINT                         0x05U
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                 0x06U
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION        0x07U
#define  USB_DESC_TYPE_BOS                              0x0FU

#if (CONFIG_USB_DEC_CDC_NUM == 1)
#define USB_CDC_CONFIG_DESC_SIZ                     67U
#define USB_INTERFACE_NUMBER                        2
#endif

#if (CONFIG_USB_DEC_CDC_NUM == 2)
#define USB_CDC_CONFIG_DESC_SIZ                     141U 
#define USB_INTERFACE_NUMBER                        4
#endif

#if (CONFIG_USB_DEC_CDC_NUM == 3)
#define USB_CDC_CONFIG_DESC_SIZ                     207U 
#define USB_INTERFACE_NUMBER                        6
#endif

#if ((CONFIG_USB_DEC_CDC_NUM > 3) || (CONFIG_USB_DEC_CDC_NUM <= 0))
#error "CONFIG_USB_DEC_CDC_NUM is not 1~3"
#endif

static const uint8_t __attribute__ ((aligned(16))) CDC_ACM_config[USB_CDC_CONFIG_DESC_SIZ] = 
{
	/* Configuration Descriptor */
	sizeof(USB_STD_CFG_DESC),					  /* bLength: Configuration Descriptor size */
	USB_TYPE_CONFIG_DESC,				         /* bDescriptorType: Configuration */
	USB_CDC_CONFIG_DESC_SIZ,					  /* wTotalLength:no of returned bytes */
	0x00,
	USB_INTERFACE_NUMBER, 					/* bNumInterfaces: 2 interface */
	0x01, 									  /* bConfigurationValue: Configuration value */
	0x00, 									  /* iConfiguration: Index of string descriptor describing the configuration */
	0xC0, 									  /* bmAttributes: self powered */
	0x32, 									  /* MaxPower 0 mA */

	/*---------------------------------------------------------------------------*/
	#if (CONFIG_USB_DEC_CDC_NUM > 1)
	/* Interface Association Descriptor: CDC device (virtual com port) */
	0x08, /* bLength: IAD size */
	0x0B, /* bDescriptorType: Interface Association Descriptor */
	0x00, /* bFirstInterface */
	0x02, /* bInterfaceCount */
	0x02, /* bFunctionClass: Communiction Interface Class */
	0x02, /* bFunctionSubClass: Abstract Control Model */
	0x01, /* bFunctionProtocol: Common AT commands */
	0x06, /* iFunction */
	#endif

	/* Interface Descriptor */
	0x09, 									  /* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,					  /* bDescriptorType: Interface */
	/* Interface descriptor type */
	0x00, 									  /* bInterfaceNumber: Number of Interface */
	0x00, 									  /* bAlternateSetting: Alternate setting */
	0x01, 									  /* bNumEndpoints: One endpoints used */
	0x02, 									  /* bInterfaceClass: Communication Interface Class */
	0x02, 									  /* bInterfaceSubClass: Abstract Control Model */
	0x01, 									  /* bInterfaceProtocol: Common AT commands */
	0x00, 									  /* iInterface: */

	/* Header Functional Descriptor */
	0x05, 									  /* bLength: Endpoint Descriptor size */
	0x24, 									  /* bDescriptorType: CS_INTERFACE */
	0x00, 									  /* bDescriptorSubtype: Header Func Desc */
	0x10, 									  /* bcdCDC: spec release number */
	0x01,

	/* Call Management Functional Descriptor */
	0x05, 									  /* bFunctionLength */
	0x24, 									  /* bDescriptorType: CS_INTERFACE */
	0x01, 									  /* bDescriptorSubtype: Call Management Func Desc */
	0x00, 									  /* bmCapabilities: D0+D1 */
	0x01, 									  /* bDataInterface: 1 */

	/* ACM Functional Descriptor */
	0x04, 									  /* bFunctionLength */
	0x24, 									  /* bDescriptorType: CS_INTERFACE */
	0x02, 									  /* bDescriptorSubtype: Abstract Control Management desc */
	0x02, 									  /* bmCapabilities */

	/* Union Functional Descriptor */
	0x05, 									  /* bFunctionLength */
	0x24, 									  /* bDescriptorType: CS_INTERFACE */
	0x06, 									  /* bDescriptorSubtype: Union func desc */
	0x00, 									  /* bMasterInterface: Communication class interface */
	0x01, 									  /* bSlaveInterface0: Data Class Interface */
	  
	/* Endpoint 2 Descriptor */
	sizeof(USB_STD_EP_DESC),	/* bLength */
	USB_TYPE_ENDPOINT_CFG_DESC,	/* bDescriptorType */
	USB_EP2_IN,			/* bEndpointAddress */
	0x03,				/* bmAttribute:Interrupt  */
	0x00,				/* wMaxPacketSize - LSB */
	0x04,				/* wMaxPacketSize - MSB */
	0x10,				/* bInterval */
	/*---------------------------------------------------------------------------*/

	/* Data class interface descriptor */
	/* CDC Standard Interface Descriptor */
	sizeof(USB_STD_IF_DESC),	/* bLength */
	USB_TYPE_INTERFACE_DESC,	/* bDescriptorType */
	0x01,				/* bInterfaceNumber */
	0x00,				/* bAlternateSetting */
	0x02,				/* bNumEndPoints */
	USB_CLASS_CDC,		/* bInterfaceClass */
	0x00,				/* bInterfaceSubClass */
	0x00,				/* bInterfaceProtocol */
	0x00,				/* iInterface */

	/* Bulk Out Endpoint Config */
	sizeof(USB_STD_EP_DESC),	/* bLength */
	USB_TYPE_ENDPOINT_CFG_DESC,	/* bDescriptorType */
	USB_EP1_OUT,			/* bEndpointAddress */
	0x02,				/* bmAttribute:BLUK  */
	0x00,				/* wMaxPacketSize - LSB */
	0x02,				/* wMaxPacketSize - MSB */
	0x00,				/* bInterval */

	/* Bulk In Endpoint Config */
	sizeof(USB_STD_EP_DESC),	/* bLength */
	USB_TYPE_ENDPOINT_CFG_DESC,	/* bDescriptorType */
	USB_EP1_IN,			/* bEndpointAddress */
	0x02,				/* bmAttribute: BLUK  */
	0x00,				/* wMaxPacketSize - LSB */
	0x02,				/* wMaxPacketSize - MSB */
	0x00,				/* bInterval */

	#if (CONFIG_USB_DEC_CDC_NUM >= 2)
	/*---------------------------------------------------------------------------*/
	/* Interface Association Descriptor: CDC device (virtual com port) */
	0x08, /* bLength: IAD size */
	0x0B, /* bDescriptorType: Interface Association Descriptor */
	0x02, /* bFirstInterface */
	0x02, /* bInterfaceCount */
	0x02, /* bFunctionClass: Communiction Interface Class */
	0x02, /* bFunctionSubClass: Abstract Control Model */
	0x01, /* bFunctionProtocol: Common AT commands */
	0x06, /* iFunction */

	/* Interface Descriptor */
	0x09, 									  /* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,					  /* bDescriptorType: Interface */
	/* Interface descriptor type */
	0x02, 									  /* bInterfaceNumber: Number of Interface */
	0x00, 									  /* bAlternateSetting: Alternate setting */
	0x01, 									  /* bNumEndpoints: One endpoints used */
	0x02, 									  /* bInterfaceClass: Communication Interface Class */
	0x02, 									  /* bInterfaceSubClass: Abstract Control Model */
	0x01, 									  /* bInterfaceProtocol: Common AT commands */
	0x00, 									  /* iInterface: */

	/* Header Functional Descriptor */
	0x05, 									  /* bLength: Endpoint Descriptor size */
	0x24, 									  /* bDescriptorType: CS_INTERFACE */
	0x00, 									  /* bDescriptorSubtype: Header Func Desc */
	0x10, 									  /* bcdCDC: spec release number */
	0x01,

	/* Call Management Functional Descriptor */
	0x05, 									  /* bFunctionLength */
	0x24, 									  /* bDescriptorType: CS_INTERFACE */
	0x01, 									  /* bDescriptorSubtype: Call Management Func Desc */
	0x00, 									  /* bmCapabilities: D0+D1 */
	0x01, 									  /* bDataInterface: 1 */

	/* ACM Functional Descriptor */
	0x04, 									  /* bFunctionLength */
	0x24, 									  /* bDescriptorType: CS_INTERFACE */
	0x02, 									  /* bDescriptorSubtype: Abstract Control Management desc */
	0x02, 									  /* bmCapabilities */

	/* Union Functional Descriptor */
	0x05, 									  /* bFunctionLength */
	0x24, 									  /* bDescriptorType: CS_INTERFACE */
	0x06, 									  /* bDescriptorSubtype: Union func desc */
	0x00, 									  /* bMasterInterface: Communication class interface */
	0x01, 									  /* bSlaveInterface0: Data Class Interface */

	/* Endpoint 2 Descriptor */
	sizeof(USB_STD_EP_DESC),	/* bLength */
	USB_TYPE_ENDPOINT_CFG_DESC,	/* bDescriptorType */
	USB_DIR_IN|0x04,			/* bEndpointAddress */
	0x03,				/* bmAttribute:Interrupt  */
	0x00,				/* wMaxPacketSize - LSB */
	0x04,				/* wMaxPacketSize - MSB */
	0x10,				/* bInterval */
	/*---------------------------------------------------------------------------*/

	/* Data class interface descriptor */
	/* CDC Standard Interface Descriptor */
	sizeof(USB_STD_IF_DESC),	/* bLength */
	USB_TYPE_INTERFACE_DESC,	/* bDescriptorType */
	0x03,				/* bInterfaceNumber */
	0x00,				/* bAlternateSetting */
	0x02,				/* bNumEndPoints */
	USB_CLASS_CDC,		/* bInterfaceClass */
	0x00,				/* bInterfaceSubClass */
	0x00,				/* bInterfaceProtocol */
	0x00,				/* iInterface */

	/* Bulk Out Endpoint Config */
	sizeof(USB_STD_EP_DESC),	/* bLength */
	USB_TYPE_ENDPOINT_CFG_DESC,	/* bDescriptorType */
	0x03,//USB_EP1_OUT,			/* bEndpointAddress */
	0x02,				/* bmAttribute:BLUK  */
	0x00,				/* wMaxPacketSize - LSB */
	0x02,				/* wMaxPacketSize - MSB */
	0x00,				/* bInterval */

		/* Bulk In Endpoint Config */
	sizeof(USB_STD_EP_DESC),	/* bLength */
	USB_TYPE_ENDPOINT_CFG_DESC,	/* bDescriptorType */
	USB_DIR_IN|0x03,//USB_EP1_IN,			/* bEndpointAddress */
	0x02,				/* bmAttribute: BLUK  */
	0x00,				/* wMaxPacketSize - LSB */
	0x02,				/* wMaxPacketSize - MSB */
	0x00,				/* bInterval */
	#endif
};

/* String Descriptors */
static u8 StringList[2][6][128] = {
	{
		"UNUSED",
		"AUTEL",
		"USB2.0 Virtual COM Port",
		"SN0123456",
		"CDC ACM Gadget",
		"Default Interface"
	},
	{
		"AUTEL base on Xilinx standalone",
		"CDC ACM",
		"2A49876D9CC1AA4",
		"USB 3.0 CDC ACM device",
		"CDC ACM Gadget",
		"7ABC7ABC7ABC7ABC7ABC7ABC"
	}
};

/*****************************************************************************/
/**
*
* This function returns the device descriptor for the device.
*
* @param	InstancePtr is a pointer to the Usb_DevData instance.
* @param	BufPtr is pointer to the buffer that is to be filled
*		with the descriptor.
* @param	BufLen is the size of the provided buffer.
*
* @return	Length of the descriptor in the buffer on success.
*		0 on error.
*
******************************************************************************/
u32 Usb_Ch9SetupDevDescReply(struct Usb_DevData *InstancePtr,
		u8 *BufPtr, u32 BufLen)
{
	u8 Index;
	s32 Status;

	Status = IsSuperSpeed(InstancePtr);
	if (Status != XST_SUCCESS) {
		/* USB 2.0 */
		Index = 0;
	} else {
		/* USB 3.0 */
		Index = 1;
	}

	if (!BufPtr || BufLen < sizeof(USB_STD_DEV_DESC))
		return 0;

	memcpy(BufPtr, &deviceDesc[Index], sizeof(USB_STD_DEV_DESC));

	return sizeof(USB_STD_DEV_DESC);
}


/*****************************************************************************/
/**
*
* This function returns the configuration descriptor for the device.
*
* @param	InstancePtr is a pointer to the Usb_DevData instance.
* @param	BufPtr is the pointer to the buffer that is to be filled with
*		the descriptor.
* @param	BufLen is the size of the provided buffer.
*
* @return	Length of the descriptor in the buffer on success.
*		0 on error.
*
******************************************************************************/
u32 Usb_Ch9SetupCfgDescReply(struct Usb_DevData *InstancePtr,
		u8 *BufPtr, u32 BufLen)
{
	s32 Status;
	u8 *config;
	u32 CfgDescLen;

	Status = IsSuperSpeed(InstancePtr);
	if (Status != XST_SUCCESS) {
		/* USB 2.0 */
		//config = (u8 *)&config2;
		//CfgDescLen  = sizeof(USB_CONFIG);
		config = (u8 *)CDC_ACM_config;
		CfgDescLen  = sizeof(CDC_ACM_config);
	} else {
		/* USB 3.0 */
		config = (u8 *)&config3;
		CfgDescLen  = sizeof(USB30_CONFIG);
	}

	if (!BufPtr || BufLen < sizeof(USB_STD_CFG_DESC))
		return 0;

	memcpy(BufPtr, config, CfgDescLen);

	return CfgDescLen;
}


/*****************************************************************************/
/**
*
* This function returns a string descriptor for the given index.
*
* @param	InstancePtr is a pointer to the Usb_DevData instance.
* @param	BufPtr is a  pointer to the buffer that is to be filled with
*		the descriptor.
* @param	BufLen is the size of the provided buffer.
* @param	Index is the index of the string for which the descriptor
*		is requested.
*
* @return	Length of the descriptor in the buffer on success.
*		0 on error.
*
******************************************************************************/
u32 Usb_Ch9SetupStrDescReply(struct Usb_DevData *InstancePtr,
		u8 *BufPtr, u32 BufLen, u8 Index)
{
	u32 i;
	char *String;
	u32 StringLen;
	u32 DescLen;
	u8 TmpBuf[128];
	s32 Status;
	u8 StrArray;

	USB_STD_STRING_DESC *StringDesc;

	Status = IsSuperSpeed(InstancePtr);
	if (Status != XST_SUCCESS) {
		/* USB 2.0 */
		StrArray = 0;
	} else {
		/* USB 3.0 */
		StrArray = 1;
	}

	if (!BufPtr)
		return 0;

	//xil_printf("string index:%d\r\n", Index);

	String = (char *)&StringList[StrArray][Index];

	if (Index >= sizeof(StringList) / sizeof(u8 *))
		return 0;

	StringLen = strlen(String);

	StringDesc = (USB_STD_STRING_DESC *) TmpBuf;

	/* Index 0 is special as we can not represent the string required in
	 * the table above. Therefore we handle index 0 as a special case.
	 */
	if (0 == Index) {
		StringDesc->bLength = 4;
		StringDesc->bDescriptorType = 0x03;
		StringDesc->wLANGID[0] = 0x0409;/* LangID = 0x0409: U.S. English */
	}
	/* All other strings can be pulled from the table above. */
	else {
		StringDesc->bLength = StringLen * 2 + 2;
		StringDesc->bDescriptorType = 0x03;

		for (i = 0; i < StringLen; i++)
			StringDesc->wLANGID[i] = (u16) String[i];
	}
	DescLen = StringDesc->bLength;

	/* Check if the provided buffer is big enough to hold the descriptor. */
	if (DescLen > BufLen)
		return 0;

	memcpy(BufPtr, StringDesc, DescLen);

	return DescLen;
}

/*****************************************************************************/
/**
*
* This function returns the BOS descriptor for the device.
*
* @param	BufPtr is the pointer to the buffer that is to be filled with
*		the descriptor.
* @param	BufLen is the size of the provided buffer.
*
* @return	Length of the descriptor in the buffer on success.
*		0 on error.
*
******************************************************************************/
u32 Usb_Ch9SetupBosDescReply(u8 *BufPtr, u32 BufLen)
{
#ifdef __ICCARM__
#pragma data_alignment = 16
	static USB_BOS_DESC bosDesc = {
#else
	static USB_BOS_DESC __attribute__ ((aligned(16))) bosDesc = {
#endif
		/* BOS descriptor */
		{sizeof(USB_STD_BOS_DESC),	/* bLength */
			USB_TYPE_BOS_DESC,	/* DescriptorType */
			sizeof(USB_BOS_DESC),	/* wTotalLength */
			0x02},			/* bNumDeviceCaps */

		{sizeof(USB_STD_DEVICE_CAP_7BYTE), /* bLength */
			0x10,			/* bDescriptorType */
			0x02,			/* bDevCapabiltyType */
			0x06},			/* bmAttributes */

		{sizeof(USB_STD_DEVICE_CAP_10BYTE), /* bLength */
			0x10,			/* bDescriptorType */
			0x03,			/* bDevCapabiltyType */
			0x00,			/* bmAttributes */
			(0x000F),		/* wSpeedsSupported */
			0x01,			/* bFunctionalitySupport */
			0x01,			/* bU1DevExitLat */
			(0x01F4)}		/* wU2DevExitLat */
	};

	/* Check buffer pointer is OK and buffer is big enough. */
	if (!BufPtr || BufLen < sizeof(USB_STD_BOS_DESC))
		return 0;

	memcpy(BufPtr, &bosDesc, sizeof(USB_BOS_DESC));

	return sizeof(USB_BOS_DESC);
}

/****************************************************************************/
/**
* Changes State of Core to USB configured State.
*
* @param	InstancePtr is a pointer to the Usb_DevData instance.
* @param	Ctrl is a pointer to the Setup packet data.
*
* @return	XST_SUCCESS else XST_FAILURE
*
* @note		None.
*
*****************************************************************************/
s32 Usb_SetConfiguration(struct Usb_DevData *InstancePtr, SetupPacket *Ctrl)
{
	u8 State;
	s32 Ret = XST_SUCCESS;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(Ctrl != NULL);

	State = InstancePtr->State;
	SetConfigDone(InstancePtr->PrivateData, 0U);

	switch (State) {
		case USB_STATE_DEFAULT:
			Ret = XST_FAILURE;
			break;

		case USB_STATE_ADDRESS:
			InstancePtr->State = USB_STATE_CONFIGURED;
			break;

		case USB_STATE_CONFIGURED:
			break;

		default:
			Ret = XST_FAILURE;
			break;
	}

	return Ret;
}

/****************************************************************************/
/**
* This function is called by Chapter9 handler when SET_CONFIGURATION command
* is received from Host.
*
* @param	InstancePtr is pointer to Usb_DevData instance.
* @param	SetupData is the setup packet received from Host.
*
* @return
*		- XST_SUCCESS if successful,
*		- XST_FAILURE if unsuccessful.
*
* @note
*		Non control endpoints must be enabled after SET_CONFIGURATION
*		command since hardware clears all previously enabled endpoints
*		except control endpoints when this command is received.
*
*****************************************************************************/
s32 Usb_SetConfigurationApp(struct Usb_DevData *InstancePtr,
		SetupPacket *SetupData)
{
	s32 ret;

	/* When we run CV test suite application in Windows, need to
	 * add SET_CONFIGURATION command with value 0/1 to pass test suite
	 */
//	LOG_DEBUG("%s in, wValue is %02X\r\n", __func__, SetupData->wValue);
	
	if ((SetupData->wValue && 0xff) ==  1) {

		u16 MaxPktSize;
		
		if (InstancePtr->Speed == USB_SPEED_SUPER)
			MaxPktSize = 1024;
		else
			MaxPktSize = 512;

		SetConfigDone(InstancePtr->PrivateData, 1U);

		#if (CONFIG_USB_DEC_CDC_NUM >= 1)
		EpClearStall(InstancePtr->PrivateData, 2, USB_EP_DIR_IN);
		ret = EpEnable(InstancePtr->PrivateData, 2, USB_EP_DIR_IN, 1024, USB_EP_TYPE_INTERRUPT);
//		LOG_DEBUG("enable ep2:%d\r\n", ret);

		/* Endpoint enables - not needed for Control EP */
		EpDisable(InstancePtr->PrivateData, 1, USB_EP_DIR_IN);
		EpClearStall(InstancePtr->PrivateData, 1, USB_EP_DIR_IN);
		ret = EpEnable(InstancePtr->PrivateData, 1, USB_EP_DIR_IN, MaxPktSize, USB_EP_TYPE_BULK);
//		LOG_DEBUG("enable ep1 in:%d\r\n", ret);

		EpDisable(InstancePtr->PrivateData, 1, USB_EP_DIR_OUT);
		EpClearStall(InstancePtr->PrivateData, 1, USB_EP_DIR_OUT);
		ret = EpEnable(InstancePtr->PrivateData, 1, USB_EP_DIR_OUT,	MaxPktSize, USB_EP_TYPE_BULK);
//		LOG_DEBUG("enable ep1 out:%d\r\n", ret);

		USB_dev_set_recv_dma_buffer(InstancePtr->PrivateData, 1);
		#endif

		#if (CONFIG_USB_DEC_CDC_NUM >= 2)
		EpClearStall(InstancePtr->PrivateData, 4, USB_EP_DIR_IN);
		ret = EpEnable(InstancePtr->PrivateData, 4, USB_EP_DIR_IN, 1024, USB_EP_TYPE_INTERRUPT);
		xil_printf("enable ep4:%d\r\n", ret);

		/* Endpoint enables - not needed for Control EP */
		EpDisable(InstancePtr->PrivateData, 3, USB_EP_DIR_IN);
		EpClearStall(InstancePtr->PrivateData, 3, USB_EP_DIR_IN);
		ret = EpEnable(InstancePtr->PrivateData, 3, USB_EP_DIR_IN, MaxPktSize, USB_EP_TYPE_BULK);
		xil_printf("enable ep3 in:%d\r\n", ret);

		EpDisable(InstancePtr->PrivateData, 3, USB_EP_DIR_OUT);
		EpClearStall(InstancePtr->PrivateData, 3, USB_EP_DIR_OUT);
		ret = EpEnable(InstancePtr->PrivateData, 3, USB_EP_DIR_OUT, MaxPktSize, USB_EP_TYPE_BULK);
		xil_printf("enable ep3 out:%d\r\n", ret);

		USB_dev_set_recv_dma_buffer(InstancePtr->PrivateData, 3);
		#endif

	} else {
		#if (CONFIG_USB_DEC_CDC_NUM >= 1)
		EpDisable(InstancePtr->PrivateData, 2, USB_EP_DIR_IN);
		EpDisable(InstancePtr->PrivateData, 1, USB_EP_DIR_IN);
		EpDisable(InstancePtr->PrivateData, 1, USB_EP_DIR_OUT);
		#endif

		#if (CONFIG_USB_DEC_CDC_NUM >= 2)
		EpDisable(InstancePtr->PrivateData, 4, USB_EP_DIR_IN);
		EpDisable(InstancePtr->PrivateData, 3, USB_EP_DIR_IN);
		EpDisable(InstancePtr->PrivateData, 3, USB_EP_DIR_OUT);
		#endif
	}

	return XST_SUCCESS;
}
