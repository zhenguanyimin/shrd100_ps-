
#include "ad9361_config.h"
#include "ad9361_config_droneid.h"
#include "zynq_bram.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xparameters.h"

#include "../../srv/log/log.h"
#include "../output/output.h"

#include "FreeRTOSConfig.h"
#define AD9361_FRE_HOP_TIME (124)
extern XScuGic xInterruptController;
uint8_t uGps915mHitFlag = Freq_TX_GPS_915m;

static uint8_t Ad9361GainBuf[5] = {0x32, 0x28, 0x00, 0x00, 0x00};

enum Freq_CFG_ADDR
{
	fre_433m_addr = 0x00,
	//	fre_1931m_addr = 0x00
	fre_842m_addr,
	fre_915m_addr,
	fre_1433m_addr,
//	fre_1185m_addr,
	fre_1225m_addr,
//	fre_1187m_addr,
//	fre_1212m_addr,
//	fre_1253m_addr,
	fre_1265m_addr,
	fre_1572m_addr,
	fre_1597m_addr,
	fre_max,
};

enum Freq_CFG1_ADDR
{
	fre1_842m_addr,
	fre1_915m_addr,
	fre1_1187m_addr,
	fre1_1212m_addr,
	fre1_1253m_addr,
	fre1_1265m_addr,
	fre1_1572m_addr,
	fre1_1597m_addr,
	fre1_max,
};

enum Freq_CFG2_ADDR
{
	fre2_828m_addr,
	fre2_838m_addr,
	fre2_848m_addr,
	fre2_858m_addr,
	fre2_900m_addr,
	fre2_910m_addr,
	fre2_920m_addr,
	fre2_930m_addr,
	fre2_max,
};

enum Freq_CFG3_ADDR
{
	fre3_1087m_addr,
	fre3_1127m_addr,
	fre3_1167m_addr,
	fre3_1207m_addr,
	fre3_1247m_addr,
	fre3_1287m_addr,
	fre3_1327m_addr,
	fre3_1367m_addr,
	fre3_max,
};

enum Freq_CFG4_ADDR
{
//	fre4_433m_addr,
	fre4_842m_addr,
	fre4_915m_addr,
	fre4_1775m_addr,
	fre4_1185m_addr,
	fre4_1225m_addr,
	fre4_1265m_addr,
	fre4_1572m_addr,
	fre4_1597m_addr,
	fre4_max,
};

RfTxProperty_t RfTxProperty[fre_max] =
{
		[fre_433m_addr]   = {ENABLE, 0},
		[fre_1433m_addr]  = {ENABLE, 0},
		[fre_842m_addr]   = {ENABLE, 0},
		[fre_915m_addr]   = {ENABLE, 0},
		[fre_1225m_addr]   = {ENABLE, 1},
		[fre_1265m_addr]   = {ENABLE, 1},
		[fre_1572m_addr]  = {ENABLE, 1},
		[fre_1597m_addr]  = {ENABLE, 1}
};

RfTxProperty_t RfTxProperty1[fre1_max] =
{
		[fre1_842m_addr]   = {ENABLE, 0},
		[fre1_915m_addr]   = {ENABLE, 0},
		[fre1_1187m_addr]  = {ENABLE, 1},
		[fre1_1212m_addr]  = {ENABLE, 1},
		[fre1_1253m_addr]  = {ENABLE, 1},
		[fre1_1265m_addr]  = {ENABLE, 1},
		[fre1_1572m_addr]  = {ENABLE, 1},
		[fre1_1597m_addr]  = {ENABLE, 1}
};

RfTxProperty_t RfTxProperty2[fre2_max] =
{
		[fre2_828m_addr]  = {ENABLE, 0},
		[fre2_838m_addr]  = {ENABLE, 0},
		[fre2_848m_addr]  = {ENABLE, 0},
		[fre2_858m_addr]  = {ENABLE, 0},
		[fre2_900m_addr]  = {ENABLE, 0},
		[fre2_910m_addr]  = {ENABLE, 0},
		[fre2_920m_addr]  = {ENABLE, 0},
		[fre2_930m_addr]  = {ENABLE, 0}
};

RfTxProperty_t RfTxProperty3[fre3_max] =
{
		[fre3_1087m_addr] = {ENABLE, 0},
		[fre3_1127m_addr] = {ENABLE, 0},
		[fre3_1167m_addr] = {ENABLE, 0},
		[fre3_1207m_addr] = {ENABLE, 0},
		[fre3_1247m_addr] = {ENABLE, 0},
		[fre3_1287m_addr] = {ENABLE, 0},
		[fre3_1327m_addr] = {ENABLE, 0},
		[fre3_1367m_addr] = {ENABLE, 0},
};
RfTxProperty_t RfTxProperty4[fre4_max] =
{
//		[fre4_842m_addr] = {ENABLE, 0},
//		[fre4_915m_addr] = {ENABLE, 0},
		[fre4_1775m_addr] = {ENABLE, 0},
		[fre4_1185m_addr] = {ENABLE, 1},
		[fre4_1225m_addr] = {ENABLE, 1},
		[fre4_1265m_addr] = {ENABLE, 1},
		[fre4_1572m_addr] = {ENABLE, 1},
		[fre4_1597m_addr] = {ENABLE, 1},
};

