/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c_driver.h"
#include "SHT45_driver.h"
#include "serial_driver.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

I2Cdriver i2c_bus;
SHT45 sensors[4];	// Four Sensors, two for RH+T heated, two for just T, non heated

uint8_t uart_rx[32];

uint8_t stream = 0;
uint8_t scan = 0;

volatile float data[4][2];	// Data array, four sensors, two kinds of data (RH, T)

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3); // Visual cue that timer is functional

  // Measurements and streaming
  for (uint8_t i = 0; i < i2c_bus.n_devices; i++) {
    uint8_t addr = i2c_bus.devices[i];
    // Read data from HYT271
    /*
    HYT_RM_status = request_measurements(&hi2c1, addr, i2c_buff);
    HAL_Delay(100);
    HYT_Read_status = read_HYT271(&hi2c1, addr, i2c_buff);
    convert_data(i2c_buff, data[i]);
    */

    if (i < 2) {
    	if(sensors[i].RH > 95) {
    		heat_and_read_SHT45(&sensors[i], &i2c_bus);
    	} else {
    		read_SHT45(&sensors[i], &i2c_bus);
    	}
    }

    if (stream) {
    	for (uint8_t i = 0; i < i2c_bus.n_devices;i++) {
    		transmit_SHT45(&huart2, &sensors[i]);
    	}

      data_transfer_concluded_message(&huart2);
    }
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

???LINES MISSING
???LINES MISSING
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
