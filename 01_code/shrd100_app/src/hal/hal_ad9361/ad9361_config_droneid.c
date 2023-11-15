
#include "ad9361_config.h"
#include "zynq_bram.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xparameters.h"

#include "../output/output.h"


static void Ad9361Config_01(void)
{
	u16 row,i;
	u8 Ad9361Config[][3] = {
		{0x80,0x00,0x81},
		{0x80,0x00,0x00},
		//************************************************************
		// AD9361 R2 Auto Generated Initialization Script:  This script was
		// generated using the AD9361 Customer software Version 2.1.4
		//************************************************************
		// Profile: Custom
		// REFCLK_IN: 40.000 MHz
		
		//RESET_FPGA	
		//RESET_DUT	
		
		//BlockWrite	2,6	// Set ADI FPGA SPI to 20Mhz
		{0x83,0xDF,0x01},		//SPIWrite	3DF,01	// Required for proper operation
		//ReadPartNumber
		{0x82,0xA6,0x0E},		//SPIWrite	2A6,0E	// Enable Master Bias
		{0x82,0xA8,0x0E},		//SPIWrite	2A8,0E	// Set Bandgap Trim
		//REFCLK_Scale	40.000000,1,2	// Sets local variables in script engine, user can ignore
		{0x82,0xAB,0x07},		//SPIWrite	2AB,07	// Set RF PLL reflclk scale to REFCLK * 2
		{0x82,0xAC,0xFF},		//SPIWrite	2AC,FF	// Set RF PLL reflclk scale to REFCLK * 2
		{0x80,0x09,0x17},		//SPIWrite	009,17	// Enable Clocks
		{0xF0,0x00,0x14},		//WAIT	20	// waits 20 ms
		//************************************************************
		// Set BBPLL Frequency: 737.280000
		//************************************************************
		{0x80,0x45,0x00},		//SPIWrite	045,00	// Set BBPLL reflclk scale to REFCLK /1
		{0x80,0x46,0x02},		//SPIWrite	046,02	// Set BBPLL Loop Filter Charge Pump current
		{0x80,0x48,0xE8},		//SPIWrite	048,E8	// Set BBPLL Loop Filter C1, R1
		{0x80,0x49,0x5B},		//SPIWrite	049,5B	// Set BBPLL Loop Filter R2, C2, C1
		{0x80,0x4A,0x35},		//SPIWrite	04A,35	// Set BBPLL Loop Filter C3,R2
		{0x80,0x4B,0xE0},		//SPIWrite	04B,E0	// Allow calibration to occur and set cal count to 1024 for max accuracy
		{0x80,0x4E,0x10},		//SPIWrite	04E,10	// Set calibration clock to REFCLK/4 for more accuracy
		{0x80,0x43,0x1F},		//SPIWrite	043,1F	// BBPLL Freq Word (Fractional[7:0])
		{0x80,0x42,0xC5},		//SPIWrite	042,C5	// BBPLL Freq Word (Fractional[15:8])
		{0x80,0x41,0x0D},		//SPIWrite	041,0D	// BBPLL Freq Word (Fractional[23:16])
		{0x80,0x44,0x12},		//SPIWrite	044,12	// BBPLL Freq Word (Integer[7:0])
		{0x80,0x3F,0x05},		//SPIWrite	03F,05	// Start BBPLL Calibration
		{0x80,0x3F,0x01},		//SPIWrite	03F,01	// Clear BBPLL start calibration bit
		{0x80,0x4C,0x86},		//SPIWrite	04C,86	// Increase BBPLL KV and phase margin
		{0x80,0x4D,0x01},		//SPIWrite	04D,01	// Increase BBPLL KV and phase margin
		{0x80,0x4D,0x05},		//SPIWrite	04D,05	// Increase BBPLL KV and phase margin
		{0x00,0x5E,0x71},		//WAIT_CALDONE	BBPLL,2000	// Wait for BBPLL to lock, Timeout 2sec, Max BBPLL VCO Cal Time: 345.600 us (Done when 0x05E[7]==1)
		
		
		{0x80,0x02,0xD9},		//SPIWrite	002,D9	// Setup Tx Digital Filters/ Channels
		{0x80,0x03,0xDD},		//SPIWrite	003,DD	// Setup Rx Digital Filters/ Channels
		{0x80,0x04,0x03},		//SPIWrite	004,03	// Select Rx input pin(A,B,C)/ Tx out pin (A,B)
		{0x80,0x0A,0x09},		//SPIWrite	00A,09	// Set BBPLL post divide rate
		
		//************************************************************
		// Program Tx FIR: D:\Program Files (x86)\Analog Devices\AD9361
		// Evaluation Software v2.1.5 (64-bit)\DigitalFilters\46.08-16ph.ftr
		//************************************************************
		{0x80,0x65,0x5A},		//SPIWrite	065,5A	// Enable clock to Tx FIR Filter and set Filter gain Setting
		{0x80,0x60,0x00},		//SPIWrite	060,00	// Write FIR coefficient address
		{0x80,0x61,0x57},		//SPIWrite	061,57	// Write FIR coefficient data[7:0]
		{0x80,0x62,0x00},		//SPIWrite	062,00	// Write FIR coefficient data[15:8]
		{0x80,0x65,0x5E},		//SPIWrite	065,5E	// Set Write EN to push data into FIR filter register map
		{0x80,0x64,0x00},		//SPIWrite	064,00	// Write to Read only register to delay ~1us
		{0x80,0x64,0x00},		//SPIWrite	064,00	// Write to Read only register to delay ~1us
		{0x80,0x60,0x01},		//SPIWrite	060,01
		{0x80,0x61,0xB0},		//SPIWrite	061,B0
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x02},		//SPIWrite	060,02
		{0x80,0x61,0xC0},		//SPIWrite	061,C0
		{0x80,0x62,0xFF},		//SPIWrite	062,FF
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x03},		//SPIWrite	060,03
		{0x80,0x61,0xBA},		//SPIWrite	061,BA
		{0x80,0x62,0xFF},		//SPIWrite	062,FF
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x04},		//SPIWrite	060,04
		{0x80,0x61,0xB3},		//SPIWrite	061,B3
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x05},		//SPIWrite	060,05
		{0x80,0x61,0x4F},		//SPIWrite	061,4F
		{0x80,0x62,0xFF},		//SPIWrite	062,FF
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x06},		//SPIWrite	060,06
		{0x80,0x61,0x15},		//SPIWrite	061,15
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x07},		//SPIWrite	060,07
		{0x80,0x61,0xE1},		//SPIWrite	061,E1
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x08},		//SPIWrite	060,08
		{0x80,0x61,0x78},		//SPIWrite	061,78
		{0x80,0x62,0xFE},		//SPIWrite	062,FE
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x09},		//SPIWrite	060,09
		{0x80,0x61,0x31},		//SPIWrite	061,31
		{0x80,0x62,0x01},		//SPIWrite	062,01
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x0A},		//SPIWrite	060,0A
		{0x80,0x61,0x46},		//SPIWrite	061,46
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x0B},		//SPIWrite	060,0B
		{0x80,0x61,0xDB},		//SPIWrite	061,DB
		{0x80,0x62,0xFD},		//SPIWrite	062,FD
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x0C},		//SPIWrite	060,0C
		{0x80,0x61,0x16},		//SPIWrite	061,16
		{0x80,0x62,0x03},		//SPIWrite	062,03
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x0D},		//SPIWrite	060,0D
		{0x80,0x61,0x0A},		//SPIWrite	061,0A
		{0x80,0x62,0xFE},		//SPIWrite	062,FE
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x0E},		//SPIWrite	060,0E
		{0x80,0x61,0xD4},		//SPIWrite	061,D4
		{0x80,0x62,0xFE},		//SPIWrite	062,FE
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x0F},		//SPIWrite	060,0F
		{0x80,0x61,0xB1},		//SPIWrite	061,B1
		{0x80,0x62,0x04},		//SPIWrite	062,04
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x10},		//SPIWrite	060,10
		{0x80,0x61,0xF9},		//SPIWrite	061,F9
		{0x80,0x62,0xF9},		//SPIWrite	062,F9
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x11},		//SPIWrite	060,11
		{0x80,0x61,0x2F},		//SPIWrite	061,2F
		{0x80,0x62,0x03},		//SPIWrite	062,03
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x12},		//SPIWrite	060,12
		{0x80,0x61,0x9E},		//SPIWrite	061,9E
		{0x80,0x62,0x03},		//SPIWrite	062,03
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x13},		//SPIWrite	060,13
		{0x80,0x61,0xE2},		//SPIWrite	061,E2
		{0x80,0x62,0xF4},		//SPIWrite	062,F4
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x14},		//SPIWrite	060,14
		{0x80,0x61,0xB9},		//SPIWrite	061,B9
		{0x80,0x62,0x0D},		//SPIWrite	062,0D
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x15},		//SPIWrite	060,15
		{0x80,0x61,0xB9},		//SPIWrite	061,B9
		{0x80,0x62,0xFA},		//SPIWrite	062,FA
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x16},		//SPIWrite	060,16
		{0x80,0x61,0x84},		//SPIWrite	061,84
		{0x80,0x62,0xEE},		//SPIWrite	062,EE
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x17},		//SPIWrite	060,17
		{0x80,0x61,0x80},		//SPIWrite	061,80
		{0x80,0x62,0x4F},		//SPIWrite	062,4F
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x18},		//SPIWrite	060,18
		{0x80,0x61,0x80},		//SPIWrite	061,80
		{0x80,0x62,0x4F},		//SPIWrite	062,4F
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x19},		//SPIWrite	060,19
		{0x80,0x61,0x84},		//SPIWrite	061,84
		{0x80,0x62,0xEE},		//SPIWrite	062,EE
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x1A},		//SPIWrite	060,1A
		{0x80,0x61,0xB9},		//SPIWrite	061,B9
		{0x80,0x62,0xFA},		//SPIWrite	062,FA
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x1B},		//SPIWrite	060,1B
		{0x80,0x61,0xB9},		//SPIWrite	061,B9
		{0x80,0x62,0x0D},		//SPIWrite	062,0D
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x1C},		//SPIWrite	060,1C
		{0x80,0x61,0xE2},		//SPIWrite	061,E2
		{0x80,0x62,0xF4},		//SPIWrite	062,F4
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x1D},		//SPIWrite	060,1D
		{0x80,0x61,0x9E},		//SPIWrite	061,9E
		{0x80,0x62,0x03},		//SPIWrite	062,03
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x1E},		//SPIWrite	060,1E
		{0x80,0x61,0x2F},		//SPIWrite	061,2F
		{0x80,0x62,0x03},		//SPIWrite	062,03
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x1F},		//SPIWrite	060,1F
		{0x80,0x61,0xF9},		//SPIWrite	061,F9
		{0x80,0x62,0xF9},		//SPIWrite	062,F9
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x20},		//SPIWrite	060,20
		{0x80,0x61,0xB1},		//SPIWrite	061,B1
		{0x80,0x62,0x04},		//SPIWrite	062,04
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x21},		//SPIWrite	060,21
		{0x80,0x61,0xD4},		//SPIWrite	061,D4
		{0x80,0x62,0xFE},		//SPIWrite	062,FE
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x22},		//SPIWrite	060,22
		{0x80,0x61,0x0A},		//SPIWrite	061,0A
		{0x80,0x62,0xFE},		//SPIWrite	062,FE
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x23},		//SPIWrite	060,23
		{0x80,0x61,0x16},		//SPIWrite	061,16
		{0x80,0x62,0x03},		//SPIWrite	062,03
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x24},		//SPIWrite	060,24
		{0x80,0x61,0xDB},		//SPIWrite	061,DB
		{0x80,0x62,0xFD},		//SPIWrite	062,FD
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x25},		//SPIWrite	060,25
		{0x80,0x61,0x46},		//SPIWrite	061,46
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x26},		//SPIWrite	060,26
		{0x80,0x61,0x31},		//SPIWrite	061,31
		{0x80,0x62,0x01},		//SPIWrite	062,01
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x27},		//SPIWrite	060,27
		{0x80,0x61,0x78},		//SPIWrite	061,78
		{0x80,0x62,0xFE},		//SPIWrite	062,FE
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x28},		//SPIWrite	060,28
		{0x80,0x61,0xE1},		//SPIWrite	061,E1
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x29},		//SPIWrite	060,29
		{0x80,0x61,0x15},		//SPIWrite	061,15
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x2A},		//SPIWrite	060,2A
		{0x80,0x61,0x4F},		//SPIWrite	061,4F
		{0x80,0x62,0xFF},		//SPIWrite	062,FF
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x2B},		//SPIWrite	060,2B
		{0x80,0x61,0xB3},		//SPIWrite	061,B3
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x2C},		//SPIWrite	060,2C
		{0x80,0x61,0xBA},		//SPIWrite	061,BA
		{0x80,0x62,0xFF},		//SPIWrite	062,FF
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x2D},		//SPIWrite	060,2D
		{0x80,0x61,0xC0},		//SPIWrite	061,C0
		{0x80,0x62,0xFF},		//SPIWrite	062,FF
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x2E},		//SPIWrite	060,2E
		{0x80,0x61,0xB0},		//SPIWrite	061,B0
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x60,0x2F},		//SPIWrite	060,2F
		{0x80,0x61,0x57},		//SPIWrite	061,57
		{0x80,0x62,0x00},		//SPIWrite	062,00
		{0x80,0x65,0x5E},		//SPIWrite	065,5E
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x64,0x00},		//SPIWrite	064,00
		{0x80,0x65,0x58},		//SPIWrite	065,58	// Disable clock to Tx Filter
		
		//************************************************************
		// Program Rx FIR: D:\Program Files (x86)\Analog Devices\AD9361
		// Evaluation Software v2.1.5 (64-bit)\DigitalFilters\46.08-16ph.ftr
		//************************************************************
		{0x80,0xF5,0x5A},		//SPIWrite	0F5,5A	// Enable clock to Rx FIR Filter
		{0x80,0xF6,0x02},		//SPIWrite	0F6,02	// Write Filter Gain setting
		{0x80,0xF0,0x00},		//SPIWrite	0F0,00	// Write FIR coefficient address
		{0x80,0xF1,0x50},		//SPIWrite	0F1,50	// Write FIR coefficient data[7:0]
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00	// Write FIR coefficient data[15:8]
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E	// Set Write EN to push data into FIR filter register map
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00	// Dummy Write to Read only register to delay ~1us
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00	// Dummy Write to Read only register to delay ~1us
		{0x80,0xF0,0x01},		//SPIWrite	0F0,01
		{0x80,0xF1,0xA0},		//SPIWrite	0F1,A0
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x02},		//SPIWrite	0F0,02
		{0x80,0xF1,0xCF},		//SPIWrite	0F1,CF
		{0x80,0xF2,0xFF},		//SPIWrite	0F2,FF
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x03},		//SPIWrite	0F0,03
		{0x80,0xF1,0xC3},		//SPIWrite	0F1,C3
		{0x80,0xF2,0xFF},		//SPIWrite	0F2,FF
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x04},		//SPIWrite	0F0,04
		{0x80,0xF1,0x93},		//SPIWrite	0F1,93
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x05},		//SPIWrite	0F0,05
		{0x80,0xF1,0x6E},		//SPIWrite	0F1,6E
		{0x80,0xF2,0xFF},		//SPIWrite	0F2,FF
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x06},		//SPIWrite	0F0,06
		{0x80,0xF1,0x14},		//SPIWrite	0F1,14
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x07},		//SPIWrite	0F0,07
		{0x80,0xF1,0xB6},		//SPIWrite	0F1,B6
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x08},		//SPIWrite	0F0,08
		{0x80,0xF1,0xBE},		//SPIWrite	0F1,BE
		{0x80,0xF2,0xFE},		//SPIWrite	0F2,FE
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x09},		//SPIWrite	0F0,09
		{0x80,0xF1,0xFE},		//SPIWrite	0F1,FE
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x0A},		//SPIWrite	0F0,0A
		{0x80,0xF1,0x33},		//SPIWrite	0F1,33
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x0B},		//SPIWrite	0F0,0B
		{0x80,0xF1,0x44},		//SPIWrite	0F1,44
		{0x80,0xF2,0xFE},		//SPIWrite	0F2,FE
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x0C},		//SPIWrite	0F0,0C
		{0x80,0xF1,0x86},		//SPIWrite	0F1,86
		{0x80,0xF2,0x02},		//SPIWrite	0F2,02
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x0D},		//SPIWrite	0F0,0D
		{0x80,0xF1,0x5B},		//SPIWrite	0F1,5B
		{0x80,0xF2,0xFE},		//SPIWrite	0F2,FE
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x0E},		//SPIWrite	0F0,0E
		{0x80,0xF1,0x1D},		//SPIWrite	0F1,1D
		{0x80,0xF2,0xFF},		//SPIWrite	0F2,FF
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x0F},		//SPIWrite	0F0,0F
		{0x80,0xF1,0xC6},		//SPIWrite	0F1,C6
		{0x80,0xF2,0x03},		//SPIWrite	0F2,03
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x10},		//SPIWrite	0F0,10
		{0x80,0xF1,0x10},		//SPIWrite	0F1,10
		{0x80,0xF2,0xFB},		//SPIWrite	0F2,FB
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x11},		//SPIWrite	0F0,11
		{0x80,0xF1,0xBC},		//SPIWrite	0F1,BC
		{0x80,0xF2,0x02},		//SPIWrite	0F2,02
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x12},		//SPIWrite	0F0,12
		{0x80,0xF1,0xB3},		//SPIWrite	0F1,B3
		{0x80,0xF2,0x02},		//SPIWrite	0F2,02
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x13},		//SPIWrite	0F0,13
		{0x80,0xF1,0x35},		//SPIWrite	0F1,35
		{0x80,0xF2,0xF7},		//SPIWrite	0F2,F7
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x14},		//SPIWrite	0F0,14
		{0x80,0xF1,0x6F},		//SPIWrite	0F1,6F
		{0x80,0xF2,0x0B},		//SPIWrite	0F2,0B
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x15},		//SPIWrite	0F0,15
		{0x80,0xF1,0xE1},		//SPIWrite	0F1,E1
		{0x80,0xF2,0xF9},		//SPIWrite	0F2,F9
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x16},		//SPIWrite	0F0,16
		{0x80,0xF1,0x9E},		//SPIWrite	0F1,9E
		{0x80,0xF2,0xF3},		//SPIWrite	0F2,F3
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x17},		//SPIWrite	0F0,17
		{0x80,0xF1,0x1A},		//SPIWrite	0F1,1A
		{0x80,0xF2,0x4C},		//SPIWrite	0F2,4C
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x18},		//SPIWrite	0F0,18
		{0x80,0xF1,0x1A},		//SPIWrite	0F1,1A
		{0x80,0xF2,0x4C},		//SPIWrite	0F2,4C
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x19},		//SPIWrite	0F0,19
		{0x80,0xF1,0x9E},		//SPIWrite	0F1,9E
	};
	row = sizeof(Ad9361Config)/sizeof(Ad9361Config[0]);
	for(i=0;i<row;i++){
		SetAd(Ad9361Config[i]);
	}
}

