/*
 * PCA9548A.c
 *
 *  Created on: Jul 24, 2025
 *      Author: Anders Eriksen
 */

#include "PCA9548A.h"

/*
 * Channel selection: The switch has a control register which is written to or read
 * by reading/transmitting to  the device at its address.
 */

uint8_t read_switch_control(I2Cdriver * comm){

	if (HAL_I2C_Master_Receive(comm->handle, SWITCH_ADDRESS << 1, comm->i2c_buff, 1, HAL_MAX_DELAY) != HAL_OK){
		return 2; // I2C error
	} else {
		return *comm->i2c_buff;
	}

}

uint8_t set_switch_control(I2Cdriver * comm, uint8_t ch) {

	comm->i2c_rx_buff = ch;

	if (HAL_I2C_Master_Transmit(comm->handle, (uint16_t)(SWITCH_ADDRESS << 1), &comm->i2c_rx_buff, 1, HAL_MAX_DELAY) != HAL_OK) {
		return 2; // I2C error
	} else {
		if (read_switch_control(comm) == ch) {
			return 0;
		} else {
			// control register error
			return 1;
		}

	}


}

uint8_t identify_switch(I2Cdriver * comm) {

	if (HAL_I2C_IsDeviceReady(comm->handle, (uint16_t)(SWITCH_ADDRESS << 1), 3, 5) != HAL_OK) {
		return 0;
	} else {
		return 1;
	}
}