uint8_t reg_value_list_433m[17] =    {0x30, 0x56, 0x00, 0x66, 0xC4, 0x4C, 0x5B, 0x32, 0x69, 0x82, 0x8F, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0D};
uint8_t reg_value_list_1931m[17] =   {0x11, 0x60, 0x00, 0x5E, 0x66, 0x46, 0x4F, 0xCE, 0x60, 0xC1, 0x90, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0C};
uint8_t reg_value_list_842m[17] =    {0x20, 0x54, 0x00, 0x50, 0x97, 0x19, 0x5B, 0x32, 0x69, 0x82, 0x8F, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0D};
uint8_t reg_value_list_915m[17] =    {0x20, 0x5B, 0x00, 0x7F, 0xFD, 0x3F, 0x58, 0xF0, 0x70, 0x82, 0x8E, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};
uint8_t reg_value_list_1187m[17] =   {0x20, 0x76, 0x00, 0x5A, 0x96, 0x59, 0x58, 0x5E, 0x60, 0x81, 0x8C, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1212m[17] =   {0x20, 0x79, 0x00, 0x50, 0x96, 0x19, 0x53, 0x55, 0x68, 0x81, 0x8B, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1253m[17] =   {0x20, 0x7D, 0x00, 0xFF, 0x62, 0x26, 0x50, 0x48, 0x70, 0x81, 0x8B, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1265m[17] =   {0x20, 0x7E, 0x00, 0x8D, 0xFC, 0x3F, 0x50, 0x44, 0x78, 0x81, 0x8A, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1433m[17] =   {0x20, 0x8F, 0x00, 0x62, 0x66, 0x26, 0x54, 0x19, 0x78, 0xC1, 0x88, 0xF6, 0xD5, 0x0E, 0x14, 0x70, 0x0C};
uint8_t reg_value_list_1572m[17] =   {0x10, 0x4E, 0x00, 0xA4, 0xCA, 0x4C, 0x51, 0x72, 0x79, 0x82, 0x8F, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};
uint8_t reg_value_list_1597m[17] =   {0x10, 0x4F, 0x00, 0x97, 0xCA, 0x6C, 0x50, 0x63, 0x79, 0x82, 0x8F, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};


uint8_t reg_value_list_828m[17] =    {0x21, 0x52, 0x00, 0x5A, 0x66, 0x66, 0x19, 0x5B, 0x68, 0xC2, 0x8F, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0D};
uint8_t reg_value_list_838m[17] =    {0x21, 0x53, 0x00, 0x5A, 0x66, 0x66, 0x19, 0x5B, 0x70, 0xC2, 0x8F, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};
uint8_t reg_value_list_848m[17] =    {0x21, 0x54, 0x00, 0x5A, 0x66, 0x66, 0x19, 0x5B, 0x68, 0xC2, 0x8F, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0D};
uint8_t reg_value_list_858m[17] =    {0x21, 0x55, 0x00, 0x5A, 0x66, 0x66, 0x19, 0x5B, 0x70, 0xC2, 0x8F, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};
//uint8_t reg_value_list_900m[17] =    {0x21, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x19, 0x5B, 0x70, 0xC2, 0x8E, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};
uint8_t reg_value_list_910m[17] =    {0x21, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x19, 0x5B, 0x70, 0xC2, 0x8E, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};
uint8_t reg_value_list_920m[17] =    {0x21, 0x5C, 0x00, 0x00, 0x00, 0x00, 0x19, 0x5B, 0x70, 0xC2, 0x8E, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};
uint8_t reg_value_list_930m[17] =    {0x21, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x19, 0x5B, 0x70, 0xC2, 0x8D, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};


uint8_t reg_value_list_1087m[17] =   {0x21, 0x6C, 0x00, 0x8F, 0x99, 0x59, 0x57, 0x85, 0x60, 0xC1, 0x8E, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0C};
uint8_t reg_value_list_1127m[17] =   {0x21, 0x70, 0x00, 0x8F, 0x99, 0x59, 0x68, 0x73, 0x60, 0xC1, 0x8D, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1167m[17] =   {0x21, 0x74, 0x00, 0x8F, 0x99, 0x59, 0x61, 0x63, 0x60, 0xC1, 0x8C, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1207m[17] =   {0x21, 0x78, 0x00, 0x8F, 0x99, 0x59, 0x57, 0x55, 0x68, 0xC1, 0x8B, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1247m[17] =   {0x21, 0x7C, 0x00, 0x8F, 0x99, 0x59, 0x55, 0x48, 0x70, 0xC1, 0x8B, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1287m[17] =   {0x21, 0x80, 0x00, 0x8F, 0x99, 0x59, 0x54, 0x3B, 0x78, 0xC1, 0x8A, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1327m[17] =   {0x21, 0x84, 0x00, 0x8F, 0x99, 0x59, 0x4C, 0x30, 0x78, 0xC1, 0x89, 0xE6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1367m[17] =   {0x21, 0x88, 0x00, 0x8F, 0x99, 0x59, 0x68, 0x26, 0x78, 0xC1, 0x89, 0xF6, 0xD5, 0x0E, 0x14, 0x70, 0x0C};

uint8_t reg_value_list_1185m[17] =   {0x21, 0x76, 0x00, 0xF8, 0xFF, 0x3F, 0x5B, 0x5D, 0x60, 0xC1, 0x8C, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
uint8_t reg_value_list_1225m[17] =   {0x21, 0x7A, 0x00, 0xF8, 0xFF, 0x3F, 0x57, 0x5D, 0x68, 0xC1, 0x8B, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};
//uint8_t reg_value_list_1775m[17] =   {0x11, 0x60, 0x00, 0x5E, 0x66, 0x46, 0x4F, 0xCE, 0x60, 0xC1, 0x90, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0C};
uint8_t reg_value_list_1775m[17] =   {0x11, 0x58, 0x00, 0xF5, 0xFF, 0x5F, 0x7F, 0xFF, 0x70, 0xC2, 0x8F, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};

//uint8_t reg_value_list_820m[17] =    {0x20, 0x54, 0x00, 0x50, 0x97, 0x19, 0x5B, 0x32, 0x69, 0x82, 0x8F, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0D};
//uint8_t reg_value_list_860m[17] =    {0x20, 0x54, 0x00, 0x50, 0x97, 0x19, 0x60, 0x30, 0x69, 0x82, 0x8F, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0D};
//uint8_t reg_value_list_900m[17] =    {0x20, 0x5B, 0x00, 0x7F, 0xFD, 0x3F, 0x50, 0xF0, 0x70, 0x82, 0x8E, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};
//uint8_t reg_value_list_940m[17] =    {0x20, 0x5B, 0x00, 0x7F, 0xFD, 0x3F, 0x58, 0xF0, 0x70, 0x82, 0x8E, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};
//uint8_t reg_value_list_1280m[17] =   {0x21, 0x80, 0x00, 0x8F, 0x99, 0x59, 0x54, 0x3B, 0x78, 0xC1, 0x8A, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};

uint8_t reg_value_list_820m[17] =    {0x21, 0x51, 0x00, 0xba, 0xfd, 0x7f, 0x53, 0x4a, 0x69, 0x82, 0x8F, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0D};
uint8_t reg_value_list_860m[17] =    {0x21, 0x55, 0x00, 0x9e, 0xfd, 0x7f, 0x58, 0x20, 0x71, 0x82, 0x8F, 0xF6, 0xD5, 0x0E, 0x1f, 0x70, 0x0D};
uint8_t reg_value_list_900m[17] =    {0x21, 0x59, 0x00, 0x82, 0xfd, 0x7f, 0x58, 0xFC, 0x70, 0x82, 0x8E, 0xF6, 0xD5, 0x0E, 0x1F, 0x70, 0x0D};
uint8_t reg_value_list_940m[17] =    {0x21, 0x5D, 0x00, 0x66, 0xFD, 0x7F, 0x53, 0xDF, 0x60, 0x81, 0x91, 0xF6, 0xD5, 0x0E, 0x17, 0x70, 0x0C};
uint8_t reg_value_list_1280m[17] =   {0x21, 0x7F, 0x00, 0x7B, 0xFC, 0x7F, 0x4B, 0x40, 0x78, 0x81, 0x8A, 0xF6, 0xD5, 0x0E, 0x16, 0x70, 0x0C};

uint16_t TX_read_reg_addr_list[17]=  {0x05,0x271,0x272,0x273,0x274,0x275,0x276,0x277,0x278,0x279,0x27B,0x27E,0x27F,0x280,0x282,0x290,0x291};
void SetHitFlag( uint8_t type )
{
	uGps915mHitFlag = type ;
}

int32_t InitAd9361(void)
{
	vTaskDelay(51);

//	usleep(5000);
	// zynq_uart_test();
	print("TX:reset ad936x...\r\n\r");
	reset_ad936x();
	vTaskDelay(100);

	print("TX:ad9361 cfg start...\r\n\r");
	//	cfgAd9361();
//	Ad9361_fre_hop_init();
	print("TX:ad9361 cfg finish...\r\n\r");
	enable_ad936x();
	print("TX:fpga_write(0x30,0x01); \r\n");
	write_fpga_regs(0x30, 0x01);
	print("TX:fpga_write(0x31,0x09); \r\n");
	write_fpga_regs(0x31, 0x09);

	return 0;
}

int32_t InitAd9361_Rx(void)
{
	vTaskDelay(5);

//	usleep(5000);
	// zynq_uart_test();
	print("RX:reset ad936x...\r\n\r");
	reset_ad936x();
//	usleep(1000000);
	vTaskDelay(100);

	print("RX:ad9361 cfg start...\r\n\r");

	cfgAd9361();

	//	Ad9361_fre_hop_init();
	print("RX:ad9361 cfg finish...\r\n\r");
	enable_ad936x();
	print("RX:fpga_write(0x30,0x01); \r\n");
	write_fpga_regs(0x30, 0x01);
	print("RX:fpga_write(0x31,0x09); \r\n");
	write_fpga_regs(0x31, 0x09);

	return 0;
}


int32_t InitAd9361_Rx_droneid(void)
{
	vTaskDelay(5);

//	usleep(5000);
	// zynq_uart_test();
	print("RX:reset ad936x...\r\n\r");
	reset_ad936x();
//	usleep(1000000);
	vTaskDelay(100);

	print("RX:ad9361 cfg start...\r\n\r");

	cfgAd9361_droneid();

	//	Ad9361_fre_hop_init();
	print("RX:ad9361 cfg finish...\r\n\r");
	enable_ad936x();
	print("RX:fpga_write(0x30,0x01); \r\n");
	write_fpga_regs(0x30, 0x01);
	print("RX:fpga_write(0x31,0x09); \r\n");
	write_fpga_regs(0x31, 0x09);

	return 0;
}

u8 Ad9361FreqConfig_RX_433m[][3] = {
	//		//************************************************************
	//		// Write Rx Frequency Words
	//		//************************************************************
	{0x82, 0x33, 0xC4}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x56}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x23}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_842m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x50}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x97}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x19}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x54}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_915m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x7F}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFD}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x5B}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_1200m[][3] = {
	//		//************************************************************
	//		// Write Rx Frequency Words
	//		//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x78}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_837m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x8F}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x99}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x59}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x53}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x12}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_1775m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF5}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x99}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x5F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x58}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
};

//-------------------------------add by 2023年8月16日17:45:04----------------------------------------------------------
u8 Ad9361FreqConfig_RX_1080m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x6C}, // SPIWrite	231,6C	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_1120m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x70}, // SPIWrite	231,70	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
};
u8 Ad9361FreqConfig_RX_1160m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x74}, // SPIWrite	231,74	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_1240m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7C}, // SPIWrite	231,7C	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_1280m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x80}, // SPIWrite	231,80	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_1320m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x84}, // SPIWrite	231,84	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_1360m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x88}, // SPIWrite	231,88	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x22}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5655m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFA}, // SPIWrite	233,FA	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,FF	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x2F}, // SPIWrite	235,2F	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8D}, // SPIWrite	231,8D	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x00}, // SPIWrite	005,00	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5695m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFA}, // SPIWrite	233,FA	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,FF	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x2F}, // SPIWrite	235,2F	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8E}, // SPIWrite	231,8E	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x00}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5725m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFE}, // SPIWrite	233,FE	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,FF	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x0F}, // SPIWrite	235,0F	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8F}, // SPIWrite	231,8F	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x20}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5850m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFC}, // SPIWrite	233,FC	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,FF	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x1F}, // SPIWrite	235,1F	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x92}, // SPIWrite	231,92	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x00}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5890m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFC}, // SPIWrite	233,FC	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,FF	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x1F}, // SPIWrite	235,1F	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x93}, // SPIWrite	231,93	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x00}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5930m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFC}, // SPIWrite	233,FC	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,FF	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x1F}, // SPIWrite	235,1F	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x94}, // SPIWrite	231,94	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x00}, // SPIWrite	005,00	// Set LO divider setting
};