static void Ad9361Config_02(void)
{
	u16 row,i;
	u8 Ad9361Config[][3] = {
		{0x80,0xF2,0xF3},		//SPIWrite	0F2,F3
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x1A},		//SPIWrite	0F0,1A
		{0x80,0xF1,0xE1},		//SPIWrite	0F1,E1
		{0x80,0xF2,0xF9},		//SPIWrite	0F2,F9
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x1B},		//SPIWrite	0F0,1B
		{0x80,0xF1,0x6F},		//SPIWrite	0F1,6F
		{0x80,0xF2,0x0B},		//SPIWrite	0F2,0B
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x1C},		//SPIWrite	0F0,1C
		{0x80,0xF1,0x35},		//SPIWrite	0F1,35
		{0x80,0xF2,0xF7},		//SPIWrite	0F2,F7
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x1D},		//SPIWrite	0F0,1D
		{0x80,0xF1,0xB3},		//SPIWrite	0F1,B3
		{0x80,0xF2,0x02},		//SPIWrite	0F2,02
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x1E},		//SPIWrite	0F0,1E
		{0x80,0xF1,0xBC},		//SPIWrite	0F1,BC
		{0x80,0xF2,0x02},		//SPIWrite	0F2,02
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x1F},		//SPIWrite	0F0,1F
		{0x80,0xF1,0x10},		//SPIWrite	0F1,10
		{0x80,0xF2,0xFB},		//SPIWrite	0F2,FB
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x20},		//SPIWrite	0F0,20
		{0x80,0xF1,0xC6},		//SPIWrite	0F1,C6
		{0x80,0xF2,0x03},		//SPIWrite	0F2,03
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x21},		//SPIWrite	0F0,21
		{0x80,0xF1,0x1D},		//SPIWrite	0F1,1D
		{0x80,0xF2,0xFF},		//SPIWrite	0F2,FF
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x22},		//SPIWrite	0F0,22
		{0x80,0xF1,0x5B},		//SPIWrite	0F1,5B
		{0x80,0xF2,0xFE},		//SPIWrite	0F2,FE
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x23},		//SPIWrite	0F0,23
		{0x80,0xF1,0x86},		//SPIWrite	0F1,86
		{0x80,0xF2,0x02},		//SPIWrite	0F2,02
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x24},		//SPIWrite	0F0,24
		{0x80,0xF1,0x44},		//SPIWrite	0F1,44
		{0x80,0xF2,0xFE},		//SPIWrite	0F2,FE
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x25},		//SPIWrite	0F0,25
		{0x80,0xF1,0x33},		//SPIWrite	0F1,33
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x26},		//SPIWrite	0F0,26
		{0x80,0xF1,0xFE},		//SPIWrite	0F1,FE
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x27},		//SPIWrite	0F0,27
		{0x80,0xF1,0xBE},		//SPIWrite	0F1,BE
		{0x80,0xF2,0xFE},		//SPIWrite	0F2,FE
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x28},		//SPIWrite	0F0,28
		{0x80,0xF1,0xB6},		//SPIWrite	0F1,B6
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x29},		//SPIWrite	0F0,29
		{0x80,0xF1,0x14},		//SPIWrite	0F1,14
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x2A},		//SPIWrite	0F0,2A
		{0x80,0xF1,0x6E},		//SPIWrite	0F1,6E
		{0x80,0xF2,0xFF},		//SPIWrite	0F2,FF
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x2B},		//SPIWrite	0F0,2B
		{0x80,0xF1,0x93},		//SPIWrite	0F1,93
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x2C},		//SPIWrite	0F0,2C
		{0x80,0xF1,0xC3},		//SPIWrite	0F1,C3
		{0x80,0xF2,0xFF},		//SPIWrite	0F2,FF
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x2D},		//SPIWrite	0F0,2D
		{0x80,0xF1,0xCF},		//SPIWrite	0F1,CF
		{0x80,0xF2,0xFF},		//SPIWrite	0F2,FF
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x2E},		//SPIWrite	0F0,2E
		{0x80,0xF1,0xA0},		//SPIWrite	0F1,A0
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF0,0x2F},		//SPIWrite	0F0,2F
		{0x80,0xF1,0x50},		//SPIWrite	0F1,50
		{0x80,0xF2,0x00},		//SPIWrite	0F2,00
		{0x80,0xF5,0x5E},		//SPIWrite	0F5,5E
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF4,0x00},		//SPIWrite	0F4,00
		{0x80,0xF5,0x58},		//SPIWrite	0F5,58	// Disable clock to Rx Filter
		
		//************************************************************
		// Setup the Parallel Port (Digital Data Interface)
		//************************************************************
		{0x80,0x10,0xC8},		//SPIWrite	010,C8	// I/O Config.  Tx Swap IQ; Rx Swap IQ; Tx CH Swap, Rx CH Swap; Rx Frame Mode; 2R2T bit; Invert data bus; Invert DATA_CLK
		{0x80,0x11,0x00},		//SPIWrite	011,00	// I/O Config.  Alt Word Order; -Rx1; -Rx2; -Tx1; -Tx2; Invert Rx Frame; Delay Rx Data
		{0x80,0x12,0x10},		//SPIWrite	012,10	// I/O Config.  Rx=2*Tx; Swap Ports; SDR; LVDS; Half Duplex; Single Port; Full Port; Swap Bits
		{0x80,0x06,0x08},		//SPIWrite	006,08	// PPORT Rx Delay (adjusts Tco Dataclk->Data)
		{0x80,0x07,0x04},		//SPIWrite	007,04	// PPORT TX Delay (adjusts setup/hold FBCLK->Data)
		
		{0x80,0x3C,0x21},		//SPIWrite	03C,21	// CLK_OUT slew; LVDS: Rx Term; Bypass Bias R; Tx LO VCM; Bias[2:0]
		{0x80,0x3D,0x00},		//SPIWrite	03D,00	// LVDS polarity invert
		{0x80,0x3E,0x00},		//SPIWrite	03E,00	// LVDS polarity invert
		
		//************************************************************
		// Setup AuxDAC
		//************************************************************
		{0x80,0x18,0x00},		//SPIWrite	018,00	// AuxDAC1 Word[9:2]
		{0x80,0x19,0x00},		//SPIWrite	019,00	// AuxDAC2 Word[9:2]
		{0x80,0x1A,0x00},		//SPIWrite	01A,00	// AuxDAC1 Config and Word[1:0]
		{0x80,0x1B,0x00},		//SPIWrite	01B,00	// AuxDAC2 Config and Word[1:0]
		{0x80,0x23,0xFF},		//SPIWrite	023,FF	// AuxDAC Manaul/Auto Control
		{0x80,0x26,0x00},		//SPIWrite	026,00	// AuxDAC Manual Select Bit/GPO Manual Select
		{0x80,0x30,0x00},		//SPIWrite	030,00	// AuxDAC1 Rx Delay
		{0x80,0x31,0x00},		//SPIWrite	031,00	// AuxDAC1 Tx Delay
		{0x80,0x32,0x00},		//SPIWrite	032,00	// AuxDAC2 Rx Delay
		{0x80,0x33,0x00},		//SPIWrite	033,00	// AuxDAC2 Tx Delay
		
		//************************************************************
		// Setup AuxADC
		//************************************************************
		{0x80,0x0B,0x00},		//SPIWrite	00B,00	// Temp Sensor Setup (Offset)
		{0x80,0x0C,0x00},		//SPIWrite	00C,00	// Temp Sensor Setup (Temp Window)
		{0x80,0x0D,0x03},		//SPIWrite	00D,03	// Temp Sensor Setup (Periodic Measure)
		{0x80,0x0F,0x04},		//SPIWrite	00F,04	// Temp Sensor Setup (Decimation)
		{0x80,0x1C,0x10},		//SPIWrite	01C,10	// AuxADC Setup (Clock Div)
		{0x80,0x1D,0x01},		//SPIWrite	01D,01	// AuxADC Setup (Decimation/Enable)
		
		//************************************************************
		// Setup Control Outs
		//************************************************************
		{0x80,0x35,0x00},		//SPIWrite	035,00	// Ctrl Out index
		{0x80,0x36,0xFF},		//SPIWrite	036,FF	// Ctrl Out [7:0] output enable
		
		//************************************************************
		// Setup GPO
		//************************************************************
		{0x80,0x3A,0x27},		//SPIWrite	03A,27	// Set number of REFCLK cycles for 1us delay timer
		{0x80,0x20,0x00},		//SPIWrite	020,00	// GPO Auto Enable Setup in RX and TX
		{0x80,0x27,0x03},		//SPIWrite	027,03	// GPO Manual and GPO auto value in ALERT
		{0x80,0x28,0x00},		//SPIWrite	028,00	// GPO_0 RX Delay
		{0x80,0x29,0x00},		//SPIWrite	029,00	// GPO_1 RX Delay
		{0x80,0x2A,0x00},		//SPIWrite	02A,00	// GPO_2 RX Delay
		{0x80,0x2B,0x00},		//SPIWrite	02B,00	// GPO_3 RX Delay
		{0x80,0x2C,0x00},		//SPIWrite	02C,00	// GPO_0 TX Delay
		{0x80,0x2D,0x00},		//SPIWrite	02D,00	// GPO_1 TX Delay
		{0x80,0x2E,0x00},		//SPIWrite	02E,00	// GPO_2 TX Delay
		{0x80,0x2F,0x00},		//SPIWrite	02F,00	// GPO_3 TX Delay
		
		//************************************************************
		// Setup RF PLL non-frequency-dependent registers
		//************************************************************
		{0x82,0x61,0x00},		//SPIWrite	261,00	// Set Rx LO Power mode
		{0x82,0xA1,0x00},		//SPIWrite	2A1,00	// Set Tx LO Power mode
		{0x82,0x48,0x0B},		//SPIWrite	248,0B	// Enable Rx VCO LDO
		{0x82,0x88,0x0B},		//SPIWrite	288,0B	// Enable Tx VCO LDO
		{0x82,0x46,0x02},		//SPIWrite	246,02	// Set VCO Power down TCF bits
		{0x82,0x86,0x02},		//SPIWrite	286,02	// Set VCO Power down TCF bits
		{0x82,0x49,0x8E},		//SPIWrite	249,8E	// Set VCO cal length
		{0x82,0x89,0x8E},		//SPIWrite	289,8E	// Set VCO cal length
		{0x82,0x3B,0x80},		//SPIWrite	23B,80	// Enable Rx VCO cal
		{0x82,0x7B,0x80},		//SPIWrite	27B,80	// Enable Tx VCO cal
		{0x82,0x43,0x0D},		//SPIWrite	243,0D	// Set Rx prescaler bias
		{0x82,0x83,0x0D},		//SPIWrite	283,0D	// Set Tx prescaler bias
		{0x82,0x3D,0x10},		//SPIWrite	23D,10	// Clear Half VCO cal clock setting
		{0x82,0x7D,0x10},		//SPIWrite	27D,10	// Clear Half VCO cal clock setting
		
		{0x80,0x15,0x0C},		//SPIWrite	015,0C	// Set Dual Synth mode bit
		{0x80,0x14,0x15},		//SPIWrite	014,15	// Set Force ALERT State bit
		{0x80,0x13,0x01},		//SPIWrite	013,01	// Set ENSM FDD mode
		{0xF0,0x00,0x01},		//WAIT	1	// waits 1 ms
		{0x82,0x3D,0x14},		//SPIWrite	23D,14	// Start RX CP cal
		{0x02,0x44,0x71},		//WAIT_CALDONE	RXCP,100	// Wait for CP cal to complete, Max RXCP Cal time: 460.800 (us)(Done when 0x244[7]==1)
		
		{0x82,0x7D,0x14},		//SPIWrite	27D,14	// Start TX CP cal
		{0x02,0x84,0x71},		//WAIT_CALDONE	TXCP,100	// Wait for CP cal to complete, Max TXCP Cal time: 460.800 (us)(Done when 0x284[7]==1)
		
		//************************************************************
		// FDD RX,TX Synth Frequency: 2420.000000,2500.000000 MHz
		//************************************************************
		//************************************************************
		// Setup Rx Frequency-Dependent Syntheisizer Registers
		//************************************************************
		{0x82,0x3A,0x4D},		//SPIWrite	23A,4D	// Set VCO Output level[3:0]
		{0x82,0x39,0xC1},		//SPIWrite	239,C1	// Set Init ALC Value[3:0] and VCO Varactor[3:0]
		{0x82,0x42,0x16},		//SPIWrite	242,16	// Set VCO Bias Tcf[1:0] and VCO Bias Ref[2:0]
		{0x82,0x38,0x68},		//SPIWrite	238,68	// Set VCO Cal Offset[3:0]
		{0x82,0x45,0x00},		//SPIWrite	245,00	// Set VCO Cal Ref Tcf[2:0]
		{0x82,0x51,0x0C},		//SPIWrite	251,0C	// Set VCO Varactor Reference[3:0]
		{0x82,0x50,0x70},		//SPIWrite	250,70	// Set VCO Varactor Ref Tcf[2:0] and VCO Varactor Offset[3:0]
		{0x82,0x3B,0x8B},		//SPIWrite	23B,8B	// Set Synth Loop Filter charge pump current (Icp)
		{0x82,0x3E,0xF6},		//SPIWrite	23E,F6	// Set Synth Loop Filter C2 and C1
		{0x82,0x3F,0xD5},		//SPIWrite	23F,D5	// Set Synth Loop Filter  R1 and C3
		{0x82,0x40,0x0E},		//SPIWrite	240,0E	// Set Synth Loop Filter R3
		
		//************************************************************
		// Setup Tx Frequency-Dependent Syntheisizer Registers
		//************************************************************
		{0x82,0x7A,0x4D},		//SPIWrite	27A,4D	// Set VCO Output level[3:0]
		{0x82,0x79,0xC1},		//SPIWrite	279,C1	// Set Init ALC Value[3:0] and VCO Varactor[3:0]
		{0x82,0x82,0x16},		//SPIWrite	282,16	// Set VCO Bias Tcf[1:0] and VCO Bias Ref[2:0]
		{0x82,0x78,0x70},		//SPIWrite	278,70	// Set VCO Cal Offset[3:0]
		{0x82,0x85,0x00},		//SPIWrite	285,00	// Set VCO Cal Ref Tcf[2:0]
		{0x82,0x91,0x0C},		//SPIWrite	291,0C	// Set VCO Varactor Reference[3:0]
		{0x82,0x90,0x70},		//SPIWrite	290,70	// Set VCO Varactor Ref Tcf[2:0] and VCO Varactor Offset[3:0]
		{0x82,0x7B,0x8B},		//SPIWrite	27B,8B	// Set Synth Loop Filter charge pump current (Icp)
		{0x82,0x7E,0xF6},		//SPIWrite	27E,F6	// Set Synth Loop Filter C2 and C1
		{0x82,0x7F,0xD5},		//SPIWrite	27F,D5	// Set Synth Loop Filter  R1 and C3
		{0x82,0x80,0x0E},		//SPIWrite	280,0E	// Set Synth Loop Filter R3
		
		//************************************************************
		// Write Rx and Tx Frequency Words
		//************************************************************
		{0x82,0x33,0x00},		//SPIWrite	233,00	// Write Rx Synth Fractional Freq Word[7:0]
		{0x82,0x34,0x00},		//SPIWrite	234,00	// Write Rx Synth Fractional Freq Word[15:8]
		{0x82,0x35,0x00},		//SPIWrite	235,00	// Write Rx Synth Fractional Freq Word[22:16]
		{0x82,0x32,0x00},		//SPIWrite	232,00	// Write Rx Synth Integer Freq Word[10:8]
		{0x82,0x31,0x79},		//SPIWrite	231,79	// Write Rx Synth Integer Freq Word[7:0]
		{0x80,0x05,0x11},		//SPIWrite	005,11	// Set LO divider setting
		{0x82,0x73,0x00},		//SPIWrite	273,00	// Write Tx Synth Fractional Freq Word[7:0]
		{0x82,0x74,0x00},		//SPIWrite	274,00	// Write Tx Synth Fractional Freq Word[15:8]
		{0x82,0x75,0x00},		//SPIWrite	275,00	// Write Tx Synth Fractional Freq Word[22:16]
		{0x82,0x72,0x00},		//SPIWrite	272,00	// Write Tx Synth Integer Freq Word[10:8]
		{0x82,0x71,0x7D},		//SPIWrite	271,7D	// Write Tx Synth Integer Freq Word[7:0] (starts VCO cal)
		{0x80,0x05,0x11},		//SPIWrite	005,11	// Set LO divider setting
		{0x02,0x47,0x11},		//SPIRead	247	// Check RX RF PLL lock status (0x247[1]==1 is locked)
		{0x02,0x87,0x11},		//SPIRead	287	// Check TX RF PLL lock status (0x287[1]==1 is locked)
		
		//************************************************************
		// Program Mixer GM Sub-table
		//************************************************************
		{0x81,0x3F,0x02},		//SPIWrite	13F,02	// Start Clock
		{0x81,0x38,0x0F},		//SPIWrite	138,0F	// Addr Table Index
		{0x81,0x39,0x78},		//SPIWrite	139,78	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x00},		//SPIWrite	13B,00	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x0E},		//SPIWrite	138,0E	// Addr Table Index
		{0x81,0x39,0x74},		//SPIWrite	139,74	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x0D},		//SPIWrite	13B,0D	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x0D},		//SPIWrite	138,0D	// Addr Table Index
		{0x81,0x39,0x70},		//SPIWrite	139,70	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x15},		//SPIWrite	13B,15	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x0C},		//SPIWrite	138,0C	// Addr Table Index
		{0x81,0x39,0x6C},		//SPIWrite	139,6C	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x1B},		//SPIWrite	13B,1B	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x0B},		//SPIWrite	138,0B	// Addr Table Index
		{0x81,0x39,0x68},		//SPIWrite	139,68	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x21},		//SPIWrite	13B,21	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x0A},		//SPIWrite	138,0A	// Addr Table Index
		{0x81,0x39,0x64},		//SPIWrite	139,64	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x25},		//SPIWrite	13B,25	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x09},		//SPIWrite	138,09	// Addr Table Index
		{0x81,0x39,0x60},		//SPIWrite	139,60	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x29},		//SPIWrite	13B,29	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x08},		//SPIWrite	138,08	// Addr Table Index
		{0x81,0x39,0x5C},		//SPIWrite	139,5C	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x2C},		//SPIWrite	13B,2C	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x07},		//SPIWrite	138,07	// Addr Table Index
		{0x81,0x39,0x58},		//SPIWrite	139,58	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x2F},		//SPIWrite	13B,2F	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x06},		//SPIWrite	138,06	// Addr Table Index
		{0x81,0x39,0x54},		//SPIWrite	139,54	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x31},		//SPIWrite	13B,31	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x05},		//SPIWrite	138,05	// Addr Table Index
		{0x81,0x39,0x50},		//SPIWrite	139,50	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x33},		//SPIWrite	13B,33	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x04},		//SPIWrite	138,04	// Addr Table Index
		{0x81,0x39,0x4C},		//SPIWrite	139,4C	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x34},		//SPIWrite	13B,34	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x03},		//SPIWrite	138,03	// Addr Table Index
		{0x81,0x39,0x48},		//SPIWrite	139,48	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x35},		//SPIWrite	13B,35	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x02},		//SPIWrite	138,02	// Addr Table Index
		{0x81,0x39,0x30},		//SPIWrite	139,30	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x3A},		//SPIWrite	13B,3A	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x01},		//SPIWrite	138,01	// Addr Table Index
		{0x81,0x39,0x18},		//SPIWrite	139,18	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x3D},		//SPIWrite	13B,3D	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x38,0x00},		//SPIWrite	138,00	// Addr Table Index
		{0x81,0x39,0x00},		//SPIWrite	139,00	// Gain
		{0x81,0x3A,0x00},		//SPIWrite	13A,00	// Bias
		{0x81,0x3B,0x3E},		//SPIWrite	13B,3E	// GM
		{0x81,0x3F,0x06},		//SPIWrite	13F,06	// Write Words
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x3F,0x02},		//SPIWrite	13F,02	// Clear Write Bit
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay for 3 ADCCLK/16 clock cycles (Dummy Write)
		{0x81,0x3C,0x00},		//SPIWrite	13C,00	// Delay ~1us (Dummy Write)
		{0x81,0x3F,0x00},		//SPIWrite	13F,00	// Stop Clock
		
		//************************************************************
		// Program Rx Gain Tables with GainTable2300MHz.csv
		//************************************************************
		
		{0x81,0x37,0x1A},		//SPIWrite	137,1A	// Start Gain Table Clock
		{0x81,0x30,0x00},		//SPIWrite	130,00	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x01},		//SPIWrite	130,01	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x02},		//SPIWrite	130,02	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x03},		//SPIWrite	130,03	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x01},		//SPIWrite	132,01	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x04},		//SPIWrite	130,04	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x02},		//SPIWrite	132,02	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x05},		//SPIWrite	130,05	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x03},		//SPIWrite	132,03	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x06},		//SPIWrite	130,06	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x04},		//SPIWrite	132,04	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x07},		//SPIWrite	130,07	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x05},		//SPIWrite	132,05	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x08},		//SPIWrite	130,08	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x03},		//SPIWrite	132,03	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x09},		//SPIWrite	130,09	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x04},		//SPIWrite	132,04	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x0A},		//SPIWrite	130,0A	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x05},		//SPIWrite	132,05	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x0B},		//SPIWrite	130,0B	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x06},		//SPIWrite	132,06	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x0C},		//SPIWrite	130,0C	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x07},		//SPIWrite	132,07	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x0D},		//SPIWrite	130,0D	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x08},		//SPIWrite	132,08	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x0E},		//SPIWrite	130,0E	// Gain Table Index
	};
	row = sizeof(Ad9361Config)/sizeof(Ad9361Config[0]);
	for(i=0;i<row;i++){
		SetAd(Ad9361Config[i]);
	}
}

