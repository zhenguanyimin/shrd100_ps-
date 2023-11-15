//#include <stdint.h>
#include "cali_para_config.h"

#include "../../hal/flash_hal/flash_hal.h"
#include "../../hal/output/output.h"
#include "../../srv/easyflash/inc/easyflash.h"
#include "../../srv/flash_nv/flash_nv.h"

#define FREQUENCY_DATA_BASE_ADDR			0x3C00000
#define FREQUENCY_SINGLE_DATA_STEP			(4U)
#define FREQUNECY_DATA_BASE_REGISTER		(0X40)
#define FREQUNECY_DATA_REGISTER_STEP		(1U)

/**
 * ============================================ for test ====================================================
 */
uint32_t getSglFreRegData(strick_fre_reg_id_t reg_id)
{
	uint32_t Address;
	uint32_t res = 0;
	uint8_t data[4] = {0};

	if (reg_id < dataSelectAID || reg_id >= dataInvalidID)
	{
		return XST_FAILURE;
	}

	Address = FREQUENCY_DATA_BASE_ADDR + FREQUENCY_SINGLE_DATA_STEP * (reg_id - 1);

	FlashHal_ReadByBytes(Address, data, 4);

	memcpy(&res, data, 4);

	return res;
}


uint32_t setSinglFreRegData(strick_fre_reg_id_t reg_id, uint32_t value)
{
	uint32_t Address;
//	uint8_t data[4] = {0};

	if (reg_id < dataSelectAID || reg_id >= dataInvalidID)
	{
		return XST_FAILURE;
	}

//	eraseQspiFlash4KSector(FREQUENCY_DATA_BASE_ADDR);
//	data[0] = (value & 0XFF000000) >> 24;
//	data[1] = (value & 0XFF0000) >> 16;
//	data[2] = (value & 0XFF00) >> 8;
//	data[3] = value & 0XFF;
	Address = FREQUENCY_DATA_BASE_ADDR + FREQUENCY_SINGLE_DATA_STEP * (reg_id - 1);
	FlashHal_WriteByBytes(Address, (uint8_t *)&value, 4);

	return XST_SUCCESS;
}
/********************************** for test ************************************/


