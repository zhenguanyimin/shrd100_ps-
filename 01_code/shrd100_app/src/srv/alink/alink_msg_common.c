
#include "alink_msg_common.h"
#include "../log/log.h"
#include "checksum.h"

bool AlinkMsg_IsCrcValid(uint8_t *msg_buf, uint16_t msg_len, uint16_t crc_extend)
{
	bool ret = true;
	uint16_t crc_calc = 0;
	uint16_t crc_actual = 0;
	uint8_t crc_lo = 0;
	uint8_t crc_hi = 0;

	LOG_VERBOSE("[%s:%d] msg_buf=0x%x, msg_len=%u, crc_extend=0x%x \r\n", __FUNCTION__, __LINE__, msg_buf, msg_len, crc_extend);
	if (msg_len > 0)
	{
		crc_calc = crc_calculate((const uint8_t *)msg_buf + 1, msg_len - 1 - ALINK_NUM_CHECKSUM_BYTES);
#if ALINK_CRC_EXTRA
		crc_accumulate(crc_extend, &crc_calc);
#endif

		crc_lo = *(msg_buf + msg_len - 2);
		crc_hi = *(msg_buf + msg_len - 1);
		crc_actual = (crc_hi << 8) + crc_lo;
		if (crc_calc != crc_actual)
		{
			ret = false;
		}
		LOG_VERBOSE("[%s:%d] crc_calc=0x%x, crc_actual=0x%x, ret=%d \r\n", __FUNCTION__, __LINE__, crc_calc, crc_actual, ret);
	}
	return ret;
}
