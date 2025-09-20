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
	/* Read which channels are connected presently.
	 *
	 * Error codes:
	 * 0: No error
	 * 1: I2C Receive channel information error
	 */

	if (HAL_I2C_Master_Receive(comm->handle, SWITCH_ADDRESS << 1, comm->i2c_buff, 1, HAL_MAX_DELAY) == HAL_OK){
		comm->channels = *comm->i2c_buff;
		//return *comm->i2c_buff;
	} else {
		return 1; // I2C error
	}

	return 0;

}

uint8_t set_switch_control(I2Cdriver * comm, uint8_t ch) {
	/* Set which i2c channel is connected
	 *
	 * 0: No error
	 * 1: I2C Transmit channel config error
	 * 2: Control register value conflicts with intended channel config
	 */

	comm->i2c_rx_buff = ch; // To be transmitted to switch

	if (HAL_I2C_Master_Transmit(comm->handle, (uint16_t)(SWITCH_ADDRESS << 1), &comm->i2c_rx_buff, 1, HAL_MAX_DELAY) != HAL_OK) {
		return 2; // I2C error
	} else {
		read_switch_control(comm);

		if (comm->channels == ch) {
			return 0;
		} else {
			// control register error
			return 2;
		}
	}
}


