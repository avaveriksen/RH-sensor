#include "SHT45_driver.h"
#include "i2c_driver.h"
#include "main.h"

uint8_t read_SHT45(SHT45 * sensor, I2Cdriver * comm){
	// Read the data from SHT45
	// return 2: 	I2C error.
	// return 1:	Stale data.
	// return 0:	OK, fresh data.

	uint8_t addr = sensor->address;
	I2C_HandleTypeDef * comm_handle = comm->handle;
	volatile uint8_t * comm_buff = comm->i2c_buff;

	// Measurement request
	if (HAL_I2C_Master_Transmit(comm_handle, addr << 1, 0xFD, 2, HAL_MAX_DELAY) != HAL_OK) {
		return 2; // I2C error
	}

	HAL_Delay(10);

	// Read measurement
	if (HAL_I2C_Master_Receive(comm_handle, addr << 1, comm_buff, 6, HAL_MAX_DELAY) != HAL_OK) {
		return 2; // I2C error
	}

	// Convert to real values
	uint16_t t_ticks = comm_buff[0] << 8 + comm_buff[1];
	uint8_t checksum_t = comm_buff[2];
	  if (!CRC_check(t_ticks, checksum_t)) {
		return 3; // CRC error
	  }

		uint16_t rh_ticks = comm_buff[3] << 8 + comm_buff[4];
		uint8_t checksum_rh = comm_buff[5];
	  if (!CRC_check(rh_ticks, checksum_rh)){
		return 3; // CRC error
	  }

	float t_degC = -45 + 175 * t_ticks/65535;
	float rh_pRH = 125 * rh_ticks / 65535 - 6;

	if (rh_pRH > 100) {
		rh_pRH = 100;
	} else if (rh_pRH < 0) {
		rh_pRH = 0;
	}

	// store data
	sensor->RH = rh_pRH;
	sensor->temperature = t_degC;

	return 0;

}

uint8_t read_SHT45_SN(SHT45 * sensor, I2Cdriver * comm) {
	// SHT45s are shipped with a serial number in memory.

	uint8_t addr = sensor->address;
	// Request SN data
	if(HAL_I2C_Master_Transmit(comm, addr << 1, 0x89, 1, HAL_MAX_DELAY) != HAL_OK){
		return 2; // I2C error
	}

	// Retrieve SN data
	if (HAL_I2c_Master_Receive(comm, addr << 1, sensor->SN, 4, HAL_MAX_DELAY) != HAL_OK){
		return 2; //I2C error
	}

	return 0;
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
  
  if(HAL_I2C_Master_Transmit(hi2c, addr << 1, 0x94, 1, HAL_MAX_DELAY) != HAL_OK) {
    return 2; // I2C error
  }
  

}

uint8_t heat_and_read_SHT45(SHT45 * sensor, I2Cdriver * comm) {

	I2C_HandleTypeDef * comm_handle = comm->handle;
    uint8_t addr = sensor->address;

  // 0x24 is 110mW for 0.1 second
  if(HAL_I2C_Master_Transmit(comm_handle, addr << 1, 0x24, 1, HAL_MAX_DELAY) != HAL_OK) {
    return 2; // I2C error
  }

  read_SHT45(sensor, comm);

}

uint8_t CRC_check(uint16_t message, uint8_t checksum){
  // Calculates the Cyclic Redundancy Checksum with polynomial 0x31
  uint8_t remainder = message % 0x31;

  if (remainder == checksum) {
    return 1; // success
  } else {
    return 0; // Does NOT check out
  }
}






