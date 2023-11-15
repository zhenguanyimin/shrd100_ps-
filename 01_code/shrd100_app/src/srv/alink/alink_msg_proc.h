#ifndef ALINK_MSG_PROC_H
#define ALINK_MSG_PROC_H
#include "FreeRTOS.h"
#include "alink_msg_common.h"
#include "semphr.h"
#define RECV_MAX_BUFF 1024*17
#define ANS_RECV_MAX_BUFF 1024
#define ALINK_MSG_SEAMPHORE_DELY	10000

enum INTERFACE
{
	UART,
	WIFI,
	TYPE_C,
};
#pragma pack(1)
typedef struct alink_msg
{
    alink_msg_head_t msg_head;     //  message header
    uint8_t buffer[RECV_MAX_BUFF]; //
    uint16_t checksum;             ///< Checksum
} alink_msg_t;
#pragma pack()

#pragma pack(1)
typedef struct alink_ans
{
    uint16_t msg_len;                  //
    uint8_t buffer[ANS_RECV_MAX_BUFF]; ///
} alink_ans_t;

#pragma pack()

#pragma pack(1)
typedef struct alink_msg_point
{
    uint16_t msg_len;                  //
    uint8_t buffer[ANS_RECV_MAX_BUFF]; ///
} alink_msg_point_t;

#pragma pack()
/**
 *  @b Description
 *  @n
 *      Init alink message process module.
 *
 *  @retval
 *      Success    - 0
 *      Fail       - others
 */
int32_t AlinkMsgProc_Init(void);

/*
 *  send-receive data
 * return
 */
int32_t AlinkMsgSendRecv(void *pUartpl, alink_msg_t *pMsg, alink_ans_t *pAns_msg, uint32_t timeout);
int32_t AlinkMsgBtSendRecv(void *pUartpl, alink_msg_t *pMsg, alink_ans_t *pAns_msg, uint32_t timeout);
void AlinkMsg_UartLiteRecvHandler(void *CallBackRef, uint32_t EventData);
bool AlinkMsgProc_IsHeaderValid(alink_msg_head_t *header);
uint8_t *Get_RevBuffPoint();
void test();
void SetAlinkInterface(uint8_t value);
uint8_t GetAlinkInterface(void);
int32_t AlinkMsgSend(void *pUartpl, alink_msg_t *pMsg);
void AlinkMsgProcTask_Post(uint32_t bufaddr);
int8_t AlinkMsgSeamphoreTake(SemaphoreHandle_t seamphore, TickType_t tick);
int8_t AlinkMsgSeamphoreGive(SemaphoreHandle_t seamphore);
#endif /* ALINK_MSG_PROC_H */
