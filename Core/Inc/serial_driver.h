/*
 * serial_driver.h
 *
 *  Created on: Jun 18, 2025
 *      Author: Anders Eriksen
 */

#ifndef INC_SERIAL_DRIVER_H_
#define INC_SERIAL_DRIVER_H_

#include <stdint.h>
#include "main.h"
#include "stm32f3xx_hal.h"
#include "i2c_driver.h"

extern uint8_t stream;
extern uint8_t rx_buff[];
extern uint8_t scan;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

uint8_t transmit_HYT(UART_HandleTypeDef *huart, float *data, uint8_t *tx_buff, uint8_t i);
uint8_t broadcast_devices(UART_HandleTypeDef *huart, uint8_t * devices, uint8_t n_devices);

#endif /* INC_SERIAL_DRIVER_H_ */
