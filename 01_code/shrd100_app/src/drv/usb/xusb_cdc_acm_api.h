/******************************************************************************
* Copyright (C) 2004 - 2023 AUTEL, Inc.  All rights reserved.
* 
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file xusb_cdc_acm_api.h
 *
 * This file contains the usb device cdc acm APIs.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -------------------------------------------------------
 *
 * </pre>
 *
 *****************************************************************************/

#ifndef XUSB_CDC_ACM_API_H
#define XUSB_CDC_ACM_API_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xusb_cdc_config.h"

/************************** Constant Definitions *****************************/

/************************** TypeDef Definitions *****************************/
/*
 * USB device handle
 */
typedef void * USB_dev_handle_t ;

/*
 * USB device call back from ISR
 * with a parameter
 */
typedef void (*USB_dev_sent_cb_t)(void *param, size_t sent_size);
typedef void (*USB_dev_recv_cb_t)(void *param, size_t recv_size);


/************************** Constant Definitions *****************************/
/**
 * @name usb device channel
 * @{
 */
enum 
{
	USB_DEVICE_CHANNEL_0 = 0,
		
	#if (CONFIG_USB_DEC_CDC_NUM >= 2)
	USB_DEVICE_CHANNEL_1 = 1,
	#endif
	
	USB_DEVICE_CHANNEL_LIMIT,
};
/* @} */


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
/** 
 * initial the usb cdc acm devices
 */
int USB_dev_init(void);

/**
 * open a usb channel
 * return: -1: error, channel error
 * return: NULL: open error
 * others: OK
 */
USB_dev_handle_t USB_dev_open(int channel);

void USB_dev_close(USB_dev_handle_t handle);

/**
 * no block, thread safe, DONOT use in ISR!
 * return: >=0: the real size that sent
 */
int USB_dev_send(USB_dev_handle_t handle, const uint8_t *data, size_t size);

/**
 * no block, thread safe, DONOT use in ISR!
 * return: >=0:  the real size that receive
 */
int USB_dev_recv(USB_dev_handle_t handle, uint8_t *data, size_t want_size);

/**
 * the isr_cb run in ISR
 * SO, the call back function DO NOT sleep and run as fast as you can
 */
int USB_dev_set_sent_cb(USB_dev_handle_t handle, USB_dev_sent_cb_t isr_cb, void *cb_param);
int USB_dev_set_recv_cb(USB_dev_handle_t handle, USB_dev_recv_cb_t isr_cb, void *cb_param);
int InitUsbProcess_Task(void);
int Usb_Send_data_func(uint8_t *data, uint32_t len);
int usb_recv_proc(USB_dev_handle_t handle, uint8_t *buffdata, uint32_t EventData);
void SetUsbDataPrintf(uint8_t value);
uint8_t GetUsbDataPrintf(void);
int UsbSendDataFunc(uint8_t *data, uint32_t len);
USB_dev_handle_t *get_dev_handele_p(void);
void SetUsbCdcConnected(uint8_t flag);
#ifdef __cplusplus
}
#endif

#endif /* XUSB_CDC_ACM_API_H */

