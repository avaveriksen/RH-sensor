/*
 * SHT45_driver.h
 *
 *  Created on: July 9th, 2025
 *  Author: Anders Eriksen
 */

#ifndef INC_SHT45_DRIVER_H_
#define INC_SHT45_DRIVER_H_

#include <stdint.h>
#include "main.h"
#include "stm32f3xx_hal.h"
#include "i2c_driver.h"

typedef struct{
	uint32_t SN;		// Serial number
	uint8_t address;
	uint8_t ID;
	float temperature;
	float RH;
} SHT45;

uint8_t read_SHT45(SHT45 * sensor, I2Cdriver * comm);
uint8_t read_SHT45_SN(SHT45 * sensor, I2Cdriver * comm);
void sensor_power(uint8_t state);
uint8_t heat_and_read_SHT45(SHT45 * sensor, I2Cdriver * comm);
uint8_t CRC_check(uint16_t message, uint8_t checksum);

#endif /* INC_SHT45_DRIVER_H_ */
