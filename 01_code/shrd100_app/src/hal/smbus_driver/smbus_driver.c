#include <stdint.h>

#include "smbus_driver.h"
#include "sleep.h"
#include "xgpio.h"
#include "xstatus.h"

XGpio Gpio;

// XGpioPs gpioInsPs = {0};

/**
 * @brief init smbus gpio
 * @param
 * @retval
 */
void smbus_sw_init(void)
{
	int Status;

//	/* Initialize the GPIO driver */
//	Status = XGpio_Initialize(&Gpio, XPAR_AXI_GPIO_0_DEVICE_ID);
//	if (Status != XST_SUCCESS)
//	{
//		xil_printf("Gpio Initialization Failed\r\n");
//	}
//	else
//	{
//		XGpio_SetDataDirection(&Gpio, SMBUS_SCL_GPIO_PIN, 0);
//		XGpio_SetDataDirection(&Gpio, SMBUS_SDA_GPIO_PIN, 0);
//
//		SDA_UP;
//		SCL_UP;
//	}
}

/**
 * @brief a simple delay function
 * @param
 * @retval
 */
static void delay_us(uint32_t time)
{
	usleep(time);
}

/**
 * @brief send smbus start condition
 * @param
 * @retval
 */
static void smbus_sw_start(void)
{
	//	SCL_DOWN;
	delay_us(20);
	SDA_UP;
	SDA_OUT;
	delay_us(20);
	SCL_UP;
	delay_us(20);
	SDA_DOWN;
	delay_us(20);
	SCL_DOWN;
	delay_us(20);
}

/**
 * @brief send smbus stop condition
 * @param
 * @retval
 */
static void smbus_sw_stop(void)
{
	SCL_DOWN;
	delay_us(20);
	SDA_OUT;
	SDA_DOWN;
	delay_us(20);
	SCL_UP;
	delay_us(20);
	SDA_UP;
}

/**
 * @brief send smbus nack condition
 * @param
 * @retval
 */
static void smbus_sw_nack(void)
{
	delay_us(1);
	SDA_OUT;
	SDA_UP;
	delay_us(20);
	SCL_UP;
	delay_us(20);
	SCL_DOWN;
}

/**
 * @brief wait for slave response
 * @param
 * @retval 0 if ack, else nack
 */
static uint8_t smbus_sw_wait_ack(void)
{
	uint16_t timeout = 1500;

	delay_us(1);
	//	SDA_UP;
	SDA_IN;
	delay_us(20);
	while (READ_SDA)
	{
		if (timeout--)
		{
			delay_us(1);
		}
		else
		{
			return 1;
		}
	}
	SCL_UP;
	delay_us(20);
	SCL_DOWN;

	SDA_OUT; // new

	return 0;
}

/**
 * @brief wait for slave response
 * @param
 * @retval 0 if ack, else nack
 */
static uint8_t smbus_sw_answer_ack(void)
{
	delay_us(20);
	SDA_OUT;
	delay_us(20);
	SDA_DOWN;
	delay_us(20);
	SCL_UP;
	delay_us(20);
	SCL_DOWN;
	delay_us(20);
	SDA_UP; // new

	return 0;
}

/**
 * @brief write 1 byte via smbus bus
 * @param data to write
 * @retval
 */
static void smbus_sw_write_byte(uint8_t data)
{
	int32_t i = 7;
	uint8_t tmp = 0;

	// 	delay_us(20);
	SDA_OUT;

	for (; i >= 0; i--)
	{
		tmp = (data >> i) & 0X01;
		delay_us(1);
		if (tmp)
		{
			SDA_UP;
		}
		else
		{
			SDA_DOWN;
		}
		delay_us(20);
		SCL_UP;
		delay_us(20);
		SCL_DOWN;
	}
}

/**
 * @brief read 1 byte via smbus bus
 * @param
 * @retval received data
 */
static uint8_t smbus_sw_read_byte(void)
{
	int32_t i = 7;
	uint8_t tmp = 0;

	SDA_IN;

	for (; i >= 0; i--)
	{
		delay_us(20);
		SCL_UP;
		delay_us(1);
		tmp |= (READ_SDA << i);
		delay_us(20);
		SCL_DOWN;
	}

	return tmp;
}

/**
* @brief write slave register
* @param slave addr ----- smbus slave address
	   register addr ----- slave register address
	   buff ----- a pointer of data buffer
	   len ----- the length of data to write
* @retval 0 if success
*/
uint8_t smbus_sw_write_register(uint8_t slave_addr, uint8_t register_addr, uint8_t *buff, uint8_t len)
{
	int err = 0;
	uint8_t i = 0;

	/* start condition */
	smbus_sw_start();

	/* send slave address */
	smbus_sw_write_byte(slave_addr << 0);
	if (smbus_sw_wait_ack())
	{
		err = 1;
		goto ret;
	}

	/* send register address */
	smbus_sw_write_byte(register_addr);
	if (smbus_sw_wait_ack())
	{
		err = 2;
		goto ret;
	}

	for (; i < len; i++)
	{
		/* send data */
		smbus_sw_write_byte(*(buff + i));
		if (smbus_sw_wait_ack())
		{
			err = 3;
			goto ret;
		}
	}

ret:
	/* stop condition */
	smbus_sw_stop();

	return err;
}

/**
* @brief read slave register
* @param slave addr ----- smbus slave address
	  register addr ----- slave register address
	  buff ----- a pointer of data buffer
	  len ----- the length of data to write
* @retval 0 if success
*/
uint8_t smbus_sw_read_register(uint8_t slave_addr, uint8_t register_addr, uint8_t *buff, uint8_t len)
{
	int err = 0;
	uint8_t i = 0;

	/* start condition */
	smbus_sw_start();

	/* send slave address(write) */
	smbus_sw_write_byte(slave_addr << 0);
	if (smbus_sw_wait_ack())
	{
		err = 4;
		goto ret;
	}

	delay_us(40);
	/* send register address */
	smbus_sw_write_byte(register_addr);
	if (smbus_sw_wait_ack())
	{
		err = 5;
		goto ret;
	}

	/* restart condition */
	smbus_sw_start();

	/* send slave address(read) */
	smbus_sw_write_byte((slave_addr << 0) + 1);
	if (smbus_sw_wait_ack())
	{
		err = 6;
		goto ret;
	}

	delay_us(100);
	for (; i < len - 1; i++)
	{
		/* read data */
		*(buff + i) = smbus_sw_read_byte();
		smbus_sw_answer_ack();
		delay_us(100);
	}
	/* read data */
	*(buff + i) = smbus_sw_read_byte();
	smbus_sw_nack();

ret:
	/* stop condition */
	smbus_sw_stop();

	return err;
}
