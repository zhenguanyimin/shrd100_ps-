/******************************************************************************
* Copyright (c) 2017 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#include "xil_types.h"
#include "pm_defs.h"

#define PM_CONFIG_MASTER_SECTION_ID	0x101U
#define PM_CONFIG_SLAVE_SECTION_ID	0x102U
#define PM_CONFIG_PREALLOC_SECTION_ID	0x103U
#define PM_CONFIG_POWER_SECTION_ID	0x104U
#define PM_CONFIG_RESET_SECTION_ID	0x105U
#define PM_CONFIG_SHUTDOWN_SECTION_ID	0x106U
#define PM_CONFIG_SET_CONFIG_SECTION_ID	0x107U
#define PM_CONFIG_GPO_SECTION_ID	0x108U

#define PM_SLAVE_FLAG_IS_SHAREABLE	0x1U
#define PM_MASTER_USING_SLAVE_MASK	0x2U

#define PM_CONFIG_GPO1_MIO_PIN_34_MAP	(1U << 10U)
#define PM_CONFIG_GPO1_MIO_PIN_35_MAP	(1U << 11U)
#define PM_CONFIG_GPO1_MIO_PIN_36_MAP	(1U << 12U)
#define PM_CONFIG_GPO1_MIO_PIN_37_MAP	(1U << 13U)

#define PM_CONFIG_GPO1_BIT_2_MASK	(1U << 2U)
#define PM_CONFIG_GPO1_BIT_3_MASK	(1U << 3U)
#define PM_CONFIG_GPO1_BIT_4_MASK	(1U << 4U)
#define PM_CONFIG_GPO1_BIT_5_MASK	(1U << 5U)

#define SUSPEND_TIMEOUT	0xFFFFFFFFU

#define PM_CONFIG_OBJECT_TYPE_BASE	0x1U


#define PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK    0x00000001
#define PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK    0x00000100
#define PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK    0x00000200



#if defined (__ICCARM__)
#pragma language=save
#pragma language=extended
#endif
#if defined (__GNUC__)
    const u32 XPm_ConfigObject[] __attribute__((used, section(".sys_cfg_data"))) =
#elif defined (__ICCARM__)
#pragma location = ".sys_cfg_data"
__root const u32 XPm_ConfigObject[] =
#endif
{
	/**********************************************************************/
	/* HEADER */
	2,	/* Number of remaining words in the header */
	8,	/* Number of sections included in config object */
	PM_CONFIG_OBJECT_TYPE_BASE,	/* Type of config object as base */
	/**********************************************************************/
	/* MASTER SECTION */
	PM_CONFIG_MASTER_SECTION_ID, /* Master SectionID */
	3U, /* No. of Masters*/

	NODE_APU, /* Master Node ID */
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK, /* IPI Mask of this master */
	SUSPEND_TIMEOUT, /* Suspend timeout */
	PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* Suspend permissions */
	PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* Wake permissions */

	NODE_RPU_0, /* Master Node ID */
	PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK, /* IPI Mask of this master */
	SUSPEND_TIMEOUT, /* Suspend timeout */
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* Suspend permissions */
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* Wake permissions */

	NODE_RPU_1, /* Master Node ID */
	PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask of this master */
	SUSPEND_TIMEOUT, /* Suspend timeout */
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK, /* Suspend permissions */
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK, /* Wake permissions */


	/**********************************************************************/
	/* SLAVE SECTION */


	PM_CONFIG_SLAVE_SECTION_ID,	/* Section ID */
	38,				/* Number of slaves */

	NODE_OCM_BANK_0,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_OCM_BANK_1,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_OCM_BANK_2,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_OCM_BANK_3,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_TCM_0_A,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK, /* IPI Mask */

	NODE_TCM_0_B,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK, /* IPI Mask */

	NODE_TCM_1_A,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_TCM_1_B,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_L2,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_GPU_PP_0,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_GPU_PP_1,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_USB_0,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_USB_1,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_TTC_0,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_TTC_1,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_TTC_2,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_TTC_3,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_ETH_1,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_UART_0,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_UART_1,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_SPI_0,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_I2C_0,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_I2C_1,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_SD_0,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_SD_1,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_GDMA,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_ADMA,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_QSPI,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_GPIO,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_EXTERN,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_DDR,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK| PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_IPI_APU,
	0U,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK, /* IPI Mask */

	NODE_IPI_RPU_0,
	0U,
	PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK, /* IPI Mask */

	NODE_IPI_RPU_1,
	0U,
	PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_GPU,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_PCIE,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_RTC,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */

	NODE_PL,
	PM_SLAVE_FLAG_IS_SHAREABLE,
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* IPI Mask */


	/**********************************************************************/
	/* PREALLOC SECTION */

	PM_CONFIG_PREALLOC_SECTION_ID, /* Preallaoc SectionID */
	3U, /* No. of Masters*/

