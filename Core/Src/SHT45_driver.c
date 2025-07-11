#include "SHT45_driver.h"

uint8_t read_SHT45(I2C_HandleTypeDef *hi2c, char variant, uint8_t *buff, float * data_subarray){
	// Read the data from SHT45
	// return 2: 	I2C error.
	// return 1:	Stale data.
	// return 0:	OK, fresh data.

	uint8_t addr = 0;


	// Determine address of sensor
	if (variant = 'A') {
		addr = 0x44;
	} else if (variant = 'B') {
		addr = 0x45;
	}

	// Measurement request
	if (HAL_I2C_Master_Transmit(hi2c, addr << 1, 0xFD, 2, HAL_MAX_DELAY) != HAL_OK) {
		return 2; // I2C error
	}

	HAL_DELAY(10);

	// Read measurement
	if (HAL_I2C_Master_Receive(hi2c, addr << 1, buff, 6, HAL_MAX_DELAY) != HAL_OK) {
		return 2; // I2C error
	}

	// Convert to real values
	uint16_t t_ticks = buff[0] << 8 + buff[1];
	uint8_t checksum_t = buff[2];
  if (!CRC_check(ticks, checksum_t)) {
    return 3; // CRC error
  }

	uint16_t rh_ticks = buff[3] << 8 + buff[4];
	uint8_t checksum_rh = buff[5];
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
	data_subarray[0] = rh_pRH;
	data_subarray[1] = t_degC;


	return 0;

}

uint8_t read_SHT45_SN(I2C_HandleTypeDef *hi2c, char variant, uint32_t * SN_buff) {
	// SHT45s are shipped with a serial number in memory.

	uint8_t addr = 0;
	uint8_t buff_idx = 0;

	// Determine address of sensor
	if (variant = 'A') {
		addr = 0x44;
	} else if (variant = 'B') {
		addr = 0x45;
		buff_idx = 1;
	} else {
    // error
  }

	// Request SN data
	if(HAL_I2C_Master_Transmit(hi2c, addr << 1, 0x89, 2, HAL_MAX_DELAY) != HAL_OK){
		return 2; // I2C error
	}

	// Retrieve SN data
	if (HAL_I2c_Master_Receive(hi2c, addr << 1, SN_buff[buff_idx], HAL_MAX_DELAY) != HAL_OK){
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

uint8_t heat_and_read_sht45(I2C_HandleTypeDef * hi2c, char variant) {
  
  uint8_t addr = 0;
  
  if(variant == 'A') {
    addr = 0x44; 
  } else if (variant == 'B') {
    addr = 0x45;
  } else {
    //error
  }
  
  // 0x24 is 110mW for 0.1 second

  if(HAL_I2C_Master_Transmit(hi2c, addr << 1, 0x24, 1, HAL_MAX_DELAY) != HAL_OK) {
    return 2; // I2C error
  }

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






