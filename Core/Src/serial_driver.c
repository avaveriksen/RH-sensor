/*
 * serial_driver.c
 *
 *  Created on: Jun 18, 2025
 *      Author: Anders Eriksen
 */

#include "serial_driver.h"


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Function to handle incoming serial messages

	uint8_t ack[] = "#!#0#\n";
	uint8_t ack_stream[] = "#A!#\n";
	uint8_t ack_stream_stop[] = "S!#\n";

	// do something with the data
	if (strcmp(rx_buff, "#?#\n") == 0) {
		HAL_UART_Transmit(huart, ack, strlen(ack), HAL_MAX_DELAY);
	} else if(strcmp(rx_buff, "#A#\n") == 0) {
		// set stream flag
		stream = 1;
		HAL_UART_Transmit(huart, ack_stream, strlen(ack), HAL_MAX_DELAY);
	} else if (strcmp(rx_buff, "#S#\n") == 0) {
		// reset stream flag (stop streaming)
		stream = 0;
		HAL_UART_Transmit(huart, ack_stream_stop, strlen(ack), HAL_MAX_DELAY);
	} else if (strcmp(rx_buff, "#s#\n") == 0) {
		// set scan flag
		scan = 1;
	}

	HAL_UART_Receive_IT(huart, rx_buff, 4);
}

// Transmit HYT271 sensor data
uint8_t transmit_HYT(UART_HandleTypeDef *huart, float *data, uint8_t *tx_buff, uint8_t addr) {
	sprintf(tx_buff,"#D#%02X#%.2f#%.2f#\n", addr, *data, *(data+1));
	if(HAL_UART_Transmit(huart, tx_buff, strlen(tx_buff), HAL_MAX_DELAY) != HAL_OK){
		return 1; // UART transmission error
	} else {
		return 0;
	}
}

uint8_t broadcast_devices(UART_HandleTypeDef *huart, uint8_t * devices, uint8_t n_devices) {
	uint8_t tx[19] = "#s!#0#00#00#00#00#\n";
	uint8_t appendix[4];

	sprintf(tx, "#s!#%d", n_devices);

	for (uint8_t i = 0; i < n_devices; i++) {

		sprintf(appendix, "#%02X", *(devices + i));	// Set up appendix with device address, format as hex
		strcat(tx, appendix); 						// concatenate 'appendix' to 'tx'
	}

	strcat(tx, "#\n");

	if(HAL_UART_Transmit(huart, tx, strlen(tx), HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	} else {
		return 0;
	}
}