static void Ad9361Config_03(void)
{
	u16 row,i;
	u8 Ad9361Config[][3] = {
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x09},		//SPIWrite	132,09	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x0F},		//SPIWrite	130,0F	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0A},		//SPIWrite	132,0A	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x10},		//SPIWrite	130,10	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0B},		//SPIWrite	132,0B	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x11},		//SPIWrite	130,11	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0C},		//SPIWrite	132,0C	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x12},		//SPIWrite	130,12	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0D},		//SPIWrite	132,0D	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x13},		//SPIWrite	130,13	// Gain Table Index
		{0x81,0x31,0x01},		//SPIWrite	131,01	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0E},		//SPIWrite	132,0E	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x14},		//SPIWrite	130,14	// Gain Table Index
		{0x81,0x31,0x02},		//SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x09},		//SPIWrite	132,09	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x15},		//SPIWrite	130,15	// Gain Table Index
		{0x81,0x31,0x02},		//SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0A},		//SPIWrite	132,0A	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x16},		//SPIWrite	130,16	// Gain Table Index
		{0x81,0x31,0x02},		//SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0B},		//SPIWrite	132,0B	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x17},		//SPIWrite	130,17	// Gain Table Index
		{0x81,0x31,0x02},		//SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0C},		//SPIWrite	132,0C	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x18},		//SPIWrite	130,18	// Gain Table Index
		{0x81,0x31,0x02},		//SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0D},		//SPIWrite	132,0D	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x19},		//SPIWrite	130,19	// Gain Table Index
		{0x81,0x31,0x02},		//SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0E},		//SPIWrite	132,0E	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x1A},		//SPIWrite	130,1A	// Gain Table Index
		{0x81,0x31,0x02},		//SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x0F},		//SPIWrite	132,0F	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x1B},		//SPIWrite	130,1B	// Gain Table Index
		{0x81,0x31,0x02},		//SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x10},		//SPIWrite	132,10	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x1C},		//SPIWrite	130,1C	// Gain Table Index
		{0x81,0x31,0x02},		//SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2B},		//SPIWrite	132,2B	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x1D},		//SPIWrite	130,1D	// Gain Table Index
		{0x81,0x31,0x02},		//SPIWrite	131,02	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2C},		//SPIWrite	132,2C	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x1E},		//SPIWrite	130,1E	// Gain Table Index
		{0x81,0x31,0x04},		//SPIWrite	131,04	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x27},		//SPIWrite	132,27	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x1F},		//SPIWrite	130,1F	// Gain Table Index
		{0x81,0x31,0x04},		//SPIWrite	131,04	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x28},		//SPIWrite	132,28	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x20},		//SPIWrite	130,20	// Gain Table Index
		{0x81,0x31,0x04},		//SPIWrite	131,04	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x29},		//SPIWrite	132,29	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x21},		//SPIWrite	130,21	// Gain Table Index
		{0x81,0x31,0x04},		//SPIWrite	131,04	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2A},		//SPIWrite	132,2A	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x22},		//SPIWrite	130,22	// Gain Table Index
		{0x81,0x31,0x04},		//SPIWrite	131,04	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2B},		//SPIWrite	132,2B	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x23},		//SPIWrite	130,23	// Gain Table Index
		{0x81,0x31,0x24},		//SPIWrite	131,24	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x21},		//SPIWrite	132,21	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x24},		//SPIWrite	130,24	// Gain Table Index
		{0x81,0x31,0x24},		//SPIWrite	131,24	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x22},		//SPIWrite	132,22	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x25},		//SPIWrite	130,25	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x20},		//SPIWrite	132,20	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x26},		//SPIWrite	130,26	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x21},		//SPIWrite	132,21	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x27},		//SPIWrite	130,27	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x22},		//SPIWrite	132,22	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x28},		//SPIWrite	130,28	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x23},		//SPIWrite	132,23	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x29},		//SPIWrite	130,29	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x24},		//SPIWrite	132,24	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x2A},		//SPIWrite	130,2A	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x25},		//SPIWrite	132,25	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x2B},		//SPIWrite	130,2B	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x26},		//SPIWrite	132,26	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x2C},		//SPIWrite	130,2C	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x27},		//SPIWrite	132,27	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x2D},		//SPIWrite	130,2D	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x28},		//SPIWrite	132,28	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x2E},		//SPIWrite	130,2E	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x29},		//SPIWrite	132,29	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x2F},		//SPIWrite	130,2F	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2A},		//SPIWrite	132,2A	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x30},		//SPIWrite	130,30	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2B},		//SPIWrite	132,2B	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x31},		//SPIWrite	130,31	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2C},		//SPIWrite	132,2C	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x32},		//SPIWrite	130,32	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2D},		//SPIWrite	132,2D	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x33},		//SPIWrite	130,33	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2E},		//SPIWrite	132,2E	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x34},		//SPIWrite	130,34	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2F},		//SPIWrite	132,2F	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x35},		//SPIWrite	130,35	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x30},		//SPIWrite	132,30	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x36},		//SPIWrite	130,36	// Gain Table Index
		{0x81,0x31,0x44},		//SPIWrite	131,44	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x31},		//SPIWrite	132,31	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x37},		//SPIWrite	130,37	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2E},		//SPIWrite	132,2E	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x38},		//SPIWrite	130,38	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x2F},		//SPIWrite	132,2F	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x39},		//SPIWrite	130,39	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x30},		//SPIWrite	132,30	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x3A},		//SPIWrite	130,3A	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x31},		//SPIWrite	132,31	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x3B},		//SPIWrite	130,3B	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x32},		//SPIWrite	132,32	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x3C},		//SPIWrite	130,3C	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x33},		//SPIWrite	132,33	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x3D},		//SPIWrite	130,3D	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x34},		//SPIWrite	132,34	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x3E},		//SPIWrite	130,3E	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x35},		//SPIWrite	132,35	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x3F},		//SPIWrite	130,3F	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x36},		//SPIWrite	132,36	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x40},		//SPIWrite	130,40	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x37},		//SPIWrite	132,37	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x41},		//SPIWrite	130,41	// Gain Table Index
		{0x81,0x31,0x64},		//SPIWrite	131,64	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x42},		//SPIWrite	130,42	// Gain Table Index
		{0x81,0x31,0x65},		//SPIWrite	131,65	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x43},		//SPIWrite	130,43	// Gain Table Index
		{0x81,0x31,0x66},		//SPIWrite	131,66	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x44},		//SPIWrite	130,44	// Gain Table Index
		{0x81,0x31,0x67},		//SPIWrite	131,67	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x45},		//SPIWrite	130,45	// Gain Table Index
		{0x81,0x31,0x68},		//SPIWrite	131,68	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x46},		//SPIWrite	130,46	// Gain Table Index
		{0x81,0x31,0x69},		//SPIWrite	131,69	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x47},		//SPIWrite	130,47	// Gain Table Index
		{0x81,0x31,0x6A},		//SPIWrite	131,6A	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x48},		//SPIWrite	130,48	// Gain Table Index
		{0x81,0x31,0x6B},		//SPIWrite	131,6B	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x49},		//SPIWrite	130,49	// Gain Table Index
		{0x81,0x31,0x6C},		//SPIWrite	131,6C	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x4A},		//SPIWrite	130,4A	// Gain Table Index
		{0x81,0x31,0x6D},		//SPIWrite	131,6D	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x4B},		//SPIWrite	130,4B	// Gain Table Index
		{0x81,0x31,0x6E},		//SPIWrite	131,6E	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x4C},		//SPIWrite	130,4C	// Gain Table Index
		{0x81,0x31,0x6F},		//SPIWrite	131,6F	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x38},		//SPIWrite	132,38	// TIA & LPF Word
		{0x81,0x33,0x20},		//SPIWrite	133,20	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x4D},		//SPIWrite	130,4D	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x4E},		//SPIWrite	130,4E	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x4F},		//SPIWrite	130,4F	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x50},		//SPIWrite	130,50	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x51},		//SPIWrite	130,51	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x52},		//SPIWrite	130,52	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x53},		//SPIWrite	130,53	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x54},		//SPIWrite	130,54	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x55},		//SPIWrite	130,55	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
	};
	row = sizeof(Ad9361Config)/sizeof(Ad9361Config[0]);
	for(i=0;i<row;i++){
		SetAd(Ad9361Config[i]);
	}
}

