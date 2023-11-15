
/*
 * hal.h
 *
 *  Created on:
 *      Author: Ultimate
 */
#ifndef HAL_H_
#define HAL_H_
#include <sys/_stdint.h>
#include "xil_io.h"
typedef enum
{
	LED_OFF,
	LED_ON,
} LED_SWITCH;

typedef enum
{
	IO_LOW,
	IO_HIGTH,
} IO_STATUS;

typedef enum
{
	PL_AD_START = 0x00, // AD使能寄存器 0：打击状态；1：侦测状态                                                                                   
	PL_LO_CODE = 0x01, // ad9361本振号
	PL_REG_AD0 = 0x02, // AD配置寄存器 对应上一版本0x0010地址寄存器(userreg_0x0010)
	PL_REG_AD1 = 0x03, // AD配置寄存器 对应上一版本0x0011地址寄存器(userreg_0x0011)
	PL_REG_AD2 = 0x04, // AD配置寄存器 对应上一版本0x0012地址寄存器(userreg_0x0012)
	PL_REG_AD3 = 0x05, // AD配置寄存器 对应上一版本0x0020地址寄存器(userreg_0x0020)bit[0]：o_ad_txnrx、硬件管脚(AA5)；bit[1]：o_ad_enable、硬件管脚(AE3)； bit[2]：o_ad_enagc、硬件管脚(Y1)；bit[3]：o_ad_rst，硬件管脚(AA6)；
	PL_DATA_SELA = 0x06, // A通道输出频段选择 0：5.2GHz；1：5.8GHz；2：5.2~5.8
	PL_DATA_SELB = 0x07, // B通道输出频段选择 0：2.4GHz；1：5.8GHz；2：2.4~3.5
	PL_SL_SWITCH = 0x08, // 门限检测结果输出、参数测量结果输出、外协算法结果输出功能切换 0：门限检测输出(算法模块一)；      1：参数测量结果输出(算法模块二)；        2：外协算法前处理结果输出(算法模块三)；
	PL_DLY_EN_VTC = 0x09, // IDELAY控制
	PL_DLY_LOAD = 0x0A, // IDELAY控制
	PL_DLY_D = 0x0B, // IDELAY控制
	PL_PL_START = 0x0c, // PL上电正常，可以进行初始化操作 0：PL端上电异常；      1：PL端上电成功 
	PL_FAN_PWM_RATE = 0x0d, // FAN转速控制 风扇转速分0~10档,0档转速为0;10档转速最快;
	PL_VOERSION_VP0 = 0x0E, // 版本号0 编号+年：如：00022022
	PL_VOERSION_VP1 = 0x0F, // 版本号1 月+日+小时+分钟  如：10151800
	PL_IRDRATE = 0x10, // 2-4G/4-6G 打击速度间隔设置 0：5.12us     1：10.24us  2：15.36us 3:20.48
	PL_MOTOR_AD4 = 0x11, // AD配置寄存器 对应上一版本0x0090地址寄存器(userreg_0x0090)
	PL_MOTOR_AD5 = 0x12, // AD配置寄存器 对应上一版本0x0091地址寄存器(userreg_0x0091)
	PL_MOTOR_AD6 = 0x13, // AD配置寄存器 对应上一版本0x009f地址寄存器(userreg_0x009f)
	PL_I_FPGA_TEMP0 = 0x14, //
	PL_I_FPGA_TEMP1 = 0x15, //
	PL_I_FPGA_TEMP2 = 0x16, //
	PL_I_FPGA_TEMP3_DIR = 0x17, // AD9361输出通道选择寄存器  0：全向通道  1：定向通道
	PL_I_FPGA_CURR0 = 0x18, //
	PL_I_FPGA_CURR1 = 0x19, //
	PL_I_FPGA_CURR2 = 0x1A, //
	PL_I_FPGA_BAT = 0x1B, //
	PL_BUZZER_EN = 0x2F, //0：关闭蜂鸣器          1：打开蜂鸣器
	PL_BUZZER_PARA = 0x30, //周期T=250us(以200M时钟为例)，   高电平维持时间t1=1/200M*（设置值)          低电平维持时间：t2=T-t1；

	PL_HIT_CHB_2_4 = 0x0100, //
	PL_HIT_CHB_5_8 = 0x0300, //
	PL_HIT_CHA_5_2 = 0x0500, //
	PL_HIT_CHA_5_3 = 0x0700, //
	PL_HIT_CHA_5_4 = 0x0900, //
	PL_HIT_CHA_5_8 = 0x0B00, //

} PL_REG;

void axi_write_data(unsigned int addr, int dat);
int axi_read_data(unsigned int addr);

void axi_write(unsigned int addr, int dat);
unsigned int axi_read(unsigned int addr);
#endif /* HAL_H_ */
