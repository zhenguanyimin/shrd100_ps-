/*
 * temperature.h
 *
 *  Created on: 2023年3月30日
 *      Author: A22745
 */

#ifndef SRC_HAL_DEVICE_IIC_HAL_TEMPERATURE_H_
#define SRC_HAL_DEVICE_IIC_HAL_TEMPERATURE_H_

#include "common_i2c.h"

#define VCC_3V3		(3.3F)
#define RESISTANCE_1	(4990)
#define RESISTANCE_2	(4990)

// 电压分辨率  FUL-SCALE RANGE(FSR) (4.069V)
#define VOLTAGE_RESOLUTION_125_UV	(125.0F)

// 电压分辨率  FUL-SCALE RANGE(FSR) (2.048v)
#define VOLTAGE_RESOLUTION_62_5_UV	(62.5F)

// 单次转换开始
#define ADS1115_REG_CONFIG_OS_START                     (0x1U << 7)//设备单词转换开启 高字节的最高位
#define ADS1115_REG_CONFIG_OS_NULL                      (0x0U << 7)

//输入引脚选择和输入方式选择
#define ADS1115_REG_CONFIG_MUX_Diff_01                  (0x0U << 4)  // 差分输入0引脚和1引脚 (默认)
#define ADS1115_REG_CONFIG_MUX_Diff_03                  (0x1U << 4)  // 差分输入0引脚和3引脚
#define ADS1115_REG_CONFIG_MUX_Diff_13                  (0x2U << 4)  // 差分输入1引脚和3引脚
#define ADS1115_REG_CONFIG_MUX_Diff_23                  (0x3U << 4)  // 差分输入2引脚和3引脚
#define ADS1115_REG_CONFIG_MUX_SINGLE_0                 (0x4U << 4)  //单端输入 0
#define ADS1115_REG_CONFIG_MUX_SINGLE_1                 (0x5U << 4)  //单端输入 1
#define ADS1115_REG_CONFIG_MUX_SINGLE_2                 (0x6U << 4)  //单端输入 2
#define ADS1115_REG_CONFIG_MUX_SINGLE_3                 (0x7U << 4)  //单端输入 3

//量程选择
#define ADS1115_REG_CONFIG_PGA_6                        (0x0U << 1) // +/- 6.1144
#define ADS1115_REG_CONFIG_PGA_4                        (0x1U << 1) // +/- 4.096
#define ADS1115_REG_CONFIG_PGA_2                        (0x2U << 1) // +/- 2.048 (默认)
#define ADS1115_REG_CONFIG_PGA_1                        (0x3U << 1) // +/- 1.024
#define ADS1115_REG_CONFIG_PGA_05                       (0x4U << 1) // +/- 0.512
#define ADS1115_REG_CONFIG_PGA_02                       (0x5U << 1) // +/- 0.256

//运行方式
#define ADS1115_REG_CONFIG_MODE_SINGLE              (0x1U << 0)  //  单次  (默认）
#define ADS1115_REG_CONFIG_MODE_CONTIN              (0x0U << 0)  //连续转换

//转换速率
#define ADS1115_REG_CONFIG_DR_8                     (0x0U << 5)
#define ADS1115_REG_CONFIG_DR_16                    (0x1U << 5)
#define ADS1115_REG_CONFIG_DR_32                    (0x2U << 5)
#define ADS1115_REG_CONFIG_DR_64                    (0x3U << 5)
#define ADS1115_REG_CONFIG_DR_128                   (0x4U << 5)
#define ADS1115_REG_CONFIG_DR_250                   (0x5U << 5)
#define ADS1115_REG_CONFIG_DR_475                   (0x6U << 5)
#define ADS1115_REG_CONFIG_DR_860                   (0x7U << 5)

//比较器模式
#define ADS1115_REG_CONFIG_COMP_MODE_TRADITIONAL    (0x0U << 4)//默认
#define ADS1115_REG_CONFIG_COMP_MODE_WINDOW         (0x1U << 4)

//比较器输出有效电平
#define ADS1115_REG_CONFIG_COMP_POL_LOW         (0x0U << 3)//默认
#define ADS1115_REG_CONFIG_COMP_POL_HIG         (0x1U << 3)

#define ADS1115_REG_CONFIG_COMP_LAT_NONLATCH        (0x0U << 2) // default
#define ADS1115_REG_CONFIG_COMP_LAT_LATCH           (0x1U << 2)

#define ADS1115_REG_CONFIG_COMP_QUE_ONE         (0x0U << 0)
#define ADS1115_REG_CONFIG_COMP_QUE_TWO         (0x1U << 0)
#define ADS1115_REG_CONFIG_COMP_QUE_THR         (0x2U << 0)
#define ADS1115_REG_CONFIG_COMP_QUE_DIS         (0x3U << 0) // default

