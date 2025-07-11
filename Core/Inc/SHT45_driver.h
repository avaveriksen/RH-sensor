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

uint8_t read_SHT45(I2C_handleTypeDef *hi2c, char variant, uint8_t *buff, float * data_subarray);
uint8_t read_SHT45_SN(I2C_HandleTypeDef *hi2c, char variant, uint32_t * SN_buff);
void sensor_power(uint8_t state);
uint8_t heat_and_read_SHT45(I2C_HandleTypeDef * hi2c, char variant);
uint8_t CRC_check(uint16_t message, uint8_t checksum);

#endif /* INC_SHT45_DRIVER_H_ */
