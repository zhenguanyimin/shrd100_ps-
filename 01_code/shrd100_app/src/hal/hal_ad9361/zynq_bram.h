#ifndef _ZYNQ_BRAM_H_
#define _ZYNQ_BRAM_H_

#define AXI_BASE_ADDR 0x43c00000

#define OK 0
#define ERROR -1

#define AD9363_SPI_RE 0x02 //bit0 @0x10,read enable*
#define AD9363_SPI_WE 0x02 //bit1 @0x10,write enable*
#define AD9363_SPI_RW_ADDR 0x03 //bit0-bit9,read and write  addr*
#define AD9363_SPI_WDATA 0x04 //bit0-bit7,write data regaddr*
#define AD9363_SPI_RDATA 0x11 //[s_ad_spi_result,s_ad_spi_rdata];s_ad_spi_result,8bits;s_ad_spi_rdata;8bits,s_ad_spi_rdata is value when s_ad_spi_result=1;*

#define AD9363_SPI_WRITE_ENABLE 0x02
#define AD9363_SPI_READ_ENABLE  0x01

#define AD9363_SPI_RW_DISABLE  0x00

#define AD936X_CTR_ADDR 	0x05	//txnrx -bit0;enable -bit1; enagc -bit2; reset -bit3; *
#define AD936X_TXNRX_CTR_BIT 0x0 
#define AD936X_ENABLE_CTR_BIT 0x01
#define AD936X_ENAGC_CTR_BIT 0x02 
#define AD936X_RST_CTR_BIT 0x03


int reset_ad936x(void);
int write_ad936x(int addr,int dat);
int read_ad9363(int addr ,int * rddat);

int enable_ad936x(void);
int write_fpga_regs(int addr,int dat);
int read_fpga_regs(int addr);



#endif