//-----------------------------------------------------------------------------------------------------

u8 Ad9361FreqConfig_RX_2406m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x62}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x26}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x78}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_2420m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x79}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_2422m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xCB}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x0C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x79}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_2437m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xC0}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x6C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x79}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_2460m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7B}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_2462m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xCB}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x0C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7B}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5160m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x81}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_5180m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x81}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5200m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x82}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5220m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x82}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5240m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x83}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5745m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8F}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5785m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x90}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5765m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFE}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x0F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x90}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x20}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5805m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFE}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x0F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x91}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5825m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x91}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5865m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x92}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_875m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x57}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x12}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_2417m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xC0}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x6C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x78}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_2447m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xC8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x2C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7A}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_2477m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xC0}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x6C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7B}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5157m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x59}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x76}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x80}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5187m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x5C}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x56}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x81}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5217m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x60}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x36}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x82}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5247m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x64}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x16}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x83}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5742m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x5E}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x46}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8F}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_5772m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x62}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x26}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x90}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_5802m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x66}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x06}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x91}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_5832m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x5A}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x66}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x91}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_5862m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x5E}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x66}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x46}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x92}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
						//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

//////////////////////////////////////////////////V3��չƵ��///////////////////////////////////////////////////////////////////////////
u8 Ad9361FreqConfig_RX_1437m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x8F}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x99}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x59}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8F}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x12}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_4870m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF5}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x5F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x79}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_4915m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF3}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x6F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7A}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_4960m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7C}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5000m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7D}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};


u8 Ad9361FreqConfig_RX_5040m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7E}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};
u8 Ad9361FreqConfig_RX_5080m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7D}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};
u8 Ad9361FreqConfig_RX_5120m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x80}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};
u8 Ad9361FreqConfig_RX_5280m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x84}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};
u8 Ad9361FreqConfig_RX_5320m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x85}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};
u8 Ad9361FreqConfig_RX_5360m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x86}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};
u8 Ad9361FreqConfig_RX_5400m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0x00}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0x00}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x00}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x87}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};
u8 Ad9361FreqConfig_RX_5830m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF5}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x5F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x91}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};


u8 Ad9361FreqConfig_RX_5445m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFE}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x0F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x88}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5490m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFC}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x1F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x89}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5535m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xFA}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x2F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8A}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5580m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8B}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5625m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8C}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5665m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8D}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};


