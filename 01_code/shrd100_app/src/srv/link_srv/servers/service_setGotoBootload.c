/*
 * service_setGotoBootload.c
 *
 *  Created on: 2023年2月16日
 *      Author: A22745
 */
#include "service_setGotoBootload.h"
#include "../../../srv/flash_nv/flash_nv.h"
#include "../../log/log.h"

#define XDCFG_MULTIBOOT_ADDR_OFFSET_REG 	(0XFFCA0000U + 0X00000010U) //MULTIBOOT寄存器，绝对地址，

#define CRL_APB_RPLL_CTRL                   (0XFF5E0000U + 0X00000030U)
#define CRL_APB_RPLL_CTRL_BYPASS_MASK       0x00000008U
#define XFSBL_MASTER_ONLY_RESET			    2U

#define CRL_APB_RESET_CTRL    				(0XFF5E0000U+ 0X00000218U)
#define CRL_APB_RESET_CTRL_SOFT_RESET_MASK  0X00000010U
#define XPLAT_PS_VERSION_ADDR 				(0xFFCA0000U + 0x00000044U)
#define XPS_VERSION_INFO_MASK 				(0xF)
#define XPS_VERSION_INFO_SHIFT 				0x0U

#define MULTI_BOOT_SEARCH_STEP 		(0x8000)
#define BOOTLOAD_APP_LOCATION (FLASH_GOLDEN_IMAGE_BASE/ MULTI_BOOT_SEARCH_STEP) //golden_image位置 , 以32KB为单位

#define FIRMWARE_UPGRADE_FLAG		(0xAA55AA55)
#define FIRMWARE_NOT_UPGRADE_FLAG	(0xFFFFFFFF)

#define CONFIRM_CODE					(0X55AA)

#pragma pack(1)
typedef struct system_reset_cmd {
	uint16_t rsetCode;
	uint16_t cmdType;
}system_reset_cmd_t;
#pragma pack()

static uint32_t XGetPSVersion()
{
	uint32_t reg;
	reg = (Xil_In32(XPLAT_PS_VERSION_ADDR) & XPS_VERSION_INFO_MASK);
	return (reg >> XPS_VERSION_INFO_SHIFT);
}

static void PsSoftwareReset(void)
{
	uint32_t RegValue;

	if (XGetPSVersion() == (uint32_t)XPS_VERSION_1)
	{
		RegValue = Xil_In32(CRL_APB_RPLL_CTRL) |
				CRL_APB_RPLL_CTRL_BYPASS_MASK;
		Xil_Out32(CRL_APB_RPLL_CTRL, RegValue);
	}

	/* make sure every thing completes */
	dsb();
	isb();

	if (XFSBL_MASTER_ONLY_RESET != 0) {

		/* Soft reset the system */
		RegValue = Xil_In32(CRL_APB_RESET_CTRL);
		Xil_Out32(CRL_APB_RESET_CTRL,
		RegValue|CRL_APB_RESET_CTRL_SOFT_RESET_MASK);

		/* wait here until reset happens */
		while(1) {
			;
		}
	}
	else
	{
		for(;;){
			/*We should not be here*/
		}
	}

}

static void ModifyMultiBoot(uint32_t pos_idx)
{
	Xil_Out32(XDCFG_MULTIBOOT_ADDR_OFFSET_REG, pos_idx); // modify MULTIBOOT value
}

static void GoToBootload(void)
{
	ModifyMultiBoot(BOOTLOAD_APP_LOCATION);
	PsSoftwareReset();
}

uint16_t Service_SetGoToBootload_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t len = 0;
	uint8_t ret;
	uint8_t retVal;
	size_t retLent;
	uint32_t uUpgradeFlag = FIRMWARE_UPGRADE_FLAG;
	uint32_t uUpgradeFlagRead = 0;
	system_reset_cmd_t system_reset_cmd;

	strncpy((char*)&system_reset_cmd, (char*)&request_msg->buffer[0], sizeof(system_reset_cmd_t));

    if (system_reset_cmd.rsetCode == CONFIRM_CODE)
    {
    	switch (system_reset_cmd.cmdType)
    	{
			case 0:
			{
				retVal = 0;
				break;
			}
			case 1:
			{
				GoToBootload();
				retVal = 0;
				break;
			}
			case 2:
			case 4:
			{
	    		ret = FlashNV_Set(FLASH_NV_ID_UPGRADE_FLAG, &uUpgradeFlag, sizeof(uUpgradeFlag));
	    		if (ret != 0)
	    		{
	    			LOG_DEBUG("[%s:%d]ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	    		}
	    		vTaskDelay(30);
	    		ret = FlashNV_Get(FLASH_NV_ID_UPGRADE_FLAG, &uUpgradeFlagRead, sizeof(uUpgradeFlagRead), &retLent);

	    		if (ret != 0)
	    		{
	    			LOG_DEBUG("[%s:%d]ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	    		}
	    		if (uUpgradeFlagRead != uUpgradeFlag)
	    		{
	    			LOG_DEBUG("[%s:%d]ret=%d, set nv(%d) != get nev(%d)\r\n", __FUNCTION__, __LINE__, uUpgradeFlag, uUpgradeFlagRead);
	    		}
	    		GoToBootload();
				break;
			}
			case 3:
			{
				retVal = 0;
				break;
			}
			default:
			{
				retVal = 10;
				break;
			}
    	}

    }
    else
    {
    	retVal = 1;
    }

	response_msg->buffer[0] = retVal;
	len = 1;

	return len;
}


__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_SetGoToBootload =
	{
		.id = CMD_SET_GOTO_BOOTLOAD,
		.index = 0x5555AAAA,
		.entry = Service_SetGoToBootload_fun,
};
