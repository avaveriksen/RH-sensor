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

uint8_t request_measurements(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *buff);
uint8_t read_HYT271(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *buff);
void convert_data(uint8_t *buff, float *data);
uint8_t scan_i2c(I2C_HandleTypeDef *hi2c, uint8_t * devices);

#endif /* INC_I2C_DRIVER_H_ */
