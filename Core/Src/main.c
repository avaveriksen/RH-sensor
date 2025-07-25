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
float version = 0.1; // depends on hardware. v0.2 has i2c switch.

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

    if (version == 0.2) {
    	set_switch_control(&i2c_bus, 1 << i);
    }

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

	  /* USER CODE BEGIN SysInit */

	  /* USER CODE END SysInit */

	  /* Initialize all configured peripherals */
	  MX_GPIO_Init();
	  MX_I2C1_Init();
	  MX_USART2_UART_Init();
	  MX_TIM2_Init();

	  /* USER CODE BEGIN 2 */
	  HAL_UART_Receive_IT(&huart2, uart_rx, 4); // enable UART interrupt
	  HAL_TIM_Base_Start_IT(&htim2);
	  HAL_NVIC_DisableIRQ(TIM2_IRQn);

	  // Sensor initialization
	  i2c_bus.handle = &hi2c1; 	// assignment must be in a function
	  sensor_power(1); 				// Power on sensor(s)
	  i2c_bus.n_devices = scan_i2c(i2c_bus.handle, i2c_bus.devices);


	  // check if i2c switch is installed, set version


	  if(identify_switch(&i2c_bus)) {
		  version = 0.2;
	  }

	  // Identify sensors on i2c bus
	  if (version == 0.2) {
		  i2c_bus.n_devices = scan_switch(&i2c_bus);
	  }


	  /* USER CODE END 2 */

	  /* Infinite loop */
	  /* USER CODE BEGIN WHILE */
  while (1)
	  while (1)
	   {
	 	  if (scan) {
	       // scan flag is set by incoming UART message
	 		  HAL_NVIC_DisableIRQ(TIM2_IRQn);
	 		  i2c_bus.n_devices = scan_i2c(i2c_bus.handle, i2c_bus.devices);
	 		  broadcast_devices(&huart2, i2c_bus.devices, i2c_bus.n_devices);
	 		  scan = 0;
	 	  }

	 	  if (stream) {
	 		  HAL_NVIC_EnableIRQ(TIM2_IRQn);
	 	  } else {
	 		  HAL_NVIC_DisableIRQ(TIM2_IRQn);
	 	  }

	     /* USER CODE END WHILE */

	     /* USER CODE BEGIN 3 */
	   }
	   /* USER CODE END 3 */
	 }

	 /**
	   * @brief System Clock Configuration
	   * @retval None
	   */
	 void SystemClock_Config(void)
	 {
	   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	   RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	   /** Initializes the RCC Oscillators according to the specified parameters
	   * in the RCC_OscInitTypeDef structure.
	   */
	   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	   RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	   RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	   {
	     Error_Handler();
	   }

	   /** Initializes the CPU, AHB and APB buses clocks
	   */
	   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	   {
	     Error_Handler();
	   }
	   PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
	   PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
	   if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	   {
	     Error_Handler();
	   }
	 }

	 /**
	   * @brief I2C1 Initialization Function
	   * @param None
	   * @retval None
	   */
	 static void MX_I2C1_Init(void)
	 {

	   /* USER CODE BEGIN I2C1_Init 0 */

	   /* USER CODE END I2C1_Init 0 */

	   /* USER CODE BEGIN I2C1_Init 1 */

	   /* USER CODE END I2C1_Init 1 */
	   hi2c1.Instance = I2C1;
	   hi2c1.Init.Timing = 0x2000090E;
	   hi2c1.Init.OwnAddress1 = 0;
	   hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	   hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	   hi2c1.Init.OwnAddress2 = 0;
	   hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	   hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	   hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	   if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	   {
	     Error_Handler();
	   }

	   /** Configure Analogue filter
	   */
	   if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	   {
	     Error_Handler();
	   }

	   /** Configure Digital filter
	   */
	   if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
	   {
	     Error_Handler();
	   }
	   /* USER CODE BEGIN I2C1_Init 2 */

	   /* USER CODE END I2C1_Init 2 */

	 }

	 /**
	   * @brief TIM2 Initialization Function
	   * @param None
	   * @retval None
	   */
	 static void MX_TIM2_Init(void)
	 {

	   /* USER CODE BEGIN TIM2_Init 0 */

	   /* USER CODE END TIM2_Init 0 */

	   TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	   TIM_MasterConfigTypeDef sMasterConfig = {0};

	   /* USER CODE BEGIN TIM2_Init 1 */

	   /* USER CODE END TIM2_Init 1 */
	   htim2.Instance = TIM2;
	   htim2.Init.Prescaler = 7;
	   htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	   htim2.Init.Period = 999999;
	   htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	   htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	   if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	   {
	     Error_Handler();
	   }
	   sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	   if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	   {
	     Error_Handler();
	   }
	   sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	   sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	   if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	   {
	     Error_Handler();
	   }
	   /* USER CODE BEGIN TIM2_Init 2 */

	   /* USER CODE END TIM2_Init 2 */

	 }

	 /**
	   * @brief USART2 Initialization Function
	   * @param None
	   * @retval None
	   */
	 static void MX_USART2_UART_Init(void)
	 {

	   /* USER CODE BEGIN USART2_Init 0 */

	   /* USER CODE END USART2_Init 0 */

	   /* USER CODE BEGIN USART2_Init 1 */

	   /* USER CODE END USART2_Init 1 */
	   huart2.Instance = USART2;
	   huart2.Init.BaudRate = 38400;
	   huart2.Init.WordLength = UART_WORDLENGTH_8B;
	   huart2.Init.StopBits = UART_STOPBITS_1;
	   huart2.Init.Parity = UART_PARITY_NONE;
	   huart2.Init.Mode = UART_MODE_TX_RX;
	   huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	   huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	   huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	   huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	   if (HAL_UART_Init(&huart2) != HAL_OK)
	   {
	     Error_Handler();
	   }
	   /* USER CODE BEGIN USART2_Init 2 */

	   /* USER CODE END USART2_Init 2 */

	 }

	 /**
	   * @brief GPIO Initialization Function
	   * @param None
	   * @retval None
	   */
	 static void MX_GPIO_Init(void)
	 {
	   GPIO_InitTypeDef GPIO_InitStruct = {0};
	 /* USER CODE BEGIN MX_GPIO_Init_1 */
	 /* USER CODE END MX_GPIO_Init_1 */

	   /* GPIO Ports Clock Enable */
	   __HAL_RCC_GPIOF_CLK_ENABLE();
	   __HAL_RCC_GPIOA_CLK_ENABLE();
	   __HAL_RCC_GPIOB_CLK_ENABLE();

	   /*Configure GPIO pin Output Level */
	   HAL_GPIO_WritePin(LS_ENABLE_GPIO_Port, LS_ENABLE_Pin, GPIO_PIN_SET);

	   /*Configure GPIO pin Output Level */
	   HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

	   /*Configure GPIO pin : LS_ENABLE_Pin */
	   GPIO_InitStruct.Pin = LS_ENABLE_Pin;
	   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	   GPIO_InitStruct.Pull = GPIO_NOPULL;
	   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	   HAL_GPIO_Init(LS_ENABLE_GPIO_Port, &GPIO_InitStruct);

	   /*Configure GPIO pin : LED1_Pin */
	   GPIO_InitStruct.Pin = LED1_Pin;
	   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	   GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	   HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

	 /* USER CODE BEGIN MX_GPIO_Init_2 */
	 /* USER CODE END MX_GPIO_Init_2 */
	 }

	 /* USER CODE BEGIN 4 */

	 /* USER CODE END 4 */

	 /**
	   * @brief  This function is executed in case of error occurrence.
	   * @retval None
	   */
	 void Error_Handler(void)
	 {
	   /* USER CODE BEGIN Error_Handler_Debug */
	   /* User can add his own implementation to report the HAL error return state */
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
