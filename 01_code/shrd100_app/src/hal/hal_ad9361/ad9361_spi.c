#include "ad9361_spi.h"

#include <string.h>

#include "../../srv/log/log.h"
u32 cmdnum =0;
u32 invalidcmdnum =0;

/*
*********************************************************************************************************
*	函 数 名: delayMs
*	功能说明: 毫秒级延时函数
*	形    参：dlyms,需要延时的毫秒数
*	返 回 值: void
*	备注	:
*********************************************************************************************************
*/
void delayMs(u8 dlyms)
{
	usleep(dlyms*1000);
}

/*
*********************************************************************************************************
*	函 数 名: selectSpi
*	功能说明: 对连接ad9361的SPI进行初始化操作
*	形    参：spinum,spinum
*	返 回 值: void
*	备注	:
*********************************************************************************************************
*/
void selectSpi(u8 spinum)
{
	//spi3_msb8_init();

}

/*
*********************************************************************************************************
*	函 数 名: ad9361_spi_write_byte
*	功能说明: 通过spi设置ad9361寄存器的值
*	形    参：addr,寄存器地址;data,需要设置的值
*	返 回 值: void
*	备注	:
*********************************************************************************************************
*/
void ad9361_spi_write_byte(u16 addr,u8 data)
{
	u16 cmddat=0x0;
	u16 addr1=0;
	addr1 = addr&0x3ff;
	//cmddat |=(1<<15);
	//cmddat |= addr1;
	write_ad936x(addr1,data);

}


/*
*********************************************************************************************************
*	函 数 名: ad9361_spi_read_byte
*	功能说明: 通过spi读取ad9361寄存器的值
*	形    参：addr,寄存器地址
*	返 回 值: 读取到的寄存器值
*	备注	:
*********************************************************************************************************
*/
u8 ad9361_spi_read_byte(u16 addr)
{
	u16 cmddat=0x0;
	u16 addr1=0;
	u8 revdat;
	addr1 = addr&0x3ff;
	//cmddat |= addr1;
	
	read_ad9363(addr1,&cmddat);
	revdat = (u8)(cmddat&0xff);

	return revdat;

}

/*
*********************************************************************************************************
*	函 数 名: SetAd
*	功能说明: 根据配置参数配置ad9361寄存器
*	形    参：psetdat,一组配置参数的地址
*	返 回 值: void
*	备注	:
*********************************************************************************************************
*/
void SetAd(u8 *psetdat)
{
	u16 addr=0;
	u8 dat=0;
	u8 util,bit;
	u8 val1=0,val2=0;

	//addr
	addr |= ((*psetdat)&0x0f);
	addr <<=8;
	addr |= (*(psetdat+1));

	//data
	dat = *(psetdat+2);
	cmdnum+=1;
	if(((*psetdat)&0xf0)==0x80)//write
	{
		ad9361_spi_write_byte(addr,dat);
//	    u8 readdat = 0;
//        readdat = ad9361_spi_read_byte(addr);
//        if(readdat != dat)
//        {
//            LOG_DEBUG("error:addr:%X  data:%X  read data:%X \r\n",addr, dat, readdat);
//        }
	}
	else if(((*psetdat)&0xf0)==0x0)//read
	{
		if(dat == 0xff)
		{
			//printf("SetAd:read,just read.cmd=0x%x,addr=0x%x.\r\n",(*psetdat),addr);
			dat = ad9361_spi_read_byte(addr);
		}
		else
		{
			
			bit = dat>>4;
			util = dat&0x0f;
			// printf("SetAd:read,wait unitl read.cmd=0x%x,addr=0x%x.util=0x%x,bit=0x%x.\r\n",(*psetdat),addr,util,bit);
			do{
				dat=ad9361_spi_read_byte(addr);
				usleep(5);
				val1 = (0x01<<bit);
				val2 = (util<<bit); 
				// printf("uitl:dat=0x%x.v1=0x%x,v2=0x%x.\r\n",dat,val1,val2);
			}while((dat& (0x01<<bit))!=(util<<bit));
			
		}
	}
	else if(((*psetdat)&0xf0)==0xf0)//delay
	{
		delayMs(dat);
		//printf("SetAd:delay,cmd =0x%x,t=0x%x.\r\n",(*psetdat),dat);
	}
	else
	{
		invalidcmdnum++;
	//	printf("SetAd:Invalied Cmd.cmd=0x%x,dat1=0x%x,dat2=0x%x.\r\n",(*psetdat),*(psetdat+1),dat);
	}
}