u8 Ad9361FreqConfig_RX_5705m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF7}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x8E}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5900m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x93}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5860m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x92}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5940m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x94}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_5980m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xF8}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xFF}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x3F}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x95}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
};

u8 Ad9361FreqConfig_RX_2452m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xC4}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x4C}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x7A}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x11}, // SPIWrite	005,22	// Set LO divider setting
							//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};

u8 Ad9361FreqConfig_RX_5766_5m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xCA}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x14}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x90}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
							//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
u8 Ad9361FreqConfig_RX_5806_5m[][3] = {
	//************************************************************
	// Write Rx Frequency Words
	//************************************************************
	{0x82, 0x33, 0xCA}, // SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
	{0x82, 0x34, 0xCC}, // SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
	{0x82, 0x35, 0x14}, // SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
	{0x82, 0x32, 0x00}, // SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
	{0x82, 0x31, 0x91}, // SPIWrite	231,78	// Write Rx Synth Integer Freq Word[7:0]
	{0x80, 0x05, 0x10}, // SPIWrite	005,22	// Set LO divider setting
							//		{0x02, 0x47, 0x11}, // SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
};
/* Sets the TX frequency. */
void set_tx_freq(float freq)
{
}
/* Sets the RX frequency. */

#define EQUAL( x , y )  fabs((x) - (y)) < 0.000001

float SetRxFreq(float uLocalOscFreq)
{
	uint8_t i = 0;
	uint8_t cnt = 6;
	float freq = 0.0;

	if( EQUAL(uLocalOscFreq,842)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_842m[i]);
		}
		freq = 842 ;
	}
	else if( EQUAL(uLocalOscFreq,915)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_915m[i]);
		}
		freq = 915 ;
	}
	else if( EQUAL(uLocalOscFreq,1080)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1080m[i]);
		}
		freq = 1080 ;
	}
	else if( EQUAL(uLocalOscFreq,1120)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1120m[i]);
		}
		freq = 1120 ;
	}
	else if( EQUAL(uLocalOscFreq,1160)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1160m[i]);
		}
		freq = 1160 ;
	}
	else if( EQUAL(uLocalOscFreq,1200)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1200m[i]);
		}
		freq = 1200 ;
	}
	else if( EQUAL(uLocalOscFreq,1240)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1240m[i]);
		}
		freq = 1240 ;
	}
	else if( EQUAL(uLocalOscFreq,1280)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1280m[i]);
		}
		freq = 1280 ;
	}
	else if( EQUAL(uLocalOscFreq,1320)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1320m[i]);
		}
		freq = 1320 ;
	}
	else if( EQUAL(uLocalOscFreq,1360)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1360m[i]);
		}
		freq = 1360 ;
	}
	else if( EQUAL(uLocalOscFreq,1437)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1437m[i]);
		}
		freq = 1437 ;
	}
	else if( EQUAL(uLocalOscFreq,2422)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_2422m[i]);
		}
		freq = 2422 ;
	}
	else if( EQUAL(uLocalOscFreq,2437)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_2437m[i]);
		}
		freq = 2437 ;
	}
	else if( EQUAL(uLocalOscFreq,2462)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_2462m[i]);
		}
		freq = 2462 ;
	}
//	else if( EQUAL(uLocalOscFreq,4915)  )
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_4915m[i]);
//		}
//		freq = 4915 ;
//	}
	else if( EQUAL(uLocalOscFreq,5160)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5160m[i]);
		}
		freq = 5160 ;
	}
	else if( EQUAL(uLocalOscFreq,5180)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5180m[i]);
		}
		freq = 5180 ;
	}
	else if( EQUAL(uLocalOscFreq,5200)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5200m[i]);
		}
		freq = 5200 ;
	}
	else if( EQUAL(uLocalOscFreq,5220)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5220m[i]);
		}
		freq = 5220 ;
	}
	else if( EQUAL(uLocalOscFreq,5240)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5240m[i]);
		}
		freq = 5240 ;
	}
//	else if( EQUAL(uLocalOscFreq,5490)  )
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5490m[i]);
//		}
//		freq = 5490 ;
//	}
	else if( EQUAL(uLocalOscFreq,5655)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5655m[i]);
		}
		freq = 5655 ;
	}
	else if( EQUAL(uLocalOscFreq,5695)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5695m[i]);
		}
		freq = 5695 ;
	}
//	else if( EQUAL(uLocalOscFreq,5665)  )
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5665m[i]);
//		}
//		freq = 5665 ;
//	}
	else if( EQUAL(uLocalOscFreq,5725)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5725m[i]);
		}

		freq = 5725 ;
	}
	else if( EQUAL(uLocalOscFreq,5745)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5745m[i]);
		}

		freq = 5745 ;
	}
	else if( EQUAL(uLocalOscFreq,5765)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5765m[i]);
		}

		freq = 5765 ;
	}
	else if( EQUAL(uLocalOscFreq,5785)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5785m[i]);
		}

		freq = 5785 ;
	}
	else if( EQUAL(uLocalOscFreq,5805)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5805m[i]);
		}

		freq = 5805 ;
	}
	else if( EQUAL(uLocalOscFreq,5825)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5825m[i]);
		}

		freq = 5825 ;
	}
	else if( EQUAL(uLocalOscFreq,5850)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5850m[i]);
		}
		freq = 5850 ;
	}
	else if( EQUAL(uLocalOscFreq,5890)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5890m[i]);
		}
		freq = 5890 ;
	}
	else if( EQUAL(uLocalOscFreq,5930)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5930m[i]);
		}
		freq = 5930 ;
	}
//	else if( EQUAL(uLocalOscFreq,5860)  )
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5860m[i]);
//		}
//		freq = 5860 ;
//	}
	else if( EQUAL(uLocalOscFreq,2422)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_2422m[i]);
		}
		freq = 2422 ;
	}
	else if( EQUAL(uLocalOscFreq,2452)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_2452m[i]);
		}
		freq = 2452 ;
	}
	else if( EQUAL(uLocalOscFreq,5766.5)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5766_5m[i]);
		}
		freq = 5766.5 ;
	}
	else if( EQUAL(uLocalOscFreq,5806.5)  )
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5806_5m[i]);
		}
		freq = 5806.5 ;
	}
	else
	{

	}
	return freq ;
}

