/*
 * i2c_driver.h
 *
 *  Created on: Jun 18, 2025
 *  Author: Anders Eriksen
 */

#ifndef INC_I2C_DRIVER_H_
#define INC_I2C_DRIVER_H_

#include <stdint.h>
#include "main.h"
#include "stm32f3xx_hal.h"

typedef struct {
	I2C_HandleTypeDef * handle;		// I2C periph. handle
	volatile uint8_t message_length;// num of bytes for transmission or receiving
	volatile uint8_t i2c_buff[6];	// buffer for received I2C bytes
	volatile uint8_t n_devices;		// number of sensors
	volatile uint8_t devices[4]; 	// addresses of sensors
} I2Cdriver;

uint8_t scan_i2c(I2C_HandleTypeDef *hi2c, volatile uint8_t * devices);

#endif /* INC_I2C_DRIVER_H_ */
