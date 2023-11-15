#include "hal.h"
#include <xparameters.h>

void axi_write_data(unsigned int addr, int dat)
{
	unsigned int *LocalAddr = (unsigned int *)(XPAR_BRAM_0_BASEADDR + (addr << 2));
	*LocalAddr = dat;
}
int axi_read_data(unsigned int addr)
{
	return *(volatile unsigned int *)(XPAR_BRAM_0_BASEADDR + (addr << 2));
}

void axi_write(unsigned int addr, int dat)
{
	unsigned int *LocalAddr = (unsigned int *)(addr);
	*LocalAddr = dat;
}
unsigned int axi_read(unsigned int addr)
{
	return *(volatile unsigned int *)(addr);
}
