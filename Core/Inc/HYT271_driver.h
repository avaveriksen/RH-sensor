/*
 * HYT271_driver.h
 *
 *  Created on: July 9th, 2025
 *  Author: Anders Eriksen
 */

#ifndef INC_HYT271_DRIVER_H_
#define INC_HYT271_DRIVER_H_

#include <stdint.h>
#include "main.h"
#include "stm32f3xx_hal.h"

typedef struct{
	volatile uint8_t HYT_RM_status;
	volatile uint8_t HYT_read_status;
} HYT271;

uint8_t request_measurements(I2C_HandleTypeDef *hi2c, uint8_t addr, volatile uint8_t *buff);
uint8_t read_HYT271(I2C_HandleTypeDef *hi2c, uint8_t addr, volatile uint8_t *buff);
void convert_data(volatile uint8_t *buff, volatile float *data);

#endif /* INC_HYT271_DRIVER_H_ */
