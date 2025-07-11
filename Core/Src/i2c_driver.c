/*
 * i2c_driver.c
 *
 *  Created on: Jun 18, 2025
 *      Author: Anders Eriksen
 */

#include "i2c_driver.h"


/*
 TO READ FROM HYT271:

 Step 1: Request measurement
 Step 2: Wait for conversion (<100 ms)
 Step 3: Read raw data.
 Step 4: Convert data.
*/

uint8_t request_measurements(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *buff) {
	// Send Measurement Request (MR)
	// Remember to put in delay to allow for conversion time
		if (HAL_I2C_Master_Transmit(hi2c, addr << 1, buff, 1, HAL_MAX_DELAY) != HAL_OK) {
			// We should probably go to error handler here
			return 1; // I2C Transmit error
		} else {
			return 0;
		}
}

uint8_t read_HYT271(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *buff){
	// Read the data from HYT271
	// return 2: 	I2C error.
	// return 1:	Stale data.
	// return 0:	OK, fresh data.
	if (HAL_I2C_Master_Receive(hi2c, addr << 1, buff, 4, HAL_MAX_DELAY) != HAL_OK) {
	   return 2;
	} else {
		// Check for stale data
		uint8_t status = (*buff & 0xC0) >> 6;
		if (status & 0x02) {
			return 1;
		} else {
			return 0;
		}
	}
}

void convert_data(uint8_t *buff, float *data){
	// Convert raw data to correct units and store in data buffer
	uint16_t hum_raw = ((*buff & 0x3F) << 8) | *(buff + 1);
	float humidity = hum_raw * (100.0 / 16383.0);

	uint16_t temp_raw = ((*(buff + 2) << 8) | (*(buff + 3) & 0xFC)) >> 2;
	float temperature = (temp_raw * 165.0f) / 16383.0f - 40.0f;

	*data = humidity;
	*(data+1) = temperature;
}

uint8_t scan_i2c(I2C_HandleTypeDef *hi2c, uint8_t * devices) {
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
