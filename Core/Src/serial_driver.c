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

	// Return messages
	uint8_t ack[] = "#!!#0#\n";
	uint8_t ack_stream[] = "#A!#\n";
	uint8_t ack_stream_stop[] = "#S!#\n";
	uint8_t ack_heater_on[] = "#H!#\n";
	uint8_t ack_heater_off[] = "#h!#\n";

	// Incoming message decoder
	if (strcmp(uart_rx, "#?#\n") == 0) {
		// Check if transceiver online
		HAL_UART_Transmit(huart, ack, strlen(ack), HAL_MAX_DELAY);
	} else if(strcmp(uart_rx, "#A#\n") == 0) {
		// Command to set stream flag
		stream = 1;
		HAL_UART_Transmit(huart, ack_stream, strlen(ack_stream), HAL_MAX_DELAY);
	} else if (strcmp(uart_rx, "#S#\n") == 0) {
		// Command to reset stream flag (stop streaming)
		stream = 0;
		HAL_UART_Transmit(huart, ack_stream_stop, strlen(ack_stream_stop), HAL_MAX_DELAY);
	} else if (strcmp(uart_rx, "#s#\n") == 0) {
		// Command to set scan flag
		scan = 1;
	} else if (strcmp(uart_rx, "#H#\n") == 0) {
		heater = 1;
		HAL_UART_Transmit(huart, ack_heater_on, strlen(ack_heater_on), HAL_MAX_DELAY);
	} else if (strcmp(uart_rx, "#h#\n") == 0) {
		heater = 0;
		HAL_UART_Transmit(huart, ack_heater_off, strlen(ack_heater_on), HAL_MAX_DELAY);
	}

	//Receive interrupt must be reenabled after RxCallback
	HAL_UART_Receive_IT(huart, uart_rx, 4);
}

uint8_t transmit_SHT45(UART_HandleTypeDef * huart, SHT45 * sensor){
	/* Transmit sensor data
	 *
	 * Error codes:
	 * 0: no error
	 * 1: UART transmit failed
	 */

	char tx[21] = "#x!#0#000.00#000.00#\n"; // dummy string
	char appendix[7];

	// Encode data in string to be transmitted
	sprintf(tx, "#D#%d", sensor->ID); 								 //'D' signifies data message
	snprintf(appendix, sizeof(appendix) + 1, "#%07.3f", sensor->RH); // read in RH data in appendix
	strcat(tx, appendix); 											 // concatenate 'appendix' to 'tx'
	snprintf(appendix, sizeof(appendix) + 1, "#%07.3f", sensor->temperature);
	strcat(tx, appendix);

	strcat(tx, "#\n"); // Finish string with '\n' character

	// Transmit data
	if(HAL_UART_Transmit(huart, tx, strlen(tx), HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t data_transfer_concluded_message(UART_HandleTypeDef * huart) {
	/* Any data transfer must be concluded with a transfer concluded message */

	uint8_t tx[] = "#D!#\n"; // 'D!' signifies data transmission concluded

	if(HAL_UART_Transmit(huart, tx, strlen(tx), HAL_MAX_DELAY) != HAL_OK) {
		return 1; // UART transmission failed

	} else {
		return 0;
	}
}

uint8_t broadcast_devices(UART_HandleTypeDef *huart, SHT45 * sensors) {
	/* Transmit info on connected sensors
	 *
	 * Error codes:
	 * 0: No error.
	 * 1: UART Transmit error
	 */
	char tx[40] = {};
	char appendix[32] = {};

	for (uint8_t i = 0; i < 4; i++) {

		if (sensors[i].address == 0x44) {
			sprintf(tx, "#s!#%d#", i + 1);
			sprintf(appendix, "%lu#\n", sensors[i].SN);
			strcat(tx, appendix);

			if(HAL_UART_Transmit(huart, &tx, strlen(tx), HAL_MAX_DELAY) != HAL_OK) {
				return 1;
			}

			HAL_Delay(10);
		}
	}

	return 0;
}