uint32_t set_rx_freq(uint32_t freqItem)
{
	uint8_t i = 0;
	uint32_t freq = 0;
	uint8_t cnt = 6;
	static uint8_t GainCutoverIndex = 0;
	static uint8_t GainCnt = 0;
	enum Freq_CFG localFreqItem = (enum Freq_CFG)freqItem;

	switch (localFreqItem)
	{
	case Freq_RX_842m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_842m[i]);
		}
		freq = 842;
	}break;
	case Freq_RX_915m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_915m[i]);
		}
		freq = 915;
	}break;

	case Freq_RX_1080m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1080m[i]);
		}
		freq = 1080;
	}break;

	case Freq_RX_1120m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1120m[i]);
		}
		freq = 1120;
	}break;

	case Freq_RX_1160m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1160m[i]);
		}
		freq = 1160;
	}break;

	case Freq_RX_1200m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1200m[i]);
		}
		freq = 1200;
	}break;

	case Freq_RX_1240m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1240m[i]);
		}
		freq = 1240;
	}break;

	case Freq_RX_1280m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1280m[i]);
		}
		freq = 1280;
	}break;

	case Freq_RX_1320m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1320m[i]);
		}
		freq = 1320;
	}break;

	case Freq_RX_1360m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1360m[i]);
		}
		freq = 1360;
	}break;

	case Freq_RX_1437m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_1437m[i]);
		}
		freq = 1437;
	}break;
//	case Freq_RX_1775m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_1775m[i]);
//		}
//		freq = 1775;
//	}break;
//	case Freq_RX_2420m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_2420m[i]);
//		}
//		freq = 2420;
//	}
//	break;
	case Freq_RX_2422m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_2422m[i]);
		}
		freq = 2422;
	}
	break;
	case Freq_RX_2437m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_2437m[i]);
		}
		freq = 2437;
	}
	break;
//	case Freq_RX_2460m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_2460m[i]);
//		}
//		freq = 2460;
//	}
//	break;
	case Freq_RX_2462m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_2462m[i]);
		}
		freq = 2462;
	}
	break;
	case Freq_RX_5160m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5160m[i]);
		}
		freq = 5160;
	}break;
	case Freq_RX_5180m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5180m[i]);
		}
		freq = 5180;
	}break;
	case Freq_RX_5200m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5200m[i]);
		}
		freq = 5200;
	}break;
	case Freq_RX_5220m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5220m[i]);
		}
		freq = 5220;
	}break;
	case Freq_RX_5240m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5240m[i]);
		}
		freq = 5240;
	}break;
	case Freq_RX_5655m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5655m[i]);
		}
		freq = 5655;
	}break;
	case Freq_RX_5695m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5695m[i]);
		}
		freq = 5695;
	}break;
	case Freq_RX_5725m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5725m[i]);
		}
		freq = 5725;
	}
	break;

	case Freq_RX_5745m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5745m[i]);
		}
		freq = 5745;
	}
	break;
	case Freq_RX_5765m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5765m[i]);
		}
		freq = 5765;
	}
	break;
	case Freq_RX_5785m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5785m[i]);
		}
		freq = 5785;
	}
	break;
	case Freq_RX_5805m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5805m[i]);
		}
		freq = 5805;
	}
	break;
	case Freq_RX_5825m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5825m[i]);
		}
		freq = 5825;
	}
	break;

	case Freq_RX_5850m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5850m[i]);
		}
		freq = 5850;
	}
	break;
	case Freq_RX_5890m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5890m[i]);
		}
		freq = 5890;
	}
	break;
	case Freq_RX_5930m:
	{
		for (i = 0; i < cnt; i++)
		{
			SetAd(Ad9361FreqConfig_RX_5930m[i]);
		}
		freq = 5930;
	}
	break;
//	case Freq_RX_5865m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5865m[i]);
//		}
//		freq = 5865;
//	}
//	break;

/////////////////////////////////////////////////////////////////////////////////////////////////
//	case Freq_RX_1437m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_1437m[i]);
//		}
//		freq = 1437;
//	}
//	break;
//	case Freq_RX_4870m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_4870m[i]);
//		}
//		freq = 4870;
//	}
//	break;
//	case Freq_RX_4915m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_4915m[i]);
//		}
//		freq = 4915;
//	}
//	break;
//	case Freq_RX_4960m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_4960m[i]);
//		}
//		freq = 4960;
//	}
//	break;
//	case Freq_RX_5000m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5000m[i]);
//		}
//		freq = 5000;
//	}
//	break;
//	case Freq_RX_5040m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5040m[i]);
//		}
//		freq = 5040;
//	}
//	break;
//	case Freq_RX_5080m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5080m[i]);
//		}
//		freq = 5080;
//	}
//	break;
//	case Freq_RX_5120m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5120m[i]);
//		}
//		freq = 5120;
//	}
//	break;
//	case Freq_RX_5280m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5280m[i]);
//		}
//		freq = 5280;
//	}
//	break;
//	case Freq_RX_5320m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5320m[i]);
//		}
//		freq = 5320;
//	}
//	break;
//	case Freq_RX_5360m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5360m[i]);
//		}
//		freq = 5360;
//	}
//	break;
//	case Freq_RX_5400m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5400m[i]);
//		}
//		freq = 5400;
//	}
//	break;
//	case Freq_RX_5830m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5830m[i]);
//		}
//		freq = 5830;
//	}
//	break;
//	case Freq_RX_5445m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5445m[i]);
//		}
//		freq = 5445;
//	}
//	break;
//	case Freq_RX_5490m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5490m[i]);
//		}
//		freq = 5490;
//	}
//	break;
//	case Freq_RX_5535m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5535m[i]);
//		}
//		freq = 5535;
//	}
//	break;
//	case Freq_RX_5580m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5580m[i]);
//		}
//		freq = 5580;
//	}
//	break;
//	case Freq_RX_5625m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5625m[i]);
//		}
//		freq = 5625;
//	}
//	break;
//	case Freq_RX_5665m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5665m[i]);
//		}
//		freq = 5665;
//	}
//	break;
//	case Freq_RX_5705m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5705m[i]);
//		}
//		freq = 5705;
//	}
//	break;
//	case Freq_RX_5900m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5900m[i]);
//		}
//		freq = 5900;
//	}
//	break;
//	case Freq_RX_5860m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5860m[i]);
//		}
//		freq = 5860;
//	}
//	break;
//	case Freq_RX_5940m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5940m[i]);
//		}
//		freq = 5940;
//	}
//	break;
//	case Freq_RX_5980m:
//	{
//		for (i = 0; i < cnt; i++)
//		{
//			SetAd(Ad9361FreqConfig_RX_5980m[i]);
//		}
//		freq = 5980;
//	}
//	break;
	case Freq_MAX_CNT:
	default:
		break;
	}

	if (GetAd9361GainCutoverFlag() == 1)
	{
		GainCnt++;
		if (GainCnt >= Freq_MAX_CNT)
		{
			GainCnt = 0;
			GainCutoverIndex++;
			if (GainCutoverIndex > 1)
			{
				GainCutoverIndex = 0;
			}
//			SetAd9361Gain(Ad9361GainBuf[GainCutoverIndex]);
			SetAd9361Gain(Ad9361GainBuf[GainCutoverIndex], Ad9361GainBuf[GainCutoverIndex]);
			LOG_DEBUG("Gain:%02X \n\r", Ad9361GainBuf[GainCutoverIndex]);
		}
	}
	return freq;
}