static void Ad9361Config_04(void)
{
	u16 row,i;
	u8 Ad9361Config[][3] = {
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x56},		//SPIWrite	130,56	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x57},		//SPIWrite	130,57	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x58},		//SPIWrite	130,58	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x59},		//SPIWrite	130,59	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x30,0x5A},		//SPIWrite	130,5A	// Gain Table Index
		{0x81,0x31,0x00},		//SPIWrite	131,00	// Ext LNA, Int LNA, & Mixer Gain Word
		{0x81,0x32,0x00},		//SPIWrite	132,00	// TIA & LPF Word
		{0x81,0x33,0x00},		//SPIWrite	133,00	// DC Cal bit & Dig Gain Word
		{0x81,0x37,0x1E},		//SPIWrite	137,1E	// Write Words
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay 3 ADCCLK/16 cycles
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x37,0x1A},		//SPIWrite	137,1A	// Clear Write Bit
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x34,0x00},		//SPIWrite	134,00	// Dummy Write to delay ~1us
		{0x81,0x37,0x00},		//SPIWrite	137,00	// Stop Gain Table Clock
		//************************************************************
		// Setup Rx Manual Gain Registers
		//************************************************************
		{0x80,0xFA,0xE0},		//SPIWrite	0FA,E0	// Gain Control Mode Select
		{0x80,0xFB,0x08},		//SPIWrite	0FB,08	// Table, Digital Gain, Man Gain Ctrl
		{0x80,0xFC,0x23},		//SPIWrite	0FC,23	// Incr Step Size, ADC Overrange Size
		{0x80,0xFD,0x4C},		//SPIWrite	0FD,4C	// Max Full/LMT Gain Table Index
		{0x80,0xFE,0x44},		//SPIWrite	0FE,44	// Decr Step Size, Peak Overload Time
		{0x81,0x00,0x6F},		//SPIWrite	100,6F	// Max Digital Gain
		{0x81,0x04,0x2F},		//SPIWrite	104,2F	// ADC Small Overload Threshold
		{0x81,0x05,0x3A},		//SPIWrite	105,3A	// ADC Large Overload Threshold
		{0x81,0x07,0x2B},		//SPIWrite	107,2B	// Small LMT Overload Threshold
		{0x81,0x08,0x31},		//SPIWrite	108,31	// Large LMT Overload Threshold
		{0x81,0x09,0x4C},		//SPIWrite	109,4C	// Rx1 Full/LMT Gain Index
		{0x81,0x0A,0x58},		//SPIWrite	10A,58	// Rx1 LPF Gain Index
		{0x81,0x0B,0x00},		//SPIWrite	10B,00	// Rx1 Digital Gain Index
		{0x81,0x0C,0x4C},		//SPIWrite	10C,4C	// Rx2 Full/LMT Gain Index
		{0x81,0x0D,0x18},		//SPIWrite	10D,18	// Rx2 LPF Gain Index
		{0x81,0x0E,0x00},		//SPIWrite	10E,00	// Rx2 Digital Gain Index
		{0x81,0x14,0x30},		//SPIWrite	114,30	// Low Power Threshold
		{0x81,0x1A,0x27},		//SPIWrite	11A,27	// Initial LMT Gain Limit
		{0x80,0x81,0x00},		//SPIWrite	081,00	// Tx Symbol Gain Control
		//************************************************************
		// RX Baseband Filter Tuning (Real BW: 19.365514 MHz) 3dB Filter
		// Corner @ 27.111720 MHz)
		//************************************************************
		{0x81,0xFB,0x13},		//SPIWrite	1FB,13	// RX Freq Corner (MHz)
		{0x81,0xFC,0x2F},		//SPIWrite	1FC,2F	// RX Freq Corner (Khz)
		{0x81,0xF8,0x03},		//SPIWrite	1F8,03	// Rx BBF Tune Divider[7:0]
		{0x81,0xF9,0x1E},		//SPIWrite	1F9,1E	// RX BBF Tune Divider[8]
		
		{0x81,0xD5,0x3F},		//SPIWrite	1D5,3F	// Set Rx Mix LO CM
		{0x81,0xC0,0x03},		//SPIWrite	1C0,03	// Set GM common mode
		{0x81,0xE2,0x02},		//SPIWrite	1E2,02	// Enable Rx1 Filter Tuner 
		{0x81,0xE3,0x02},		//SPIWrite	1E3,02	// Enable Rx2 Filter Tuner 
		{0x80,0x16,0x80},		//SPIWrite	016,80	// Start RX Filter Tune
		{0x00,0x16,0x70},		//WAIT_CALDONE	RXFILTER,2000	// Wait for RX filter to tune, Max Cal Time: 2.482 us (Done when 0x016[7]==0)
		
		{0x81,0xE2,0x03},		//SPIWrite	1E2,03	// Disable Rx Filter Tuner (Rx1)
		{0x81,0xE3,0x03},		//SPIWrite	1E3,03	// Disable Rx Filter Tuner (Rx2)
		//************************************************************
		// TX Baseband Filter Tuning (Real BW: 16.944758 MHz) 3dB Filter
		// Corner @ 27.111614 MHz)
		//************************************************************
		{0x80,0xD6,0x04},		//SPIWrite	0D6,04	// TX BBF Tune Divider[7:0]
		{0x80,0xD7,0x1E},		//SPIWrite	0D7,1E	// TX BBF Tune Divider[8]
		
		{0x80,0xCA,0x22},		//SPIWrite	0CA,22	// Enable Tx Filter Tuner
		{0x80,0x16,0x40},		//SPIWrite	016,40	// Start Tx Filter Tune
		{0x00,0x16,0x60},		//WAIT_CALDONE	TXFILTER,2000	// Wait for TX filter to tune, Max Cal Time: 1.926 us (Done when 0x016[6]==0)
		
		{0x80,0xCA,0x26},		//SPIWrite	0CA,26	// Disable Tx Filter Tuner (Both Channels)
		//************************************************************
		// RX TIA Setup:  Setup values scale based on RxBBF calibration
		// results.  See information in Calibration Guide. 
		//************************************************************
		{0x01,0xEB,0xFF},		//SPIRead	1EB	// Read RXBBF C3(MSB)
		{0x01,0xEC,0xFF},		//SPIRead	1EC	// Read RXBBF C3(LSB)
		{0x01,0xE6,0xFF},		//SPIRead	1E6	// Read RXBBF R2346
		{0x81,0xDB,0x20},		//SPIWrite	1DB,20	// Set TIA selcc[2:0]
		{0x81,0xDD,0x00},		//SPIWrite	1DD,00	// Set RX TIA1 C MSB[6:0]
		{0x81,0xDF,0x00},		//SPIWrite	1DF,00	// Set RX TIA2 C MSB[6:0]
		{0x81,0xDC,0x47},		//SPIWrite	1DC,47	// Set RX TIA1 C LSB[5:0]
		{0x81,0xDE,0x47},		//SPIWrite	1DE,47	// Set RX TIA2 C LSB[5:0]
		
		//************************************************************
		// TX Secondary Filter Calibration Setup:  Real Bandwidth
		// 16.944758MHz, 3dB Corner @ 84.723793MHz
		//************************************************************
		{0x80,0xD2,0x07},		//SPIWrite	0D2,07	// TX Secondary Filter PDF Cap cal[5:0]
		{0x80,0xD1,0x0C},		//SPIWrite	0D1,0C	// TX Secondary Filter PDF Res cal[3:0]
		{0x80,0xD0,0x57},		//SPIWrite	0D0,57	// Pdampbias
		
		//************************************************************
		// ADC Setup:  Tune ADC Performance based on RX analog filter tune
		// corner.  Real Bandwidth: 19.365502 MHz, ADC Clock Frequency:
		// 368.640000 MHz.  The values in registers 0x200 - 0x227 need to be
		// calculated using the equations in the Calibration Guide.
		//************************************************************
		{0x01,0xEB,0xFF},		//SPIRead	1EB	// Read RxBBF C3 MSB after calibration
		{0x01,0xEC,0xFF},		//SPIRead	1EC	// Read RxBBF C3 LSB after calibration
		{0x01,0xE6,0xFF},		//SPIRead	1E6	// Read RxBBF R3 after calibration
		
		{0x82,0x00,0x00},		//SPIWrite	200,00
		{0x82,0x01,0x00},		//SPIWrite	201,00
		{0x82,0x02,0x00},		//SPIWrite	202,00
		{0x82,0x03,0x24},		//SPIWrite	203,24
		{0x82,0x04,0x24},		//SPIWrite	204,24
		{0x82,0x05,0x00},		//SPIWrite	205,00
		{0x82,0x06,0x00},		//SPIWrite	206,00
		{0x82,0x07,0x6B},		//SPIWrite	207,6B
		{0x82,0x08,0x40},		//SPIWrite	208,40
		{0x82,0x09,0x34},		//SPIWrite	209,34
		{0x82,0x0A,0x41},		//SPIWrite	20A,41
		{0x82,0x0B,0x28},		//SPIWrite	20B,28
		{0x82,0x0C,0x43},		//SPIWrite	20C,43
		{0x82,0x0D,0x26},		//SPIWrite	20D,26
		{0x82,0x0E,0x00},		//SPIWrite	20E,00
		{0x82,0x0F,0x6E},		//SPIWrite	20F,6E
		{0x82,0x10,0x6E},		//SPIWrite	210,6E
		{0x82,0x11,0x6E},		//SPIWrite	211,6E
		{0x82,0x12,0x3F},		//SPIWrite	212,3F
		{0x82,0x13,0x3F},		//SPIWrite	213,3F
		{0x82,0x14,0x3F},		//SPIWrite	214,3F
		{0x82,0x15,0x41},		//SPIWrite	215,41
		{0x82,0x16,0x41},		//SPIWrite	216,41
		{0x82,0x17,0x41},		//SPIWrite	217,41
		{0x82,0x18,0x2E},		//SPIWrite	218,2E
		{0x82,0x19,0xA4},		//SPIWrite	219,A4
		{0x82,0x1A,0x26},		//SPIWrite	21A,26
		{0x82,0x1B,0x18},		//SPIWrite	21B,18
		{0x82,0x1C,0xA4},		//SPIWrite	21C,A4
		{0x82,0x1D,0x26},		//SPIWrite	21D,26
		{0x82,0x1E,0x18},		//SPIWrite	21E,18
		{0x82,0x1F,0xA4},		//SPIWrite	21F,A4
		{0x82,0x20,0x26},		//SPIWrite	220,26
		{0x82,0x21,0x2F},		//SPIWrite	221,2F
		{0x82,0x22,0x30},		//SPIWrite	222,30
		{0x82,0x23,0x40},		//SPIWrite	223,40
		{0x82,0x24,0x40},		//SPIWrite	224,40
		{0x82,0x25,0x2C},		//SPIWrite	225,2C
		{0x82,0x26,0x00},		//SPIWrite	226,00
		{0x82,0x27,0x00},		//SPIWrite	227,00
		//************************************************************
		// Setup and Run BB DC and RF DC Offset Calibrations
		//************************************************************
		{0x81,0x93,0x3F},		//SPIWrite	193,3F
		{0x81,0x90,0x0F},		//SPIWrite	190,0F	// Set BBDC tracking shift M value, only applies when BB DC tracking enabled
		{0x81,0x94,0x01},		//SPIWrite	194,01	// BBDC Cal setting
		{0x80,0x16,0x01},		//SPIWrite	016,01	// Start BBDC offset cal
		{0x00,0x16,0x00},		//WAIT_CALDONE	BBDC,2000	// BBDC Max Cal Time: 8767.361 us. Cal done when 0x016[0]==0
		
		{0x81,0x85,0x20},		//SPIWrite	185,20	// Set RF DC offset Wait Count
		{0x81,0x86,0x32},		//SPIWrite	186,32	// Set RF DC Offset Count[7:0]
		{0x81,0x87,0x24},		//SPIWrite	187,24	// Settings for RF DC cal
		{0x81,0x8B,0x83},		//SPIWrite	18B,83	// Settings for RF DC cal
		{0x81,0x88,0x05},		//SPIWrite	188,05	// Settings for RF DC cal
		{0x81,0x89,0x30},		//SPIWrite	189,30	// Settings for RF DC cal
		{0x80,0x16,0x02},		//SPIWrite	016,02	// Start RFDC offset cal
		
		//************************************************************
		// Tx Quadrature Calibration Settings
		//************************************************************
		{0x00,0xA3,0xFF},		//SPIRead	0A3	// Masked Read:  Read lower 6 bits, overwrite [7:6] below
		{0x80,0xA0,0x5F},		//SPIWrite	0A0,5F	// Set TxQuadcal NCO frequency
		{0x80,0xA3,0x80},		//SPIWrite	0A3,80	// Set TxQuadcal NCO frequency (Only update bits [7:6])
		{0x80,0xA1,0x7B},		//SPIWrite	0A1,7B	// Tx Quad Cal Configuration, Phase and Gain Cal Enable
		{0x80,0xA9,0xFF},		//SPIWrite	0A9,FF	// Set Tx Quad Cal Count
		{0x80,0xA2,0x7F},		//SPIWrite	0A2,7F	// Set Tx Quad Cal Kexp
		{0x80,0xA5,0x01},		//SPIWrite	0A5,01	// Set Tx Quad Cal Magnitude Threshhold
		{0x80,0xA6,0x01},		//SPIWrite	0A6,01	// Set Tx Quad Cal Magnitude Threshhold
		{0x80,0xAA,0x25},		//SPIWrite	0AA,25	// Set Tx Quad Cal Gain Table index
		{0x80,0xA4,0xF0},		//SPIWrite	0A4,F0	// Set Tx Quad Cal Settle Count
		{0x80,0xAE,0x00},		//SPIWrite	0AE,00	// Set Tx Quad Cal LPF Gain index incase Split table mode used
		
		{0x81,0x69,0xC0},		//SPIWrite	169,C0	// Disable Rx Quadrature Calibration before Running Tx Quadrature Calibration
		{0x80,0x16,0x10},		//SPIWrite	016,10	// Start Tx Quad cal
		{0x00,0x16,0x40},		//WAIT_CALDONE	TXQUAD,2000	// Wait for cal to complete (Done when 0x016[4]==0)
		
		{0x81,0x6A,0x75},		//SPIWrite	16A,75	// Set Kexp Phase
		{0x81,0x6B,0x95},		//SPIWrite	16B,95	// Set Kexp Amplitude & Prevent Positive Gain Bit
		{0x81,0x69,0xCF},		//SPIWrite	169,CF	// Enable Rx Quadrature Calibration Tracking
		{0x81,0x8B,0xAD},		//SPIWrite	18B,AD	// Enable BB and RF DC Tracking Calibrations
		{0x80,0x12,0x10},		//SPIWrite	012,10	// Cals done, Set PPORT Config
		{0x80,0x13,0x01},		//SPIWrite	013,01	// Set ENSM FDD/TDD bit
		{0x80,0x15,0x0C},		//SPIWrite	015,0C	// Set Dual Synth Mode, FDD External Control bits properly
		
		//************************************************************
		// Set Tx Attenuation: Tx1: 10.00 dB,  Tx2: 10.00 dB 
		//************************************************************
		{0x80,0x73,0x28},		//SPIWrite	073,28
		{0x80,0x74,0x00},		//SPIWrite	074,00
		{0x80,0x75,0x28},		//SPIWrite	075,28
		{0x80,0x76,0x00},		//SPIWrite	076,00
		//************************************************************
		// Setup RSSI and Power Measurement Duration Registers
		//************************************************************
		{0x81,0x50,0x0E},		//SPIWrite	150,0E	// RSSI Measurement Duration 0, 1
		{0x81,0x51,0x00},		//SPIWrite	151,00	// RSSI Measurement Duration 2, 3
		{0x81,0x52,0xFF},		//SPIWrite	152,FF	// RSSI Weighted Multiplier 0
		{0x81,0x53,0x00},		//SPIWrite	153,00	// RSSI Weighted Multiplier 1
		{0x81,0x54,0x00},		//SPIWrite	154,00	// RSSI Weighted Multiplier 2
		{0x81,0x55,0x00},		//SPIWrite	155,00	// RSSI Weighted Multiplier 3
		{0x81,0x56,0x00},		//SPIWrite	156,00	// RSSI Delay
		{0x81,0x57,0x00},		//SPIWrite	157,00	// RSSI Wait
		{0x81,0x58,0x0D},		//SPIWrite	158,0D	// RSSI Mode Select
		{0x81,0x5C,0x67},		//SPIWrite	15C,67	// Power Measurement Duration
		
	};
	row = sizeof(Ad9361Config)/sizeof(Ad9361Config[0]);
	for(i=0;i<row;i++){
		SetAd(Ad9361Config[i]);
	}
}

void cfgAd9361_droneid()
{
	Ad9361Config_01();
	Ad9361Config_02();
	Ad9361Config_03();
	Ad9361Config_04();
}
