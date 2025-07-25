/*
 * PCA9548A.c
 *
 *  Created on: Jul 24, 2025
 *      Author: Anders Eriksen
 */

#include "PCA9548A.h"

/*
 * Channel selection: The switch has a control register which is written to or read
 * by reading/transmitting to the device at its address.
 */

uint8_t read_switch_control(I2Cdriver * comm){

	if (HAL_I2C_Master_Receive(comm->handle, SWITCH_ADDRESS << 1, comm->i2c_buff, 1, HAL_MAX_DELAY) != HAL_OK){
		return 2; // I2C error
	} else {
		return *comm->i2c_buff;
	}
}

uint8_t set_switch_control(I2Cdriver * comm, uint8_t ctrl_byte) {

	if (HAL_I2C_Master_Transmit(comm->handle, SWITCH_ADDRESS, ctrl_byte, 1, HAL_MAX_DELAY) != HAL_OK) {
		return 2; // I2C error
	} else {
		return 0;
	}
}

uint8_t scan_switch(I2Cdriver * comm) {

	uint8_t identified = 0;

	for (uint8_t i = 0; i < 4; i++) {
		set_switch_control(comm, 1 << i);
		identified += scan_i2c(comm->handle, comm->devices + identified);
	}

	return identified;
}

uint8_t identify_switch(I2Cdriver * comm) {

	uint8_t switch_ack = read_switch_control(comm);

	if (switch_ack == 0) {
		return 1;
	} else {
		return 0;
	}
}

