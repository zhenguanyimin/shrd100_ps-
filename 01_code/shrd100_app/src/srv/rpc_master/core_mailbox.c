
#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xil_io.h"
#include "xil_printf.h"
#include "xil_types.h"

#include "core_mailbox.h"
#include "xscugic.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "../../srv/log/log.h"


#define GICC_AIAR_OFFSET_ADDR 	0x20
#define GICD_SGIR_OFFSET_ADDR  0x0000000F00
#define GICD_SPENDSGIRn_OFFSET_ADDR 0xF20
#define GICD_CTLR_OFFSET  0x0

#define GICC_IAR_OFFSET_ADDR 0x0c
#define GICC_CTLR_OFFSET  0x0



#define CORE_0_READY_FLAG (0x1111111111111110)
#define CORE_1_READY_FLAG (0x3333333333333330)
#define CORE_2_READY_FLAG (0x7777777777777770)
#define CORE_3_READY_FLAG (0xFFFFFFFFFFFFFFF0)

static const uint64_t CORES_READY_FLAG[CPU_CORE_MAX_NUMBER][CORE_INT_MAX_NUMBER] = {{CORE_0_READY_FLAG}, {CORE_1_READY_FLAG}, {CORE_2_READY_FLAG}, {CORE_3_READY_FLAG}};
static uint64_t core_ready_state[CPU_CORE_MAX_NUMBER][CORE_INT_MAX_NUMBER] __attribute__((section(".mailbox_share_memory")));;
static mailbox_msg_t msg[CPU_CORE_MAX_NUMBER][CORE_INT_MAX_NUMBER] __attribute__((section(".mailbox_share_memory.msg")));

#define CORE_MAILBOX_READY_START_INIT 0x55
static uint32_t ready_state_init_flag = CORE_MAILBOX_READY_START_INIT;


#define CURRENT_CORE_ID XScuGic_GetCpuID()

static void ready_state_init(void)
{
	if (CORE_MAILBOX_READY_START_INIT == ready_state_init_flag)
	{
		ready_state_init_flag = 0;
		memset(core_ready_state[CURRENT_CORE_ID], 0, sizeof(core_ready_state[0]));
	}
}


static void set_core_ready_state(uint32_t int_id, int is_ready)
{
	if (is_ready)
	{
		core_ready_state[CURRENT_CORE_ID % CPU_CORE_MAX_NUMBER][int_id % CORE_INT_MAX_NUMBER] =
			CORES_READY_FLAG[CURRENT_CORE_ID % CPU_CORE_MAX_NUMBER][int_id % CORE_INT_MAX_NUMBER] + int_id;
	}
	else
	{
		core_ready_state[CURRENT_CORE_ID % CPU_CORE_MAX_NUMBER][int_id % CORE_INT_MAX_NUMBER] = 0;
	}
}

int core_mailbox_core_is_ready(uint32_t core_id, uint32_t int_id)
{
	uint64_t flag;
	uint64_t value;

	flag = CORES_READY_FLAG[core_id%CPU_CORE_MAX_NUMBER][int_id % CORE_INT_MAX_NUMBER] + int_id;
	value = core_ready_state[core_id%CPU_CORE_MAX_NUMBER][int_id % CORE_INT_MAX_NUMBER];

	return (flag == value)? 1 : 0;
}

static void set_req_msg_info(mailbox_req_msg_t *msg, uint32_t msg_id, void *data, uint32_t size)
{
	msg->type = MAILBOX_MSG_TYPE_REQ;
	msg->req_id = msg_id;
	msg->data = data;
	msg->size = size;
}

static void set_rsp_msg_info(mailbox_rsp_msg_t *msg, uint32_t msg_id, uint32_t rsp_state)
{
	msg->type = MAILBOX_MSG_TYPE_RSP;
	msg->rsp_id = msg_id;
	msg->rsp_state = rsp_state;
}

static void core_software_int_trigger(uint32_t dest_core_id, uint32_t int_id)
{
	XScuGic_Config *IntcConfig;

	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	XScuGic_WriteReg(IntcConfig->DistBaseAddress, GICD_SGIR_OFFSET_ADDR, (0x00<<24) | ((1<<dest_core_id)<<16) | int_id);
}

static void req_handler(void *core_mailbox)
{
	int ret = -1;
	mailbox_t *mailbox = (mailbox_t *)core_mailbox;
	mailbox_req_msg_t *req_msg = mailbox->req_msg;

	if (MAILBOX_MSG_TYPE_REQ == req_msg->type)
	{
		if (NULL == mailbox->recv_cb)
		{
//			error, need a recive call back function
//			LOG_ERROR("%s[%d]no call back function, send NACK\r\n", __func__, __LINE__);
			ret = -1;
		}
		else
		{
			ret = mailbox->recv_cb(mailbox->recv_cb_param, req_msg->data, req_msg->size);
		}
	}

	//send ACK or NACK back
	if (0 == ret)
	{
		//ACK
		set_rsp_msg_info(mailbox->rsp_msg, req_msg->req_id, MAILBOX_RSP_OK);
	}
	else
	{
		//error, send a NACK
		set_rsp_msg_info(mailbox->rsp_msg, req_msg->req_id, MAILBOX_RSP_ERR);
	}

	core_software_int_trigger(mailbox->dest_core_id, mailbox->dest_int_id);
	//xil_printf("%s[%d]\r\n", __func__, __LINE__);
}