/* Prealloc for psu_cortexa53_0 */
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK,
	12,
	NODE_DDR,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_L2,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_OCM_BANK_0,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_OCM_BANK_1,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_OCM_BANK_2,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_OCM_BANK_3,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_I2C_0,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_I2C_1,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_SD_1,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_QSPI,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_PL,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_IPI_APU,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */


	/* Prealloc for psu_cortexr5_0 */
	PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK,
	3,
	NODE_TCM_0_A,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_TCM_0_B,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_IPI_RPU_0,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */


	/* Prealloc for psu_cortexr5_1 */
	PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	3,
	NODE_TCM_1_A,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_TCM_1_B,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */

	NODE_IPI_RPU_1,
	PM_MASTER_USING_SLAVE_MASK, /* Master is using Slave */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Current Requirements */
	PM_CAP_ACCESS | PM_CAP_CONTEXT, /* Default Requirements */


	
	/**********************************************************************/
	/* POWER SECTION */

	PM_CONFIG_POWER_SECTION_ID, /* Power Section ID */
	4U, /* Number of power nodes */

	NODE_APU, /* Power node ID */
	PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* Force power down permissions */

	NODE_RPU, /* Power node ID */
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* Force power down permissions */

	NODE_FPD, /* Power node ID */
	PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* Force power down permissions */

	NODE_PLD, /* Power node ID */
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* Force power down permissions */


	/**********************************************************************/
	/* RESET SECTION */

	PM_CONFIG_RESET_SECTION_ID, /* Reset Section ID */
	120U, /* Number of resets */

	XILPM_RESET_PCIE_CFG, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_PCIE_BRIDGE, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_PCIE_CTRL, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_DP, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_SWDT_CRF, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_AFI_FM5, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_AFI_FM4, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_AFI_FM3, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_AFI_FM2, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_AFI_FM1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_AFI_FM0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GDMA, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPU_PP1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPU_PP0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPU, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GT, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_SATA, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_ACPU3_PWRON, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_ACPU2_PWRON, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_ACPU1_PWRON, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_ACPU0_PWRON, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_APU_L2, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_ACPU3, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_ACPU2, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_ACPU1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_ACPU0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_DDR, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_APM_FPD, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_SOFT, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GEM0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GEM1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GEM2, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GEM3, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_QSPI, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_UART0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_UART1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_SPI0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_SPI1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_SDIO0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_SDIO1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_CAN0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_CAN1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_I2C0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_I2C1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_TTC0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_TTC1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_TTC2, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_TTC3, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_SWDT_CRL, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_NAND, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_ADMA, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPIO, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_IOU_CC, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_TIMESTAMP, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_RPU_R50, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_RPU_R51, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_RPU_AMBA, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_OCM, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_RPU_PGE, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_USB0_CORERESET, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_USB1_CORERESET, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_USB0_HIBERRESET, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_USB1_HIBERRESET, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_USB0_APB, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_USB1_APB, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_IPI, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_APM_LPD, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_RTC, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_SYSMON, 0,
	XILPM_RESET_AFI_FM6, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_LPD_SWDT, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_FPD, PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK,
	XILPM_RESET_RPU_DBG1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_RPU_DBG0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_DBG_LPD, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_DBG_FPD, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_APLL, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_DPLL, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_VPLL, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_IOPLL, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_RPLL, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_0, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_1, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_2, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_3, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_4, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_5, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_6, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_7, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_8, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_9, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_10, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_11, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_12, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_13, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_14, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_15, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_16, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_17, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_18, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_19, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_20, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_21, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_22, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_23, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_24, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_25, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_26, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_27, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_28, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_29, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_30, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPO3_PL_31, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_RPU_LS, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_PS_ONLY, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_PL, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPIO5_EMIO_92, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPIO5_EMIO_93, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPIO5_EMIO_94, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,
	XILPM_RESET_GPIO5_EMIO_95, PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK,

	/**********************************************************************/
	/* SET CONFIG SECTION */
	PM_CONFIG_SET_CONFIG_SECTION_ID,		/* Set Config Section ID */
	0U, /* Permissions to load base config object */
	0U, /* Permissions to load overlay config object */

	/**********************************************************************/
	/* SHUTDOWN SECTION */

	PM_CONFIG_SHUTDOWN_SECTION_ID, /* Shutdown Section ID */
	PM_CONFIG_IPI_PSU_CORTEXA53_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_0_MASK | PM_CONFIG_IPI_PSU_CORTEXR5_1_MASK, /* System Shutdown/Restart Permission */

	/**********************************************************************/
	/* GPO SECTION */
	PM_CONFIG_GPO_SECTION_ID,		/* GPO Section ID */
	0,					/* State of GPO pins */
};
#if defined (__ICCARM__)
#pragma language=restore
#endif

