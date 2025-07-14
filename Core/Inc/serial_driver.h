/*
 * serial_driver.h
 *
 *  Created on: Jun 18, 2025
 *      Author: Anders Eriksen
 */

#ifndef INC_SERIAL_DRIVER_H_
#define INC_SERIAL_DRIVER_H_

#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "stm32f3xx_hal.h"
#include "i2c_driver.h"
#include "SHT45_driver.h"

extern uint8_t stream;
extern uint8_t uart_rx[];
extern uint8_t scan;

typedef struct {
	UART_HandleTypeDef * handle;
	volatile uint8_t rx_buff[32];
	volatile uint8_t tx_buff[32];

} serial_driver;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
uint8_t transmit_SHT45(UART_HandleTypeDef * huart, SHT45 * sensor);
uint8_t transmit_HYT(UART_HandleTypeDef *huart, volatile float *data, volatile uint8_t *tx_buff, uint8_t i);
uint8_t broadcast_devices(UART_HandleTypeDef *huart, volatile uint8_t * devices, volatile uint8_t n_devices);

#endif /* INC_SERIAL_DRIVER_H_ */