static void rsp_handler(void *core_mailbox)
{
	mailbox_t *mailbox = (mailbox_t *)core_mailbox;
	BaseType_t xHigherPriorityTaskWoken;

	//xil_printf("%s[%d]\r\n", __func__, __LINE__);
	//xil_printf("%s[%d]mailbox addr:%08X\r\n", __func__, __LINE__, (int)core_mailbox);

	xSemaphoreGiveFromISR(mailbox->rsp_sem, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE)
	{
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	//xil_printf("%s[%d]\r\n", __func__, __LINE__);
}

static void core_software_int_init(mailbox_t *mailbox)
{
	XScuGic_Config *IntcConfig;
	/*d
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (NULL == IntcConfig) {
		return ;
	}

	XScuGic_SetPriTrigTypeByDistAddr(IntcConfig->DistBaseAddress, mailbox->source_int_id, /*248*/19<<3, 0x03);

	//if i'am a writer, i just need to wait a respond
	if (CORE_MAILBOX_TYPE_WRITE == mailbox->rw_type)
	{
		XScuGic_RegisterHandler(IntcConfig->CpuBaseAddress, mailbox->source_int_id, rsp_handler, mailbox);
		//xil_printf("%s[%d]source_int_id:%d, mailbox addr:%08X\r\n", __func__, __LINE__, mailbox->source_int_id, (int)mailbox);
	}
	else
	{
		XScuGic_RegisterHandler(IntcConfig->CpuBaseAddress,mailbox->source_int_id, req_handler, mailbox);
		//xil_printf("%s[%d]source_int_id:%d, mailbox addr:%08X\r\n", __func__, __LINE__, mailbox->source_int_id, (int)mailbox);
	}

	XScuGic_EnableIntr(IntcConfig->DistBaseAddress, mailbox->source_int_id);

}

static void core_software_int_deinit(mailbox_t *mailbox)
{
	XScuGic_Config *IntcConfig;

	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);

	XScuGic_DisableIntr(IntcConfig->DistBaseAddress, mailbox->source_int_id);
	///XScuGic_Disconnect(&IntcInstance, mailbox->source_int_id);
}




#define LOCK(mutex)   xSemaphoreTake(mutex, portMAX_DELAY)
#define UNLOCK(mutex) xSemaphoreGive(mutex)

int core_mailbox_init(core_mailbox_t *core_mailbox, uint32_t dest_core_id, uint32_t source_int_id, uint32_t dest_int_id, CORE_MAILBOX_TYPE_t RW)
{
	mailbox_t *mailbox = (mailbox_t *)core_mailbox;

	ready_state_init();

	if (dest_core_id >= CPU_CORE_MAX_NUMBER)
	{
		return CORE_MAILBOX_ERROR;
	}

	if (NULL == mailbox)
	{
		return CORE_MAILBOX_ERROR;
	}

	if ((source_int_id >= CORE_INT_MAX_NUMBER) || (dest_int_id >= CORE_INT_MAX_NUMBER))
	{
		return CORE_MAILBOX_ERROR;
	}

	if ((CORE_MAILBOX_TYPE_WRITE != RW) && (CORE_MAILBOX_TYPE_READ != RW))
	{
		return CORE_MAILBOX_ERROR;
	}

	if (core_mailbox_core_is_ready(CURRENT_CORE_ID, source_int_id))
	{
		return CORE_MAILBOX_ERROR;
	}

	mailbox->dest_core_id = dest_core_id;
	mailbox->owner_core_id = CURRENT_CORE_ID;

	mailbox->dest_int_id = dest_int_id;
	mailbox->source_int_id = source_int_id;

	mailbox->rw_type = RW;

	if (CORE_MAILBOX_TYPE_WRITE == RW)
	{
		mailbox->req_msg = &msg[dest_core_id][dest_int_id].req_msg;
		mailbox->rsp_msg = &msg[mailbox->owner_core_id][source_int_id].rsp_msg;
	}
	else
	{
		mailbox->req_msg = &msg[mailbox->owner_core_id][source_int_id].req_msg;
		mailbox->rsp_msg = &msg[dest_core_id][dest_int_id].rsp_msg;
	}

	mailbox->send_msg_id = (uint32_t)rand();

	mailbox->lock = xSemaphoreCreateMutex();
	if (NULL == mailbox->lock)
	{
		return CORE_MAILBOX_ERROR;
	}

	mailbox->rsp_sem = xSemaphoreCreateBinary();
	if (NULL == mailbox->rsp_sem)
	{
		vSemaphoreDelete(mailbox->lock);
		mailbox->lock = NULL;

		return CORE_MAILBOX_ERROR;
	}

	core_software_int_init(mailbox);
	set_core_ready_state(source_int_id, 1);

	return 0;
}



