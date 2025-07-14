/*
 * i2c_driver.c
 *
 *  Created on: Jun 18, 2025
 *      Author: Anders Eriksen
 */

#include "i2c_driver.h"

uint8_t scan_i2c(I2C_HandleTypeDef *hi2c, volatile uint8_t * devices) {
	// Scan whole range of i2c addresses to identify devices
	uint8_t identified = 0; // number of identified devices

	for (uint8_t i = 0; i < 128; i++) {
		if (HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(i<<1), 3, 5) == HAL_OK) {
			*(devices + identified) = i;
			identified++;
		}
	}

	return identified;
}

//Untested
uint8_t manual_I2C(I2C_HandleTypeDef *hi2c, uint8_t dir, uint8_t addr, uint8_t * rx, uint8_t tx, uint8_t len) {
	// dir = 1, transmit, else receive

	if (dir) {
		if (HAL_I2C_Master_Transmit(hi2c, addr << 1, rx, len, HAL_MAX_DELAY) != HAL_OK) {
			// We should probably go to error handler here
			return 1; // I2C Transmit error
		} else {
			return 0;
		}
	} else {
		if (HAL_I2C_Master_Receive(hi2c, addr << 1, tx, len, HAL_MAX_DELAY) != HAL_OK) {
		   return 1; // I2C transmit error
		}
	}
}
