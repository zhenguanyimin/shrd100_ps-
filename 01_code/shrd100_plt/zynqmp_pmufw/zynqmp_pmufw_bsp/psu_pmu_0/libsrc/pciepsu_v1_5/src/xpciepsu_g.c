
/*******************************************************************
*
* CAUTION: This file is automatically generated by HSI.
* Version: 2022.2
* DO NOT EDIT.
*
* Copyright (C) 2010-2023 Xilinx, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT 

* 
* Description: Driver configuration
*
*******************************************************************/

#include "xparameters.h"
#include "xpciepsu.h"

/*
* The configuration table for devices
*/

XPciePsu_Config XPciePsu_ConfigTable[] =
{
	{
		XPAR_PSU_PCIE_DEVICE_ID,
		XPAR_PSU_PCIE_BASEADDR,
		XPAR_PSU_PCIE_ATTRIB_0_BASEADDR,
		XPAR_PSU_PCIE_LOW_BASEADDR,
		XPAR_PSU_PCIE_LOW_HIGHADDR,
		XPAR_PSU_PCIE_DMA_BASEADDR,
		XPAR_PSU_PCIE_PCIE_MODE
	}
};


size_t XPciePsu_ConfigTableSize = ARRAY_SIZE(XPciePsu_ConfigTable);