/* Gets the RX frequency. */
uint32_t get_rx_freq(uint32_t freqItem)
{
	uint32_t freq = 0;
	switch (freqItem)
	{
	case Freq_RX_842m:
	{
		freq = 842;
	}break;
	case Freq_RX_915m:
	{
		freq = 915;
	}break;
	case Freq_RX_1080m:
	{
		freq = 1080;
	}break;
	case Freq_RX_1120m:
	{
		freq = 1120;
	}break;
	case Freq_RX_1160m:
	{
		freq = 1160;
	}break;
	case Freq_RX_1200m:
	{
		freq = 1200;
	}break;
	case Freq_RX_1240m:
	{
		freq = 1240;
	}break;
	case Freq_RX_1280m:
	{
		freq = 1280;
	}break;
	case Freq_RX_1320m:
	{
		freq = 1320;
	}break;
	case Freq_RX_1360m:
	{
		freq = 1360;
	}break;

	case Freq_RX_1437m:
	{
		freq = 1437;
	}break;
//	case Freq_RX_1775m:
//	{
//		freq = 1775;
//	}break;
//	case Freq_RX_2420m:
//	{
//		freq = 2420;
//	}
	break;
	case Freq_RX_2422m:
	{
		freq = 2422;
	}
	break;
	case Freq_RX_2437m:
	{
		freq = 2437;
	}
	break;
//	case Freq_RX_2460m:
//	{
//		freq = 2460;
//	}
	break;
	case Freq_RX_2462m:
	{
		freq = 2462;
	}
	break;
	case Freq_RX_5160m:
	{
		freq = 5160;
	}break;
	case Freq_RX_5180m:
	{
		freq = 5180;
	}break;
	case Freq_RX_5200m:
	{
		freq = 5200;
	}break;
	case Freq_RX_5220m:
	{
		freq = 5220;
	}break;
	case Freq_RX_5240m:
	{
		freq = 5240;
	}break;

	case Freq_RX_5655m:
	{
		freq = 5655;
	}
	break;
	case Freq_RX_5695m:
	{
		freq = 5695;
	}
	break;
	case Freq_RX_5725m:
	{
		freq = 5725;
	}
	break;

	case Freq_RX_5745m:
	{
		freq = 5745;
	}
	break;
	case Freq_RX_5765m:
	{
		freq = 5765;
	}
	break;
	case Freq_RX_5785m:
	{
		freq = 5785;
	}
	break;
	case Freq_RX_5805m:
	{
		freq = 5805;
	}
	break;
	case Freq_RX_5825m:
	{
		freq = 5825;
	}
	break;

	case Freq_RX_5850m:
	{
		freq = 5850;
	}
	break;
	case Freq_RX_5890m:
	{
		freq = 5890;
	}
	break;
	case Freq_RX_5930m:
	{
		freq = 5930;
	}
	break;
//	case Freq_RX_4870m:
//	{
//		freq = 4870;
//	}
//	break;
//	case Freq_RX_4915m:
//	{
//		freq = 4915;
//	}
//	break;
//	case Freq_RX_4960m:
//	{
//		freq = 4960;
//	}
//	break;
//	case Freq_RX_5000m:
//	{
//		freq = 5000;
//	}
//	break;
//	case Freq_RX_5040m:
//	{
//		freq = 5040;
//	}
//	break;
//	case Freq_RX_5080m:
//	{
//		freq = 5080;
//	}
//	break;
//	case Freq_RX_5120m:
//	{
//		freq = 5120;
//	}
//	break;
//	case Freq_RX_5280m:
//	{
//		freq = 5280;
//	}
//	break;
//	case Freq_RX_5320m:
//	{
//		freq = 5320;
//	}
//	break;
//	case Freq_RX_5360m:
//	{
//		freq = 5360;
//	}
//	break;
//	case Freq_RX_5400m:
//	{
//		freq = 5400;
//	}
//	break;
//	case Freq_RX_5830m:
//	{
//		freq = 5830;
//	}
//	break;
//	case Freq_RX_5445m:
//	{
//		freq = 5445;
//	}
//	break;
//	case Freq_RX_5490m:
//	{
//		freq = 5490;
//	}
//	break;
//	case Freq_RX_5535m:
//	{
//		freq = 5535;
//	}
//	break;
//	case Freq_RX_5580m:
//	{
//		freq = 5580;
//	}
//	break;
//	case Freq_RX_5625m:
//	{
//		freq = 5625;
//	}
//	break;
//	case Freq_RX_5665m:
//	{
//		freq = 5665;
//	}break;
//	case Freq_RX_5705m:
//	{
//		freq = 5705;
//	}
//	break;
//	case Freq_RX_5900m:
//	{
//		freq = 5900;
//	}
//	break;
//	case Freq_RX_5860m:
//	{
//		freq = 5860;
//	}
//	break;
//	case Freq_RX_5940m:
//	{
//		freq = 5940;
//	}
//	break;
//	case Freq_RX_5980m:
//	{
//		freq = 5980;
//	}
//	break;
	case Freq_MAX_CNT:
	default:
		break;
	}

	return freq;
}

