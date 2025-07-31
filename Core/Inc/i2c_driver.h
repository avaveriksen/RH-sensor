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
	volatile uint8_t i2c_rx_buff[1];
	volatile uint8_t n_devices;		// number of sensors
	volatile uint8_t devices[4]; 	// addresses of sensors
} I2Cdriver;

typedef struct{
	uint32_t SN;		// Serial number
	uint8_t address;
	uint8_t ID;
	float temperature;
	float RH;
} SHT45;

uint8_t scan_i2c(I2Cdriver * comm, SHT45 * sensors);
uint8_t manual_I2C(I2C_HandleTypeDef *hi2c, uint8_t dir, uint8_t addr, uint8_t * rx, uint8_t tx, uint8_t len);
uint32_t read_SHT45(SHT45 * sensor, I2Cdriver * comm);
uint8_t read_SHT45_SN(SHT45 * sensor, I2Cdriver * comm);
void sensor_power(uint8_t state);
uint8_t reset_SHT45(I2C_HandleTypeDef * hi2c, char variant);
uint8_t heat_and_read_SHT45(SHT45 * sensor, I2Cdriver * comm);
uint8_t CRC_check(uint16_t message, uint8_t checksum);

#endif /* INC_I2C_DRIVER_H_ */
