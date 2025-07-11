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

#endif /* INC_SHT45_DRIVER_H_ */
