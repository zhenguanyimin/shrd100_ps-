/**
 * mail box for SMP communication
 * base on GIC V2 SGI
 */

/**
 * core mailbox has tow role, one call writer and one call reader.
 * the data transfer  writer to reader only. A core has a writer and B core has a reader.
 * every core can initial many writer and reader.
 *
 * usage:
 * A core: core_mailbox_init(mailbox_instance, B_core_ID, A_ID, B_ID, writer ) -> core_mailbox_send(instance, data, size, timeout) ->core_mailbox_deinit
 * B core: core_mailbox_init(mailbox_instance, A_core_ID, B_ID, A_ID, reader ) -> core_mailbox_set_recv_cb(instance, cb), and wait recv data ->core_mailbox_deinit
 */

#ifndef __CORE_MAILBOX__H__
#define __CORE_MAILBOX__H__

#include "FreeRTOS.h"
#include "semphr.h"

#define CPU_CORE_MAX_NUMBER (4)
#define CORE_INT_MAX_NUMBER (16)

#define MAILBOX_RSP_OK (0xFF0055AA)
#define MAILBOX_RSP_ERR (0x00FFAA55)

#define MAILBOX_MSG_TYPE_REQ 0x11223344
#define MAILBOX_MSG_TYPE_RSP 0xAABBCCDD



typedef struct
{
	uint32_t type; //must be first

	uint32_t req_id;
	void *data;
	uint32_t size;
}mailbox_req_msg_t;

typedef struct
{
	uint32_t type; //must be first

	uint32_t rsp_id;
	uint32_t rsp_state; //MAILBOX_RSP_OK or MAILBOX_RSP_ERR
}mailbox_rsp_msg_t;

typedef struct
{
	union
	{
		mailbox_req_msg_t req_msg;
		mailbox_rsp_msg_t rsp_msg;
	};
}mailbox_msg_t;


//============API============================================

//error code
#define CORE_MAILBOX_ERROR (-1) //parameter error or other errors
#define CORE_MAILBOX_SEND_TIMEOUT (-2)
#define CORE_MAILBOX_RSP_ERROR (-3) //remote give a NACK respond
#define CORE_MAILBOX_RW_ERROR (-4)
#define CORE_MAILBOX_DEST_CORE_NO_READY (-5)

typedef enum
{
	CORE_MAILBOX_TYPE_WRITE = 0,
	CORE_MAILBOX_TYPE_READ,
}CORE_MAILBOX_TYPE_t;

//return <0 mean receive error, the sender will receive the error message.
typedef int(*core_mailbox_cb_t)(void *param, void *data, uint32_t size);

typedef struct
{
	CORE_MAILBOX_TYPE_t rw_type;

	uint32_t dest_core_id;
	uint32_t owner_core_id;

	uint32_t dest_int_id;
	uint32_t source_int_id;

	uint32_t send_msg_id; //when recv ack, the msg_id MUST be same
	mailbox_req_msg_t *req_msg;
	mailbox_rsp_msg_t *rsp_msg;

	core_mailbox_cb_t recv_cb; //if rw_type == CORE_MAILBOX_TYPE_READ, it set to NULL
	void *recv_cb_param;

	SemaphoreHandle_t lock;
	SemaphoreHandle_t rsp_sem;
}mailbox_t;


typedef mailbox_t core_mailbox_t;

int core_mailbox_core_is_ready(uint32_t core_id, uint32_t int_id);

//init a mailbox writer or mailbox reader.
int core_mailbox_init(core_mailbox_t *core_mailbox, uint32_t dest_core_id, uint32_t source_int_id, uint32_t dest_int_id, CORE_MAILBOX_TYPE_t RW);

int core_mailbox_send(core_mailbox_t *core_mailbox, const void *data, uint32_t size, unsigned int time_out_ms);
int core_mailbox_set_recv_cb(core_mailbox_t *core_mailbox, core_mailbox_cb_t cb, void *param);

void core_mailbox_deinit(core_mailbox_t *core_mailbox);





#endif

