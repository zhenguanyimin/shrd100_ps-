/*
 * battery.c
 *
 *  Created on: 2023年4月11日
 *      Author: A22745
 */


#include "battery.h"
#include "common_i2c.h"


unsigned char  BatteryCrc8(unsigned char* data,unsigned char len)
{
     unsigned char  crc=0;
	 unsigned char  i=0;

	  while(len--)
		{
		    crc^=*data++;
			  for(i=0;i<8;i++)
			  {
				    if(crc&0x80)
						{
						   crc=(crc<<1)^0x07;
						}
						else
						{
						   crc<<=1;
						}
				}
		}

		return crc;
}

ret_code_t GetBatteryInfo(batteryRegisterAddr_e reg, uint16_t *data)
{
	ret_code_t ret = RET_OK;
	uint8_t data_s[3] = {0};
	uint8_t crc_get = 0;
	uint8_t crc_cal = 0;
	uint8_t crc_data[5] = {0};
	uint16_t u16_data = 0;

	crc_data[0] = 0xaa;
	crc_data[1] = reg;
	crc_data[2] = 0xab;

	ret = I2c_Reg_Read(BATTERY_BOARD_6S2P, reg, &data_s[0], 3);
	crc_get = data_s[2];
	u16_data = (data_s[1] << 8) | data_s[0];

	if (ret == RET_OK)
	{
		crc_data[3] = data_s[0];
		crc_data[4] = data_s[1];

		crc_cal = BatteryCrc8(crc_data,5);
		if (crc_cal == crc_get)
		{
			if (data != NULL)
			{
				*data = u16_data;
			}
		}
		else
		{
			ret = RET_GENERAL_ERR;
		}
	}

	return ret;
}


