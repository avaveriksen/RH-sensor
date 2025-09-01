/*
 * i2c_driver.c
 *
 *  Created on: Jun 18, 2025
 *      Author: Anders Eriksen
 */

#include "i2c_driver.h"
#include "PCA9548A.h"

void scan_i2c(I2Cdriver * comm, SHT45 * sensors, CRC_HandleTypeDef * hcrc) {
	comm->n_devices = 0;

	for (uint8_t i = 0; i < 4; i++) {
		set_switch_control(comm, 1 << i);
		if (HAL_I2C_IsDeviceReady(comm->handle, (uint16_t)(0x44 << 1), 3, 5) == HAL_OK) {
			sensors[i].address = 0x44;
			sensors[i].ID = i + 1;
			sensors[i].SN = read_SHT45_SN(comm, hcrc);
			comm->n_devices++;
			comm->devices[i] = 0x44;
		}
	}
}

uint32_t read_SHT45(SHT45 * sensor, I2Cdriver * comm, CRC_HandleTypeDef * hcrc, uint8_t heat){
	// Read the data from SHT45
	// return 2: 	I2C error.
	// return 1:	Stale data.
	// return 0:	OK, fresh data.

	uint8_t addr = sensor->address;

	uint8_t cmd_reset = 0x94;
	uint8_t cmd_measure = 0xFD;
	uint8_t cmd_SN = 0x89;

	if (heat) {
		cmd_measure = 0x32; //200mW for 0.1 s
	}

	uint32_t i2c_error = 0;

	// Measurement request
	if (HAL_I2C_Master_Transmit(comm->handle, (uint16_t)(addr << 1), &cmd_measure, 1, I2C_TIMEOUT_DURATION) == HAL_OK) {
		HAL_Delay(15);
		if (HAL_I2C_Master_Receive(comm->handle, (uint16_t)(addr << 1), comm->i2c_buff, 6, I2C_TIMEOUT_DURATION) != HAL_OK) {
				i2c_error = comm->handle->ErrorCode;
			}

	} else {
		i2c_error = comm->handle->ErrorCode;
	}


	// Convert to real values
	uint16_t t_ticks = (comm->i2c_buff[0] << 8) + comm->i2c_buff[1];
	uint8_t crc_T = HAL_CRC_Calculate(hcrc, comm->i2c_buff, 2);
	if (crc_T == comm->i2c_buff[2]) {
		float t_degC = -45 + 175 * (float)t_ticks/65535;
		sensor->temperature = t_degC;
	} else {
		sensor->temperature = 0xFF;
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
	}

	return 0;

}

uint32_t read_SHT45_SN(I2Cdriver * comm, CRC_HandleTypeDef * hcrc ) {

	// SHT45s are shipped with a serial number in memory.
	uint8_t cmd = 0x89;
	uint32_t SN = 0;

	// Request SN data
	if(HAL_I2C_Master_Transmit(comm->handle, (uint16_t)(0x44 << 1), &cmd, 1, I2C_TIMEOUT_DURATION) != HAL_OK){
		return 2; // I2C error
	}

	HAL_Delay(15);

	// Retrieve SN data
	if (HAL_I2C_Master_Receive(comm->handle, (uint16_t)(0x44 << 1), comm->i2c_buff, 6, I2C_TIMEOUT_DURATION) == HAL_OK){
		uint8_t crc = HAL_CRC_Calculate(hcrc, comm->i2c_buff, 2);
		if (crc == comm->i2c_buff[2]) {
			SN = (*(comm->i2c_buff) << 3*8) + (*(comm->i2c_buff + 1) << 2*8) + (*(comm->i2c_buff + 3) << 8) + (*(comm->i2c_buff + 4));
		} else {
			return 1; //CRC error
		}
	} else {
		return 2; //I2C error
	}

	return SN;
}

void sensor_power(uint8_t state){
  // Enable or diable power to sensor(s). All sensors are powered by the same source.
  if(state){
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
  }
}

uint8_t reset_SHT45(I2C_HandleTypeDef * hi2c, char variant){
  // Soft reset of sensor

  uint8_t addr = 0;

  if(variant == 'A') {
    addr = 0x44;
  } else if (variant == 'B') {
    addr = 0x45;
  } else {
    //error
  }

  if(HAL_I2C_Master_Transmit(hi2c, addr << 1, 0x94, 1, I2C_TIMEOUT_DURATION) != HAL_OK) {
    return 2; // I2C error
  }


}

