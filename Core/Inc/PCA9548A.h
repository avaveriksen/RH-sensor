/*
 * PCA9548A.h
 *
 *  Created on: Jul 24, 2025
 *      Author: Anders Eriksen
 */

#ifndef SRC_PCA9548A_H_
#define SRC_PCA9548A_H_

#define SWITCH_ADDRESS 0x70
#define I2C_CH1 1
#define I2C_CH2 2
#define I2C_CH3 4
#define I2C_CH4 8

#include "main.h"
#include "i2c_driver.h"

uint8_t read_switch_control(I2Cdriver * comm);
uint8_t set_switch_control(I2Cdriver * comm, uint8_t ch);
uint8_t identify_switch(I2Cdriver * comm);

#endif /* SRC_PCA9548A_H_ */
