/*
 * output.h
 *
 *  Created on:
 *      Author: Ultimate
 */
#ifndef OUTPUT_H_
#define OUTPUT_H_
#include <sys/_stdint.h>
#include <xgpio.h>
#include <xil_exception.h>
#include <xscugic.h>
#include <xgpiops.h>
#include "../hal.h"

#define EMIO_OFFSET		77
#define GPIO_DEVICE_ID		XPAR_XGPIOPS_0_DEVICE_ID

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

typedef enum
{
	IO_VOL,
	IO_VOH,
} GPIO_STATUS;

typedef enum
{
	INPUT,
	OUTPUT,
} DIRECTION;

typedef enum
{
	MIO,
	EMIO,
	AXI_GPIO,
	AXI_BRAM,
	GPIO_MODE_MAX
} GPIO_MODE; //名字

typedef struct
{
	uint8_t Enable;//1 初始化   0 不初始化
	uint8_t GpioMode;
	uint8_t DeviceId;
	uint8_t AxiGpioChl;
	uint16_t Pin;
	uint8_t InitLevel;//初始电平,1 OR 0
}OUTPUT_ARG;

typedef enum
{
	MIO_PS_WD,	//U31:复位芯片喂狗信号,
	MIO_PS_ENET_RESET	,
	MIO_PS_USB_RESET	,
	MIO_PS_USB_CS	,
	MIO_EMMC0_SD0_RSTN,
	MIO_V3_1,
	MIO_V2_1,
	MIO_V1_1,
	MIO_V3_2,
	MIO_V2_2,
	MIO_V1_2,
	MIO_GPIO0_DISPLAY,
	MIO_GPIO1_DISPLAY,
	MIO_USB_5V_EN,
	MIO_VBUS_ON,
	MIO_EN_2,
	MIO_ETH_VCC_1V2_EN,
	MIO_VDD_3V3_EN,
	MIO_VDD_1V8_EN,
	MIO_VCC_5V_EN,
	MIO_SYS_LED0,
	MIO_SYS_LED1,
	EMIO_SYS_LED2,
	EMIO_SYS_LED3,

	EMIO_FAN_EN,   //输出高电平风扇供电
	EMIO_BUZZER_EN,   //输出高电平蜂鸣器供电电源输出
	EMIO_VBRATOR_EN,   //高电平马达电源输出
	EMIO_RG450_BT_EN,  //高电平蓝牙电源输出
	EMIO_VCC_GYRO_EN,  //高电平陀螺仪电源输出

	EMIO_SWITCH_OE2,   //高电平使能I2C电平转换和温度传感器中断输出的电平转换
	EMIO_AVCC_ADJ_EN,   //输出高电平使能运放的电源输出
	EMIO_DAC_3V3_EN,   //高电平输出DAC工作电压
	EMIO_G_CLK_EN,   //高电平使能时钟缓冲输出
	EMIO_AVCC_5V_EN,   //高电平使能运放5V工作电压输出
	EMIO_AVCC_1V8_EN,   //高电平使能ADC1.8V工作电压输出
	EMIO_ADC_RESET,   //低电平复位ADC芯片
	EMIO_ADC_SEN,   //低电平使能ADC通讯功能
	EMIO_SWITCH_5V_EN,   //高电平使能9361 5V输出
	EMIO_ADC_PDN,   //低电平给ADC开启供电
	EMIO_SWITCH_12V_EN,   //高电平使能开关滤波器12V输出
	EMIO_L_CTRL_D0,
	EMIO_L_CTRL_D1,
	EMIO_L_CTRL_D2,
	EMIO_L_CTRL_D3,
	EMIO_L_CTRL_D4,
	EMIO_L_CTRL_D5,
	EMIO_L_CTRL_D6,
	EMIO_SWITCH_OE0,   //控制数控衰减器衰减大小数字量（D0-D3）电平转换的控制开关，高电平开启电平转换
	EMIO_SWITCH_OE1,   //控制数控衰减器衰减大小数字量（D4-D6）电平转换的控制开关，高电平开启电平转换
	EMIO_CTRL_IN3,   //
	EMIO_CTRL_IN2,   //
	EMIO_CTRL_IN1,   //
	EMIO_CTRL_IN0,   //
	EMIO_ACC_CS,   //电子罗盘I2C/SPI mode selection1: SPI idle mode / I2C communication enabled;0: SPI communication mode / I2C disabled
	EMIO_PROG_SET1,   //程序运行状态指示灯控制1
	EMIO_PROG_SET2,   //程序运行状态指示灯控制2
	EMIO_PROG_SET3,   //程序运行状态指示灯控制3
	EMIO_PROG_SET4,   //程序运行状态指示灯控制4
	EMIO_GPS_RESET,   //GPS_RESET
	EMIO_GPS_FORCE_ON,   //GPS_FORCE_ON
	EMIO_SWITCH_OE5,   //上面三个IO口电平转换使能控制，高电平开启电平转换
	EMIO_SWITCH_OE4,   //下面四个IO口电平转换使能控制，高电平开启电平转换
	EMIO_SWITCH_OE7,   //下面四个IO口电平转换使能控制，高电平开启电平转换
	EMIO_SWITCH_OE6,   //下面两个IO口电平转换使能控制，高电平开启电平转换
	EMIO_WIFI_WAKEUP_OUT,   //
	EMIO_WIFI_RST,   //WIFI_RST
	EMIO_GYRO_CS,
	EMIO_WIFI_WAKEUP_IN_OUT,
	EMIO_DX_CTRL_OUT0,
	EMIO_DX_CTRL_OUT1,
	EMIO_CHANNEL_CTRLA_0,
#if RF_T3 == 1
	EMIO_RF_EN,
	EMIO_RF_RESET,
	EMIO_RF_CTLA_1,
	EMIO_RF_CTLB_1,
	EMIO_RF_CTLC_1,
	EMIO_RF_CTLD_1,
	EMIO_RF_CTLA_2,
	EMIO_RF_CTLB_2,
	EMIO_RF_CTLC_2,
	EMIO_RF_CTLD_2,
	EMIO_RF_CTLA_3,
	EMIO_RF_CTLB_3,
	EMIO_RF_CTLC_3,
	EMIO_RF_CTLD_3,
#else
	EMIO_RF_CTLA_0,
	EMIO_RF_CTLB_0,
	EMIO_RF_CTLA_1,
	EMIO_RF_CTLB_1,
	EMIO_RF_CTLA_2,
	EMIO_RF_CTLB_2,
	EMIO_RF_CTLA_3,
	EMIO_RF_CTLB_3,
#endif
	OUTPUT_NAME_MAX
} OUTPUT_NAME; //名字

XGpioPs *GetPsGpioDev(void);
XGpio *GetPlGpioDev(uint8_t id);
int32_t OutputInit(void);
void GPIO_OutputCtrl(OUTPUT_NAME name, uint8_t value);
uint8_t Output_GetValue(OUTPUT_NAME name);
void write_register(uint32_t regAddr, uint32_t value);
#endif /* OUTPUT_H_ */