int core_mailbox_send(core_mailbox_t *core_mailbox, const void *data, uint32_t size, unsigned int time_out_ms)
{
	mailbox_t *mailbox = (mailbox_t *)core_mailbox;
	mailbox_rsp_msg_t *rsp_msg;
	TickType_t block_time;

	int ret = CORE_MAILBOX_ERROR;
	BaseType_t wait_ret;

	if (!core_mailbox_core_is_ready(mailbox->dest_core_id, mailbox->dest_int_id))
	{
		return CORE_MAILBOX_DEST_CORE_NO_READY;
	}

	LOCK(mailbox->lock);

	if (CORE_MAILBOX_TYPE_WRITE != mailbox->rw_type)
	{
		UNLOCK(mailbox->lock);
		return CORE_MAILBOX_RW_ERROR;
	}

	memset(mailbox->rsp_msg, 0, sizeof(mailbox_rsp_msg_t));
	set_req_msg_info(mailbox->req_msg, mailbox->send_msg_id, data, size);
	core_software_int_trigger(mailbox->dest_core_id, mailbox->dest_int_id);

	//wait for ack or nack, maybe timeout
	while(1)
	{
		//xil_printf("%s[%d]wait for function call signal。tick:%d\r\n", __func__, __LINE__, xTaskGetTickCount());
		if (time_out_ms == portMAX_DELAY)
		{
			block_time = portMAX_DELAY;
		}
		else
		{
			block_time = time_out_ms/portTICK_PERIOD_MS;
		}
		
		wait_ret = xSemaphoreTake(mailbox->rsp_sem, block_time);
		//xil_printf("%s[%d]got call signal, ret:%d, tick:%d, block:%d\r\n", __func__, __LINE__, wait_ret, xTaskGetTickCount(), block_time);
		if (pdTRUE == wait_ret)
		{
			rsp_msg = mailbox->rsp_msg;
			if ((MAILBOX_MSG_TYPE_RSP != rsp_msg->type) || (rsp_msg->rsp_id != mailbox->send_msg_id))
			{
				//the rsp is not for me
				continue;
			}

			if (MAILBOX_RSP_OK != rsp_msg->rsp_state)
			{
				ret = CORE_MAILBOX_RSP_ERROR;
			}
			else
			{
				ret = 0;
			}

			mailbox->send_msg_id++;
			set_rsp_msg_info(rsp_msg, 0, MAILBOX_RSP_ERR);
		}
		else //time out
		{
			ret = CORE_MAILBOX_SEND_TIMEOUT;
		}

		break;
	}

	UNLOCK(mailbox->lock);

	return ret;
}

int core_mailbox_set_recv_cb(core_mailbox_t *core_mailbox, core_mailbox_cb_t cb, void *param)
{
	mailbox_t *mailbox = (mailbox_t *)core_mailbox;

	LOCK(mailbox->lock);

	mailbox->recv_cb = cb;
	mailbox->recv_cb_param = param;

	UNLOCK(mailbox->lock);

	return 0;
}

void core_mailbox_deinit(core_mailbox_t *core_mailbox)
{
	mailbox_t *mailbox = (mailbox_t *)core_mailbox;

	if (NULL == mailbox)
	{
		return;
	}

	LOCK(mailbox->lock);

	core_software_int_deinit(mailbox);
	vSemaphoreDelete(mailbox->rsp_sem);
	vSemaphoreDelete(mailbox->lock);
	set_core_ready_state(mailbox->source_int_id, 0);

	memset(mailbox, 0, sizeof(*mailbox));
}

//============================================================================//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static uint8_t reader_buffer[64];
#define TARGET_CORE_ID (0)
static int run = 0;

static int reader_cb(void *param, void *data, uint32_t size)
{
	uint8_t *pbuffer = (uint8_t *)param;

	//memset(pbuffer, 0, 64);
	//memcpy(pbuffer, data, size);

	//xil_printf("recv[%d]%s\r\n", size, pbuffer);
	LOG_DEBUG("recv[%d]%s\r\n", size, data);
	run = 1;

	return 0;
}

void mailbox_demo(void)
{
	mailbox_t writer, reader;
	int ret;
	int cnt = 0;
	uint8_t buffer[64];

	ret = core_mailbox_init(&writer, TARGET_CORE_ID, 1, 1, CORE_MAILBOX_TYPE_WRITE);
	LOG_DEBUG("writer init:%d\r\n", ret);

	ret = core_mailbox_init(&reader, TARGET_CORE_ID, 0, 0, CORE_MAILBOX_TYPE_READ);
	LOG_DEBUG("reader init:%d\r\n", ret);

	core_mailbox_set_recv_cb(&reader, reader_cb, reader_buffer);

	while(1)
	{
		/*
		if (!run)
		{
			vTaskDelay(10);
			continue;
		}
		*/

		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "[%d]APP1 writer", cnt);

		ret = core_mailbox_send(&writer, buffer, strlen((const char *)buffer), 1000);
		LOG_DEBUG("[%d]1send:%d\r\n", cnt, ret);

		vTaskDelay(100);

		cnt++;
	}

	core_mailbox_deinit(&writer);
	core_mailbox_deinit(&reader);
	while(1)
	{

	}
}




