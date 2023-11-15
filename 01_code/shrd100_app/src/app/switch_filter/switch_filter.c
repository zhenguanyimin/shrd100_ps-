#include "switch_filter.h"
#include "../../drv/axiuart/sys_intr.h"
#include "../../hal/output/output.h"
#include "../../srv/log/log.h"
#include "../../srv/link_srv/link_srv.h"

uint8_t CodeCalcValue(uint8_t index, uint16_t *value)
{
	uint16_t buff[7] = { 25, 50, 100, 200, 400, 800, 1600 };
	uint8_t result = 0;
	if (index < 7)
	{
		if (*value >= buff[index])
		{
			*value -= buff[index];
			result = 1;
		}
	}

	return result;
}

uint8_t SetSwitchFilterGain(uint16_t value)
{
	uint8_t data = 0;
	uint8_t bit = 0;
	uint8_t i = 7;
	while(i--)
	{
		bit = CodeCalcValue(i, &value);
		data += bit << i;
	}

	SwitchFilterGainIoCtrl(data);

	return data;
}

int32_t SwitchFilterGainIoCtrl(uint8_t value)
{
	int32_t ret = -1;

	uint8_t data1  = value & 0x01;
	uint8_t data2 = value & 0x02;
	uint8_t data3  = value & 0x04;
	uint8_t data4  = value & 0x08;
	uint8_t data5  = value & 0x10;
	uint8_t data6  = value & 0x20;
	uint8_t data7  = value & 0x40;

	GPIO_OutputCtrl(EMIO_L_CTRL_D0, ((value & 0x01) != 0));
	GPIO_OutputCtrl(EMIO_L_CTRL_D1, ((value & 0x02) != 0));
	GPIO_OutputCtrl(EMIO_L_CTRL_D2, ((value & 0x04) != 0));
	GPIO_OutputCtrl(EMIO_L_CTRL_D3, ((value & 0x08) != 0));
	GPIO_OutputCtrl(EMIO_L_CTRL_D4, ((value & 0x10) != 0));
	GPIO_OutputCtrl(EMIO_L_CTRL_D5, ((value & 0x20) != 0));
	GPIO_OutputCtrl(EMIO_L_CTRL_D6, ((value & 0x40) != 0));

	return ret;
}

void Adrf5250Ctrl(uint8_t channel)
{
	uint8_t channel2;
	uint8_t flag = 0;

	channel2 = 6 - channel ;
	flag = (channel  & 0x01);
	GPIO_OutputCtrl(MIO_V1_1, flag);

	flag = (channel  & 0x02)>>1;
	GPIO_OutputCtrl(MIO_V2_1, flag);

	flag = (channel  & 0x04)>>2;
	GPIO_OutputCtrl(MIO_V3_1, flag);

	flag = (channel2 & 0x01);
	GPIO_OutputCtrl(MIO_V1_2, flag);

	flag = (channel2 & 0x02)>>1;
	GPIO_OutputCtrl(MIO_V2_2, flag);

	flag = (channel2 & 0x04)>>2;
	GPIO_OutputCtrl(MIO_V3_2, flag);
}


void Adrf5250Disable()
{
	uint8_t data;
	uint8_t flag = 0;
	uint8_t value = 1;

	data = 7 - value;
	flag = (value & 0x01);
	GPIO_OutputCtrl(MIO_V1_1, flag);

	flag = (value & 0x02)>>1;
	GPIO_OutputCtrl(MIO_V2_1, flag);

	flag = (value & 0x04)>>2;
	GPIO_OutputCtrl(MIO_V3_1, flag);

	flag = (data & 0x01);
	GPIO_OutputCtrl(MIO_V1_2, flag);

	flag = (data & 0x02)>>1;
	GPIO_OutputCtrl(MIO_V2_2, flag);

	flag = (data & 0x04)>>2;
	GPIO_OutputCtrl(MIO_V3_2, flag);
}
