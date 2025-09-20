/*
 * i2c_driver.c
 *
 *  Created on: Jun 18, 2025
 *      Author: Anders Eriksen
 */

#include "i2c_driver.h"
#include "PCA9548A.h"

void scan_i2c(I2Cdriver * comm, SHT45 * sensors, CRC_HandleTypeDef * hcrc) {
	comm->n_devices = 0; // Reset comm bus data

	for (uint8_t i = 0; i < 4; i++) {
		set_switch_control(comm, 1 << i);	// connect one i2c channel at a time
		if (HAL_I2C_IsDeviceReady(comm->handle, (uint16_t)(0x44 << 1), 3, 5) == HAL_OK) {
			// if sensor present on channel
			sensors[i].address = 0x44; 	// redundant, all sensors have address 0x44
			sensors[i].ID = i + 1;		// corresponds to channel number (1, 2, 3, 4)
			read_SHT45_SN(comm, &sensors[i], hcrc); // Get serial number
			comm->n_devices++;			// Update sensor count
			comm->devices[i] = 0x44;	// Redundant
		}
	}
}

uint8_t read_SHT45(SHT45 * sensor, I2Cdriver * comm, CRC_HandleTypeDef * hcrc, uint8_t heat){
	// Read the data from SHT45

	/*
	Return error codes:
	0: No error
	1: I2C Transmit Measurement Request failed
	2: I2C Receive Measurement failed
	3: CRC Temperature failure
	4: CRC RH failure
	*/

	uint8_t addr = sensor->address; 	// 0x44 for all sensors

	// Some commands from SHT45 datasheet
	uint8_t cmd_reset = 0x94;
	uint8_t cmd_measure = 0xFD;
	uint8_t cmd_SN = 0x89;

	if (heat) {
		// cmd_measure = 0x32; 	//200mW for 0.1 s
		// cmd_measure = 0x24; 	//110mW for 0.1 s
		cmd_measure = 0x15; 	//20mW for 0.1 s
	}

	uint32_t i2c_error = 0;

	// Measurement request
	if (HAL_I2C_Master_Transmit(comm->handle, (uint16_t)(addr << 1), &cmd_measure, 1, I2C_TIMEOUT_DURATION) == HAL_OK) {

		if(heat) {
			HAL_Delay(100); // Heating for 100ms, wait it out
		}

		HAL_Delay(15); 		// wait for sensor to do reading

		if (HAL_I2C_Master_Receive(comm->handle, (uint16_t)(addr << 1), comm->i2c_buff, 6, I2C_TIMEOUT_DURATION) != HAL_OK) {
				i2c_error = 2;
		}

	} else {
		i2c_error = 1;
	}

	if(i2c_error) {
		// leave function returning error message
		return i2c_error;
	}


	// If i2c successfull, convert received data to real values
	uint16_t t_ticks = (comm->i2c_buff[0] << 8) + comm->i2c_buff[1];
	uint8_t crc_T = HAL_CRC_Calculate(hcrc, comm->i2c_buff, 2);
	if (crc_T == comm->i2c_buff[2]) {
		float t_degC = -45 + 175 * (float)t_ticks/65535;
		sensor->temperature = t_degC;
	} else {
		sensor->temperature = 0xFF;
		i2c_error = 3;
	}

	uint16_t rh_ticks = (comm->i2c_buff[3] << 8) + comm->i2c_buff[4];
	uint8_t crc_RH = HAL_CRC_Calculate(hcrc, comm->i2c_buff + 3 , 2);
	if (crc_RH == comm->i2c_buff[5]) {
		float rh_pRH = 125 * (float)rh_ticks / 65535 - 6;

			if (rh_pRH > 100) {
				rh_pRH = 100;
			} else if (rh_pRH < 0) {
				rh_pRH = 0;
			}
			sensor->RH = rh_pRH;
	} else {
		sensor->RH = 0xFF;
		i2c_error = 4;
	}

	return i2c_error;

}

uint8_t read_SHT45_SN(I2Cdriver * comm, SHT45 * sensor, CRC_HandleTypeDef * hcrc) {

	/* SHT45s are shipped with a 32 bit serial number in memory.
	 *
	 * Error codes:
	 * 0: No error
	 * 1: I2C Transmit SN request error
	 * 2: I2C Receive SN error
	 * 3: CRC SN error
	 */
	uint8_t cmd = 0x89;
	uint32_t SN = 123;	// Default garbage value

	// Request SN data
	if(HAL_I2C_Master_Transmit(comm->handle, (uint16_t)(0x44 << 1), &cmd, 1, I2C_TIMEOUT_DURATION) != HAL_OK){
		return 2; // I2C transmit error
	}

	HAL_Delay(15); // Sensor thinking time

	// Retrieve SN data
	if (HAL_I2C_Master_Receive(comm->handle, (uint16_t)(0x44 << 1), comm->i2c_buff, 6, I2C_TIMEOUT_DURATION) == HAL_OK){
		// check validity of data
		uint8_t crc = HAL_CRC_Calculate(hcrc, comm->i2c_buff, 2);
		if (crc == comm->i2c_buff[2]) {
			sensor->SN = (*(comm->i2c_buff) << 3*8) + (*(comm->i2c_buff + 1) << 2*8) + (*(comm->i2c_buff + 3) << 8) + (*(comm->i2c_buff + 4));
		} else {
			return 3; //CRC error
		}
	} else {
		return 2; //I2C receive error
	}

	return 0;
}

uint8_t reset_SHT45(I2C_HandleTypeDef * hi2c){
  // Soft reset of sensor

  uint8_t addr = 0x44;
  uint8_t cmd_reset = 0x94;

  if(HAL_I2C_Master_Transmit(hi2c, addr << 1, &cmd_reset, 1, I2C_TIMEOUT_DURATION) != HAL_OK) {
    return 1; // I2C error
  }

  return 0;


}

