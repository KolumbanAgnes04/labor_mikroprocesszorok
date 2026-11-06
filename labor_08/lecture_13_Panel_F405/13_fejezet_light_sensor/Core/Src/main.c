/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "OLED.h"
#include "light_sensor.h"
#include <stdio.h>

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Megszakítások számlálására létrehozott változóVariable for counting the interrupts
uint16_t interrupt_counter = 0;

// Külső interrupt lekezelő függvény
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	interrupt_counter++;			// számláló növelés
	LightSensor_ClearInterrupt();	// új megszakítások törlése
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
  /* USER CODE BEGIN 2 */

  // OLED kijelző inicializálása a hőmérséklet kijelzéshez
  OLED_Init();

  // Mért fényintenzitások eltárolására szolgáló változók
  float lightLevel;
  uint16_t channel_0;
  uint16_t channel_1;

  // Karakter tömb a mért fényintenzitás kijelzésére
  char lightLevel_char[18];
  uint16_t lightlevel_integer;
  uint16_t lightlevel_fractional;
  // Karakter tömb a megszakítás számláló kijelzésére
  char interrupt_char[18];
  char channels_char[18];
  // Változó a kommunikáció állapotónak ellenőrzésére
  HAL_StatusTypeDef sensor_i2c = HAL_OK;

  // I2C slave cím beállítás a szenzornak
  //LightSensor_SetAddress(LightSensor_ADDR_VDD);
  LightSensor_SetAddress(LightSensor_ADDR_FLOAT);
  //LightSensor_SetAddress(LightSensor_ADDR_GND);

  // Szenzor inicializálása
  sensor_i2c |= LightSensor_SetPowerState(LightSensor_POWER_ON);
  sensor_i2c |= LightSensor_SetGain(LightSensor_GAIN_16X);
  sensor_i2c |= LightSensor_SetIntegrationTime(LightSensor_INTEG_101m);

  // Megszakítások engedélyezése
  sensor_i2c |= LightSensor_SetInterruptState(LightSensor_INT_ON);
  /*
   * Megszakítás beállítása úgy, hogy akkor történjen
   * ,ha a fényintenzitás egymás után három integrálási
   * cikluson keresztül kívül esik a beállított küszöbértékeken.
   */
  sensor_i2c |= LightSensor_SetInterruptPersist(LightSensor_PERSIST_3CYCLE);
  // Küszöbértékek beállítása
  uint16_t lower_itr_level = 500;
  uint16_t upper_itr_level = 5000;
  sensor_i2c |= LightSensor_SetThreshold(lower_itr_level, upper_itr_level);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  {
	  			// Beolvasás
	  			LightSensor_GetLightLevel(&lightLevel);
	  			LightSensor_GetCH0Value(&channel_0);
	  			LightSensor_GetCH1Value(&channel_1);

	  			// Sztring konverzió
	  			lightlevel_integer = (uint16_t)lightLevel;
	  			lightlevel_fractional = (uint16_t)(lightLevel * 100) - 100 * (uint16_t)lightLevel;
	  			sprintf(lightLevel_char, "%d.%d lux", lightlevel_integer, lightlevel_fractional);
	  			sprintf(channels_char, "%d, %d", channel_0, channel_1);

	  			OLED_Fill(Black);

	  			OLED_SetCursor(2, 0);
	  			OLED_WriteString("Illuminance:", Font_7x10, White);
	  			OLED_SetCursor(2, 10);
	  			OLED_WriteString(lightLevel_char, Font_7x10, White);

	  		}

	// Hiba esetén az összes LED bekapcsolása
	if (sensor_i2c == HAL_ERROR)
	{
		HAL_GPIO_WritePin(HMI_LED_1_GPIO_Port, HMI_LED_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(HMI_LED_2_GPIO_Port, HMI_LED_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(HMI_LED_3_GPIO_Port, HMI_LED_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(HMI_LED_4_GPIO_Port, HMI_LED_4_Pin, GPIO_PIN_SET);
		// Állapotváltozó visszaállítása a következő ciklushoz
		sensor_i2c = HAL_OK;
		LightSensor_SetAddress(LightSensor_ADDR_GND);
		LightSensor_ClearInterrupt();
		LightSensor_SetAddress(LightSensor_ADDR_FLOAT);
	}
	else
	{
		HAL_GPIO_WritePin(HMI_LED_1_GPIO_Port, HMI_LED_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(HMI_LED_2_GPIO_Port, HMI_LED_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(HMI_LED_3_GPIO_Port, HMI_LED_3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(HMI_LED_4_GPIO_Port, HMI_LED_4_Pin, GPIO_PIN_RESET);
	}
	OLED_UpdateScreen();
	HAL_Delay(200);

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
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

	HAL_GPIO_WritePin(HMI_LED_1_GPIO_Port, HMI_LED_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HMI_LED_2_GPIO_Port, HMI_LED_2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HMI_LED_3_GPIO_Port, HMI_LED_3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HMI_LED_4_GPIO_Port, HMI_LED_4_Pin, GPIO_PIN_RESET);

	while (1)
	{
		HAL_GPIO_TogglePin(HMI_LED_1_GPIO_Port, HMI_LED_1_Pin);
		HAL_GPIO_TogglePin(HMI_LED_2_GPIO_Port, HMI_LED_2_Pin);
		HAL_GPIO_TogglePin(HMI_LED_3_GPIO_Port, HMI_LED_3_Pin);
		HAL_GPIO_TogglePin(HMI_LED_4_GPIO_Port, HMI_LED_4_Pin);

		HAL_Delay(200);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