// 寄存器配置 AIN0 单端输入AIN0、满量程2.048V、连续转换、转换速率128
//#define CONFIG_REG_AINO_H     (ADS1115_REG_CONFIG_OS_NULL|\
//                         	 ADS1115_REG_CONFIG_MUX_SINGLE_0|\
//							 ADS1115_REG_CONFIG_PGA_2|\
//							 ADS1115_REG_CONFIG_MODE_CONTIN)
//
//#define CONFIG_REG_AINO_L    (ADS1115_REG_CONFIG_DR_128|\
//                        	ADS1115_REG_CONFIG_COMP_MODE_TRADITIONAL|\
//							ADS1115_REG_CONFIG_COMP_POL_LOW|\
//							ADS1115_REG_CONFIG_COMP_LAT_NONLATCH|\
//							ADS1115_REG_CONFIG_COMP_QUE_DIS)
//
//
//// 寄存器配置 AIN1 单端输入AIN1、满量程2.048V、连续转换、转换速率128
//#define CONFIG_REG_AIN1_H     (ADS1115_REG_CONFIG_OS_NULL|\
//                         	 ADS1115_REG_CONFIG_MUX_SINGLE_1|\
//							 ADS1115_REG_CONFIG_PGA_2|\
//							 ADS1115_REG_CONFIG_MODE_CONTIN)
//
//#define CONFIG_REG_AIN1_L    (ADS1115_REG_CONFIG_DR_128|\
//                        	ADS1115_REG_CONFIG_COMP_MODE_TRADITIONAL|\
//							ADS1115_REG_CONFIG_COMP_POL_LOW|\
//							ADS1115_REG_CONFIG_COMP_LAT_NONLATCH|\
//							ADS1115_REG_CONFIG_COMP_QUE_DIS)


#define CONFIG_REG_AIN0_H     (ADS1115_REG_CONFIG_OS_START|\
                         	 ADS1115_REG_CONFIG_MUX_SINGLE_0|\
							 ADS1115_REG_CONFIG_PGA_2|\
							 ADS1115_REG_CONFIG_MODE_SINGLE)

#define CONFIG_REG_AIN0_L    (ADS1115_REG_CONFIG_DR_128|\
                        	ADS1115_REG_CONFIG_COMP_MODE_TRADITIONAL|\
							ADS1115_REG_CONFIG_COMP_POL_LOW|\
							ADS1115_REG_CONFIG_COMP_LAT_NONLATCH|\
							ADS1115_REG_CONFIG_COMP_QUE_DIS)

#define CONFIG_REG_AIN1_H     (ADS1115_REG_CONFIG_OS_START|\
                         	 ADS1115_REG_CONFIG_MUX_SINGLE_1|\
							 ADS1115_REG_CONFIG_PGA_2|\
							 ADS1115_REG_CONFIG_MODE_SINGLE)

#define CONFIG_REG_AIN1_L    (ADS1115_REG_CONFIG_DR_128|\
                        	ADS1115_REG_CONFIG_COMP_MODE_TRADITIONAL|\
							ADS1115_REG_CONFIG_COMP_POL_LOW|\
							ADS1115_REG_CONFIG_COMP_LAT_NONLATCH|\
							ADS1115_REG_CONFIG_COMP_QUE_DIS)

/***************/
//配置寄存器说明