void ad9361_generate_fast_lock_profile(uint8_t profile_num, uint8_t *reg_value_list)
{
	u16 row, i;
	u8 Ad9361Config[][3] = {
		{0x82, 0x9a, 0x03},
		{0x82, 0x9c, 0x00 + profile_num},
		{0x82, 0x9d, reg_value_list[1]},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x01 + profile_num},
		{0x82, 0x9d, reg_value_list[2]},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x02 + profile_num},
		{0x82, 0x9d, reg_value_list[3]},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x03 + profile_num}, // Synthesizer Fractional Word[15:8] 0x274[D7:D0]
		{0x82, 0x9d, reg_value_list[4]},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x04 + profile_num}, // Synthesizer Fractional Word[22:16] 0x275[D6:D0]
		{0x82, 0x9d, reg_value_list[5] & 0x7f},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x05 + profile_num}, // VCO Bias Ref[2:0] shift left by 4;  0x282[D2:D0]
		{0x82, 0x9d, ((reg_value_list[14] & 0x7) << 4) + (reg_value_list[9] & 0xf)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x06 + profile_num}, // VCO Varactor[3:0]; 0x279[D3:D0]    VCO Bias Tcf[1:0] shift left by 6;  0x282[D4:D3]
		{0x82, 0x9d, (((reg_value_list[14] & 0x18) >> 3) << 6) + (reg_value_list[10] & 0x3f)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x07 + profile_num}, // Charge Pump Current (Init)[5:0]; Set per Init ; Charge Pump Current[5:0]; 0x27B[D5:D0]
		{0x82, 0x9d, reg_value_list[10] & 0x3f},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x08 + profile_num}, // Loop Filter R3[3:0] shift left by 4; 0x280[D3:D0]
		{0x82, 0x9d, ((reg_value_list[13] & 0xf) << 4) + (reg_value_list[13] & 0xf)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x09 + profile_num}, // Loop Filter R3 (Init)[3:0]; Set per Init; Loop Filter C3[3:0] shift left by 4; 0x27F[D3:D0]
		{0x82, 0x9d, ((reg_value_list[12] & 0xf) << 4) + (reg_value_list[12] & 0xf)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0A + profile_num}, // Loop Filter C3 (Init)[3:0] ;Set per Init; Loop Filter C1[3:0] shift left by 4; 0x27E[D3:D0]
		{0x82, 0x9d, ((reg_value_list[11] & 0xf) << 4) + (reg_value_list[11] >> 4)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0B + profile_num}, // Loop Filter C2[3:0] shift right by 4;0x27E[D7:D4]); Loop Filter R1[3:0]; 0x27F[D7:D4]
		{0x82, 0x9d, (reg_value_list[12] & 0xf0) + (reg_value_list[12] >> 4)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0C + profile_num}, // Loop Filter R1 (Init)[3:0]; Set per Init N calculation;VCO Varactor Reference Tcf[2:0]; 0x290[D6:D4]
		{0x82, 0x9d, (reg_value_list[15] & 0x70) + (reg_value_list[0] >> 4)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0D + profile_num}, // Rx VCO Divider[3:0]; 0x005[D7:D4];  VCO Cal Offset[3:0] shift left by 1;0x278[D6:D3]
		{0x82, 0x9d, ((reg_value_list[8] & 0x78) << 1) + (reg_value_list[16] & 0xf)},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0E + profile_num}, // VCO Varactor Reference[3:0]; 0x291[D3:D0]; Force VCO Tune[7:0]; 0x277[D7:D0]
		{0x82, 0x9d, reg_value_list[7]},
		{0x82, 0x9f, 0x03},
		{0x82, 0x9c, 0x0F + profile_num}, // Force ALC word[6:0] shift left by 1; 0x276[D6:D0] Force VCO Tune[8]; 0x278[D0]
		{0x82, 0x9d, ((reg_value_list[6] & 0x7f) << 1) + (reg_value_list[8] & 0x1)},
		{0x82, 0x9f, 0x03},
	};
	row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
	for (i = 0; i < row; i++)
	{
		SetAd(Ad9361Config[i]);
	}
}

void enable_fast_lock_profile()
{
	u16 row, i;
	u8 Ad9361Config[][3] = {
		{0x82, 0x9B, 0x00},
		{0x82, 0x9a, 0x03},
		{0x82, 0x9f, 0x00},
		{0x80, 0x15, 0x0F},
		{0x82, 0x70, 0x55},
	};
	row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
	for (i = 0; i < row; i++)
	{
		SetAd(Ad9361Config[i]);
	}
}

#define HOP_BY_GPIO

static void fre_hop_handle(void *param)
{}

int32_t Ad9361_fre_hop_init(void)
{
	int status;

	//	uint8_t Ad9361Config[3] = {0x00, 0x05, 0xff};
	//	for(uint8_t i=0;i<17;i++){
	//		Ad9361Config[0] = (TX_read_reg_addr_list[i] & 0x0F00) >> 8;
	//		Ad9361Config[1] = (TX_read_reg_addr_list[i] & 0x00FF);
	//		SetAd(Ad9361Config);
	//	}

	ad9361_cfg_tx_init(); // debug
//	XGpio_Initialize(&sGpio, XPAR_AXI_GPIO_0_DEVICE_ID);
//	status = XScuGic_Connect(&xInterruptController,
//							 AD9361_FRE_HOP_TIME,
//							 (Xil_ExceptionHandler)fre_hop_handle,
//							 NULL);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	IntcTypeSetup(&xInterruptController, AD9361_FRE_HOP_TIME, 0x03);

	//	Ad9361_fre_hop_start();

	return 0;
}

int32_t Ad9361_fre_hop_start(void)
{
	XScuGic_Enable(&xInterruptController, AD9361_FRE_HOP_TIME);

	return 0;
}

int32_t Ad9361_fre_hop_stop(void)
{
	XScuGic_Disable(&xInterruptController, AD9361_FRE_HOP_TIME);

	return 0;
}

int32_t Ad9361_fre_hop_set(int32_t rfRang)
{
	ad9361_generate_fast_lock_profile(fre_433m_addr << 4, reg_value_list_433m);
	ad9361_generate_fast_lock_profile(fre_842m_addr << 4, reg_value_list_842m);
	ad9361_generate_fast_lock_profile(fre_915m_addr << 4, reg_value_list_915m);
	//	ad9361_generate_fast_lock_profile(fre_1931m_addr << 4, reg_value_list_1931m);
	//	ad9361_generate_fast_lock_profile(fre_1433m_addr << 4, reg_value_list_1433m);
	//	ad9361_generate_fast_lock_profile(fre_1187m_addr << 4, reg_value_list_1187m);
	//	ad9361_generate_fast_lock_profile(fre_1212m_addr << 4, reg_value_list_1212m);
	//	ad9361_generate_fast_lock_profile(fre_1253m_addr << 4, reg_value_list_1253m);
	//	ad9361_generate_fast_lock_profile(fre_1265m_addr << 4, reg_value_list_1265m);
	ad9361_generate_fast_lock_profile(fre_1572m_addr << 4, reg_value_list_1572m);
	ad9361_generate_fast_lock_profile(fre_1597m_addr << 4, reg_value_list_1597m);
	enable_fast_lock_profile();
	uGps915mHitFlag = rfRang;

	return 0;
}

void SetAD9361Gpio(uint32_t value)
{
//	GPIO_OutputCtrl(EMIO_CTRL_IN0, ((value&0x01) != 0));
	GPIO_OutputCtrl(EMIO_CTRL_IN1, ((value&0x01) != 0));
	GPIO_OutputCtrl(EMIO_CTRL_IN2, ((value&0x02) != 0));
	GPIO_OutputCtrl(EMIO_CTRL_IN3, ((value&0x04) != 0));
}

void SetAd9361Gain(uint8_t omnidirect_value, uint8_t orientation_value)
{
	u8 Ad9361Config[3] = {0x81, 0x09, 0x00};
	u8 Ad9361Config2[3] = {0x81, 0x0C, 0x00};
	Ad9361Config[2] = orientation_value;
	Ad9361Config2[2] = omnidirect_value;
	SetAd(Ad9361Config);
	SetAd(Ad9361Config2);
}

static uint8_t Ad9361GainCutoverFlag = 0;
void SetAd9361GainCutoverFlag(uint8_t value)
{
	Ad9361GainCutoverFlag = value;
}
uint8_t GetAd9361GainCutoverFlag(void)
{
	return Ad9361GainCutoverFlag;
}
void SetAd9361GainCutoverValue(uint8_t value1, uint8_t value2)
{
	Ad9361GainBuf[0] = value1;
	Ad9361GainBuf[1] = value2;
}

