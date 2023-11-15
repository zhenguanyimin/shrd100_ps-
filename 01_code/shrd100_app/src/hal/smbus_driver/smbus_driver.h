#ifndef BATTERY_DRIVER_H
#define BATTERY_DRIVER_H

//#include "common.h"
#include "xgpio.h"
/* pins define */
#define SMBUS_SCL_GPIO_PIN	2
#define SMBUS_SDA_GPIO_PIN	1

extern XGpio Gpio;

#define SCL_DOWN			XGpio_DiscreteWrite(&Gpio, SMBUS_SCL_GPIO_PIN, 0)
#define SCL_UP			    XGpio_DiscreteWrite(&Gpio, SMBUS_SCL_GPIO_PIN, 1)
							
#define SDA_DOWN			XGpio_DiscreteWrite(&Gpio, SMBUS_SDA_GPIO_PIN, 0)
#define SDA_UP			    XGpio_DiscreteWrite(&Gpio, SMBUS_SDA_GPIO_PIN, 1)

#define SDA_OUT				XGpio_SetDataDirection(&Gpio, SMBUS_SDA_GPIO_PIN, 0);
#define SDA_IN			    XGpio_SetDataDirection(&Gpio, SMBUS_SDA_GPIO_PIN, 1);
#define READ_SDA			XGpio_DiscreteRead(&Gpio, SMBUS_SDA_GPIO_PIN)

/* global function define */
void smbus_sw_init(void);
uint8_t smbus_sw_write_registers(uint8_t slave_addr, uint8_t register_addr, uint8_t *buff, uint8_t len);
uint8_t smbus_sw_read_registers(uint8_t slave_addr, uint8_t register_addr, uint8_t *buff, uint8_t len);
uint8_t smbus_sw_read_register(uint8_t slave_addr, uint8_t register_addr, uint8_t *buff, uint8_t len);

#endif /* BATTERY_DRIVER_H */