//config register
/*CRH[15:8](R/W)
   BIT      15      14      13      12      11      10      9       8
   NAME     OS      MUX2    MUX1    MUX0    PGA2    PGA1    PGA0    MODE
CRL[7:0] (R/W)
   BIT      7       6       5       4       3       2       1       0
   NAME    DR0     DR1     DR0   COM_MODE COM_POL COM_LAT COM_QUE1 COM_QUE0


   -----------------------------------------------------------------------------------
 * 15    | OS             |  运行状态会单词转换开始
 *       |                | 写时:
 *       |                | 0   : 无效
 *       |                | 1   : 开始单次转换处于掉电状态时
 *       |                | 读时:
 *       |                | 0   : 正在转换
 *       |                | 1   : 未执行转换
 * -----------------------------------------------------------------------------------
 * 14:12 | MUX [2:0]      | 输入复用多路配置
 *       |                | 000 : AINP = AIN0 and AINN = AIN1 (default)
 *       |                | 001 : AINP = AIN0 and AINN = AIN3
 *       |                | 010 : AINP = AIN1 and AINN = AIN3
 *       |                | 011 : AINP = AIN2 and AINN = AIN3
 *       |                | 100 : AINP = AIN0 and AINN = GND
 *       |                | 101 : AINP = AIN1 and AINN = GND
 *       |                | 110 : AINP = AIN2 and AINN = GND
 *       |                | 111 : AINP = AIN3 and AINN = GND
 * -----------------------------------------------------------------------------------
 * 11:9  | PGA [2:0]      | 可编程增益放大器配置(FSR  full scale range)
 *       |                | 000 : FSR = В±6.144 V
 *       |                | 001 : FSR = В±4.096 V
 *       |                | 010 : FSR = В±2.048 V (默认)
 *       |                | 011 : FSR = В±1.024 V
 *       |                | 100 : FSR = В±0.512 V
 *       |                | 101 : FSR = В±0.256 V
 *       |                | 110 : FSR = В±0.256 V
 *       |                | 111 : FSR = В±0.256 V
 * -----------------------------------------------------------------------------------
 * 8     | MODE           | 工作模式
 *       |                | 0   : 连续转换
 *       |                | 1   : 单词转换
 * -----------------------------------------------------------------------------------
 * 7:5   | DR [2:0]       | 采样频率
 *       |                | 000 : 8 SPS
 *       |                | 001 : 16 SPS
 *       |                | 010 : 32 SPS
 *       |                | 011 : 64 SPS
 *       |                | 100 : 128 SPS (默认)
 *       |                | 101 : 250 SPS
 *       |                | 110 : 475 SPS
 *       |                | 111 : 860 SPS
 * -----------------------------------------------------------------------------------
 * 4     | COMP_MODE      | 比较器模式
 *       |                | 0   : 传统比较器 (default)
 *       |                | 1   : 窗口比较器
 * -----------------------------------------------------------------------------------
 * 3     | COMP_POL       | Comparator polarity
 *       |                | 0   : 低电平有效 (default)
 *       |                | 1   : 高电平有效
 * -----------------------------------------------------------------------------------
 * 2     | COMP_LAT       | Latching comparator
 *       |                | 0   : 非锁存比较器. (default)
 *       |                | 1   : 锁存比较器.
 * -----------------------------------------------------------------------------------
 * 1:0   | COMP_QUE [1:0] | Comparator queue and disable
 *       |                | 00  : Assert after one conversion
 *       |                | 01  : Assert after two conversions
 *       |                | 10  : Assert after four conversions
 *       |                | 11  : 禁用比较器并将ALERT/RDY设置为高阻抗 (default)
 * -----------------------------------------------------------------------------------
*/

typedef enum tmp75RegisterAddr {
	TMP75_TEMPERATURE_REGISTER 	= 0x00,
	TMP75_CONFIGURATION_REGISTER 	= 0x01,
	TMP75_T_LOW_REGISTER 			= 0x02,
	TMP75_T_HIGH_REGISTER 		= 0x03,
	TMP75_ONE_SHOT_REGISTER		= 0x04,
} tmp75RegisterAddr_t;

typedef enum nsa2300RegisterAddr {
	NSA2300_PART_ID_REGISTER 	= 0x01,
	NSA2300_STATUS_REGISTER 	= 0x02,

	NSA2300_DATA_MSB_REGISTER 	= 0x06,
	NSA2300_DATA_CSB_REGISTER 	= 0x07,
	NSA2300_DATA_LSB_REGISTER 	= 0x08,
	NSA2300_TEMP_MSB_REGISTER 	= 0x09,
	NSA2300_TEMP_LSB_REGISTER	= 0x0A,

	NSA2300_CMD_REGISTER		= 0X30,
	NSA2300_CONFIG_ID_REGISTER	= 0XA4,
	NSA2300_SYS_CONFIG_REGISTER	= 0XA5,
	NSA2300_P_CONFIG_REGISTER	= 0XA6,
	NSA2300_T_CONFIG_REGISTER	= 0XA7,
} nsa2300RegisterAddr_t;

typedef enum temperature_type {
	core_board_temperature_tmp75 = 0,
	power_board_temperature_nsa2300_1 = 1,
	battery_temperature = 2,
	power_board_temperature_ads1115_0 = 3,
	power_board_temperature_ads1115_1 = 4,
} temperature_type_t;


typedef enum ads1115RegisterAddr {
	ADS1115_CONVERSION_REGISTER = 0X00,
	ADS1115_CONFIG_REGISTER = 0X01,
	ADS1115_LO_THRESH_REGISTER = 0X02,
	ADS1115_HO_THRESH_REGISTER = 0X03,
} asd1115RegisterAddr_t;

typedef enum ads1115InputChannel {
	ADS1115_INPUT_CHANNEL_0_J27,
	ADS1115_INPUT_CHANNEL_1_J28,
	ADS1115_INPUT_CHANNEL_MAX_NUM,
} ads1115InputChannel_t;

ret_code_t GetTemperature(temperature_type_t type, float *getTemp);
ret_code_t Nsa2300_init(void);
void GetAds1115ConfigTest(void);

#endif /* SRC_HAL_DEVICE_IIC_HAL_TEMPERATURE_H_ */
