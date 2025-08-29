/*
 * PCA9548A.h
 *
 *  Created on: Jul 24, 2025
 *      Author: Anders Eriksen
 */

#ifndef SRC_PCA9548A_H_
#define SRC_PCA9548A_H_

#define SWITCH_ADDRESS 0x70

#include "main.h"
#include "i2c_driver.h"

uint8_t read_switch_control(I2Cdriver * comm);
uint8_t set_switch_control(I2Cdriver * comm, uint8_t ctrl_byte);
uint8_t identify_switch(I2Cdriver * comm);

#endif /* SRC_PCA9548A_H_ */
