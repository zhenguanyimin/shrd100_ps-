#ifndef _AD9361_H_
#define _AD9361_H_
#include <xil_types.h>

void SetAd(u8 *psetdat);
void delayMs(u8 dlyms);
void selectSpi(u8 spinum);
u8 ad9361_spi_read_byte(u16 addr);
void ad9361_spi_write_byte(u16 addr,u8 data);
void ad9361_spi_write_byte(u16 addr,u8 data);
void ad9361_spi_test(void);

#endif

