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

extern uint8_t stream;
extern uint8_t uart_rx[];
extern uint8_t scan;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
uint8_t transmit_SHT45(UART_HandleTypeDef * huart, SHT45 * sensor);
uint8_t broadcast_devices(UART_HandleTypeDef *huart, SHT45 * sensors);
uint8_t data_transfer_concluded_message(UART_HandleTypeDef * huart);

#endif /* INC_SERIAL_DRIVER_H_ */
