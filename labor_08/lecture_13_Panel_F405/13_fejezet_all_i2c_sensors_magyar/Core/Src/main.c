/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "OLED.h"
#include "humidity_sensor.h"
#include "light_sensor.h"
#include "temperature_sensor.h"
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  // Pergésmentesítő kódrészlet a 7. tananyagrészből
#define DEBOUNCING_TRESHOLD 20

  uint8_t count_debounce = 0;
  uint8_t previous_state_debounce;
  uint8_t current_state_debounce = HAL_GPIO_ReadPin(HMI_BTN_3_GPIO_Port, HMI_BTN_3_Pin);

  uint8_t previous_state_edge = HAL_GPIO_ReadPin(HMI_BTN_3_GPIO_Port, HMI_BTN_3_Pin);
  uint8_t counter = 0x01;

  // OLED kijelző inicializálás a beolvasott lm_temperature kijelzéshez
   OLED_Init();

   // Fényszenzor
   // A mért fényerősség tárolása
   float lightLevel;
   uint16_t channel_0;
   uint16_t channel_1;

   // Karaktertömb a mért fényerősség kijelzésére
   char lightLevel_char[18];
   uint16_t lightlevel_integer;
   uint16_t lightlevel_fractional;
   char channels_char[18];

   // A fényérzékelő I2C slave címének beállítása
   //LightSensor_SetAddress(LightSensor_ADDR_VDD);
   LightSensor_SetAddress(LightSensor_ADDR_FLOAT);
   //LightSensor_SetAddress(LightSensor_ADDR_GND);

   // Fényérzékelő inicializálása
   LightSensor_SetPowerState(LightSensor_POWER_ON);
   LightSensor_SetGain(LightSensor_GAIN_16X);
   LightSensor_SetIntegrationTime(LightSensor_INTEG_101m);

   // Hőmérséklet érzékelő
   // Változó a mért hih_temperature érték eltárolására
   float lm_temperature;
   uint16_t lm_temperature_int;
   uint16_t lm_temperature_frac;

   // Character array for displaying the measured hih_temperature
   // Karaktertömb a mért hőmérséklet kijelzésére
   char lm_temperature_char[20];

   /*
    * Az IC túlmelegedési leállításának használata
    * (LED az LM75A IC mellett)
    */
   // Túlhőmérsékleti küszöbérték (bekapcsolás) beállítása 28 Celsius-fokra
   LM75A_SetOSTemperature(28);
   // Túlhőmérsékleti hiszterézis (kikapcsolás) beállítása 27 Celsius-fokra
   LM75A_SetOSHysteresis(27);

   // Páratartalom érzékelő
   // Mért érték tárolása
   float humidityLevel;
   float hih_temperature;

   // Karaktertömb a mért páratartalom kijelzésére
   int string_length = 20;
   char humidityLevel_char[string_length];
   char hih_temperature_char[string_length];


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		count_debounce = 0;
		while (count_debounce < DEBOUNCING_TRESHOLD)
		{
			previous_state_debounce = current_state_debounce;
			current_state_debounce = HAL_GPIO_ReadPin(HMI_BTN_3_GPIO_Port, HMI_BTN_3_Pin);
			if (previous_state_debounce == current_state_debounce)
			{
				count_debounce++;
			}
			else
			{
				count_debounce = 0;
			}
		}
		if(current_state_debounce && !previous_state_edge)
		{
			counter <<= 1;
			if (counter == 0x10)
			{
			  counter = 0x01;
			}
		}
		previous_state_edge = current_state_debounce;

		// Fényérzékelő
		if (counter == 0x01)
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

			OLED_SetCursor(2, 10+10);
			OLED_WriteString("Channel 0, 1:", Font_7x10, White);
			OLED_SetCursor(2, 10+10+10);
			OLED_WriteString(channels_char, Font_7x10, White);
		}
		else if(counter == 0x02)
		{
			// Hőmérséklet érzékelő
			// Szenzor beolvasás
			LM75A_GetTemperature(&lm_temperature);

			// Float érték sztringgé alakítása. Az egész és a tört rész külön van kezelve.
			lm_temperature_int = (uint16_t)lm_temperature;
			lm_temperature_frac = (uint16_t)(lm_temperature * 100) - 100 * (uint16_t)lm_temperature;
			sprintf(lm_temperature_char, "%d.%d C", lm_temperature_int, lm_temperature_frac);

			// Kijelző frissítés
			OLED_Fill(Black);
			OLED_SetCursor(2, 0);
			OLED_WriteString("Temperature:", Font_7x10, White);
		    OLED_SetCursor(2, 10);
		    OLED_WriteString(lm_temperature_char, Font_7x10, White);
		    // A °C jelben a kör pozíciója a kijelzett szám törtrészének hosszától függ
		    if (lm_temperature_char[4] == ' ')
		    {
		    	OLED_DrawCircle(33, 12, 2, White);
		    }
		    else
		    {
		    	OLED_DrawCircle(40, 12, 2, White);
		    }
		}
		else if (counter == 0x04)
		{
			/*
			 * Szenzor beolvasás és adatkonverzió
			 * Az egészrész és a törtrész konverziója külön történik,
			 * mivel az (uint16_t) kasztolás ignorálja a törtrészét a float típusú számnak.
			 */
			HumTempSensor_GetValues(&humidityLevel, &hih_temperature);
			sprintf(humidityLevel_char, "%d.%d%%", (uint16_t)humidityLevel, (uint16_t)(humidityLevel * 100) - ((uint16_t)humidityLevel) * 100);
			sprintf(hih_temperature_char, "%d.%d C", (uint16_t)hih_temperature, (uint16_t)(hih_temperature * 100) - ((uint16_t)hih_temperature) * 100 );

			// Kijelző frissítés
			OLED_Fill(Black);

			OLED_SetCursor(2, 0);
			OLED_WriteString("Relative humidity:", Font_7x10, White);
			OLED_SetCursor(2, 10);
			OLED_WriteString(humidityLevel_char, Font_7x10, White);
			OLED_SetCursor(2, 10+10);
			OLED_WriteString("Temperature:", Font_7x10, White);
			OLED_SetCursor(2, 10+10+10);
			OLED_WriteString(hih_temperature_char, Font_7x10, White);
			// A °C jelben a kör pozíciója a kijelzett szám törtrészének hosszától függ
			if (hih_temperature_char[4] == ' ')
			{
				OLED_DrawCircle(33, 32, 2, White);
			}
			else
			{
				OLED_DrawCircle(40, 32, 2, White);
			}
		}
		else if (counter == 0x08)
		{
			// Hőmérséklet érzékelő
			// Hőmérséklet beolvasás
			LM75A_GetTemperature(&lm_temperature);

			// Float érték sztringgé alakítása. Az egész és a tört rész külön van kezelve.
			lm_temperature_int = (uint16_t)lm_temperature;
			lm_temperature_frac = (uint16_t)(lm_temperature * 100) - 100 * (uint16_t)lm_temperature;
			sprintf(lm_temperature_char, "%d.%d C", lm_temperature_int, lm_temperature_frac);

			// Páratartalom érzékelő

			HumTempSensor_GetValues(&humidityLevel, &hih_temperature);
			sprintf(hih_temperature_char, "%d.%d C", (uint16_t)hih_temperature, (uint16_t)(hih_temperature * 100) - ((uint16_t)hih_temperature) * 100 );


			// Kijelző frissítés
			OLED_Fill(Black);
			OLED_SetCursor(2, 0);
			OLED_WriteString("LM75:", Font_7x10, White);
		    OLED_SetCursor(2, 10);
		    OLED_WriteString(lm_temperature_char, Font_7x10, White);
		    // A °C jelben a kör pozíciója a kijelzett szám törtrészének hosszától függ
		    if (lm_temperature_char[4] == ' ')
		    {
		    	OLED_DrawCircle(33, 12, 2, White);
		    }
		    else
		    {
		    	OLED_DrawCircle(40, 12, 2, White);
		    }
			OLED_SetCursor(2, 20);
			OLED_WriteString("HIH8120:", Font_7x10, White);
		    OLED_SetCursor(2, 30);
		    OLED_WriteString(hih_temperature_char, Font_7x10, White);
		    // A °C jelben a kör pozíciója a kijelzett szám törtrészének hosszától függ
		    if (hih_temperature_char[4] == ' ')
		    {
		    	OLED_DrawCircle(33, 32, 2, White);
		    }
		    else
		    {
		    	OLED_DrawCircle(40, 32, 2, White);
		    }
		}
		else
		{
			OLED_Fill(Black);

		}
		OLED_UpdateScreen();


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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