void SetAd9361GainAGC(uint8_t value)
{
	u16 row, i;
	static u8 Ad9361Config[][3] = {
		//************************************************************
		// Setup Rx AGC Fast AttackRegisters
		//************************************************************
		{0x80, 0x22, 0x0A}, // SPIWrite	022,0A	// AGC Fast Attack Gain Lock Delay
		{0x80, 0xFA, 0xE5}, // SPIWrite	0FA,E5	// Gain Control Mode Select
		{0x80, 0xFB, 0x08}, // SPIWrite	0FB,08	// Gain Control Config
		{0x80, 0xFC, 0x23}, // SPIWrite	0FC,23	// ADC Overrange Sample Size
		{0x80, 0xFD, 0x4C}, // SPIWrite	0FD,4C	// Max Full/LMT Gain Table Index
		{0x80, 0xFE, 0x44}, // SPIWrite	0FE,44	// Peak Overload Wait Time
		{0x81, 0x00, 0x6F}, // SPIWrite	100,6F	// Dig Gain: Step Size & Max
		{0x81, 0x01, 0x0A}, // SPIWrite	101,0A	// AGC Lock Level
		{0x81, 0x03, 0x08}, // SPIWrite	103,08	// Large LMT or Step 3 Size
		{0x81, 0x04, 0x2F}, // SPIWrite	104,2F	// ADC Small Overload Threshold
		{0x81, 0x05, 0x3A}, // SPIWrite	105,3A	// ADC Large Overload Threshold
		{0x81, 0x06, 0x22}, // SPIWrite	106,22	// Overload Step Sizes
		{0x81, 0x07, 0x2B}, // SPIWrite	107,2B	// Small LMT Overload Threshold
		{0x81, 0x08, 0x31}, // SPIWrite	108,31	// Large LMT Overload Threshold
		{0x81, 0x09, 0x4C}, // SPIWrite	109,4C	// State 5 Power Measurement MSB
		{0x81, 0x0A, 0x58}, // SPIWrite	10A,58	// State 5 Power Measurement LSBs
		{0x81, 0x0B, 0x00}, // SPIWrite	10B,00	// Rx1 Force Digital Gain
		{0x81, 0x0E, 0x00}, // SPIWrite	10E,00	// Rx2 Force Digital Gain
		{0x81, 0x10, 0x00}, // SPIWrite	110,00	// AGC Fast Attack Config
		{0x81, 0x11, 0x0A}, // SPIWrite	111,0A	// Settling Delay & AGC Config
		{0x81, 0x12, 0x52}, // SPIWrite	112,52	// Post Lock Step & Energy Lost Thresh
		{0x81, 0x13, 0x4C}, // SPIWrite	113,4C	// Post Lock Step & Strong Sig Thresh
		{0x81, 0x14, 0x30}, // SPIWrite	114,30	// Low Power Threshold & ADC Ovr Ld
		{0x81, 0x15, 0x00}, // SPIWrite	115,00	// Stronger Signal Unlock Control
		{0x81, 0x16, 0x65}, // SPIWrite	116,65	// Final Overrange and Opt Gain Offset
		{0x81, 0x17, 0x08}, // SPIWrite	117,08	// Gain Inc Step & Energy Detect Cnt
		{0x81, 0x18, 0x05}, // SPIWrite	118,05	// Lock Level GAin Incr Upper Limit
		{0x81, 0x19, 0x08}, // SPIWrite	119,08	// Gain Lock Exit Count
		{0x81, 0x1A, 0x27}, // SPIWrite	11A,27	// Initial LMT Gain Limit
		{0x81, 0x1B, 0x0A}, // SPIWrite	11B,0A	// Increment Time
	};
	static u8 Ad9361Config1[][3] = {
		//************************************************************
		// Setup Rx Manual Gain Registers
		//************************************************************
		{0x80, 0xFA, 0xE0}, // SPIWrite	0FA,E0	// Gain Control Mode Select
		{0x80, 0xFB, 0x08}, // SPIWrite	0FB,08	// Table, Digital Gain, Man Gain Ctrl
		{0x80, 0xFC, 0x23}, // SPIWrite	0FC,23	// Incr Step Size, ADC Overrange Size
		{0x80, 0xFD, 0x4C}, // SPIWrite	0FD,4C	// Max Full/LMT Gain Table Index
		{0x80, 0xFE, 0x44}, // SPIWrite	0FE,44	// Decr Step Size, Peak Overload Time
		{0x81, 0x00, 0x6F}, // SPIWrite	100,6F	// Max Digital Gain
		{0x81, 0x04, 0x2F}, // SPIWrite	104,2F	// ADC Small Overload Threshold
		{0x81, 0x05, 0x3A}, // SPIWrite	105,3A	// ADC Large Overload Threshold
		{0x81, 0x07, 0x2B}, // SPIWrite	107,2B	// Small LMT Overload Threshold
		{0x81, 0x08, 0x31}, // SPIWrite	108,31	// Large LMT Overload Threshold
		{0x81, 0x09, 0x3C}, // SPIWrite	109,0A	// Rx1 Full/LMT Gain Index
		{0x81, 0x0A, 0x58}, // SPIWrite	10A,58	// Rx1 LPF Gain Index
		{0x81, 0x0B, 0x00}, // SPIWrite	10B,00	// Rx1 Digital Gain Index
		{0x81, 0x0C, 0x3C}, // SPIWrite	10C,0A	// Rx2 Full/LMT Gain Index
		{0x81, 0x0D, 0x18}, // SPIWrite	10D,18	// Rx2 LPF Gain Index
		{0x81, 0x0E, 0x00}, // SPIWrite	10E,00	// Rx2 Digital Gain Index
		{0x81, 0x14, 0x30}, // SPIWrite	114,30	// Low Power Threshold
		{0x81, 0x1A, 0x27}, // SPIWrite	11A,27	// Initial LMT Gain Limit
		{0x80, 0x81, 0x00}, // SPIWrite	081,00	// Tx Symbol Gain Control
	};
	if (value == 1)
	{
		row = sizeof(Ad9361Config) / sizeof(Ad9361Config[0]);
		for (i = 0; i < row; i++)
		{
			SetAd(Ad9361Config[i]);
		}
	}
	else if (value == 0)
	{
		row = sizeof(Ad9361Config1) / sizeof(Ad9361Config1[0]);
		for (i = 0; i < row; i++)
		{
			SetAd(Ad9361Config1[i]);
		}
	}
	else if (value == 2)
	{
		uint8_t dat = ad9361_spi_read_byte(0x10C);
		LOG_DEBUG("Gain: %d\r\n", dat);
	}
}

void SetAd9361Reg(uint8_t addr, uint8_t addr2, uint8_t value)
{
	u8 Ad9361Config[3] = {0};
	Ad9361Config[0] = addr;
	Ad9361Config[1] = addr2;
	Ad9361Config[2] = value;

	SetAd(Ad9361Config);
}
