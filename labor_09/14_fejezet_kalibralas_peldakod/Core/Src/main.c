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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "OLED.h"
#include "thermistor_misc.h"
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
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

// lehetséges képernyő üzemmódok
enum Screen
{
	SCRN_Start = 0,
	SCRN_ADC = 1,
};

//a SCRN_ADC képernyő megjelenítési módjai
enum ADC_DisplayMode
{
	DM_Temp = 0,
	DM_Diff = 1,
	DM_ConvRes = 2
};

// üzemmódok és kijelzési módok mennyiségének definiálása
const unsigned int NUMBER_OF_WORKING_MODES = 2;
const unsigned int NUMBER_OF_DISPLAY_MODES = 3;

// alapértelmezett üzemmód és kijelzési mód definiálása
unsigned short current_screen = SCRN_Start;
unsigned short display_mode = DM_Temp;

//végleges adc eredményt tároló változó
uint32_t adc_result = 0;

//kiszámított hőmérsékletet és a kalibrációból adódó korrekció értékét tároló változó
float temperature_calculated = 0.0;
float temperature_correction = 0.0;

// gombok állapotának tárolása
bool button_state = true;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// az A-D átalakítás elvégzése és az aktuális képernyő megjelenítése
void ConvertAndDisplay()
{
	// változó a mért adc értékek összegzésére
	float cumulated_result = 0.0;

	// az átlagoláshoz használt átalakítások száma
	const unsigned int SAMPLE_NUMBER = 16;
	// kommunikáció ellenőrzésére változó létrehozása
	HAL_StatusTypeDef error_code = HAL_OK;
	for (unsigned int i = 0;i < SAMPLE_NUMBER;++i)
	{
		// analóg-digitális átalakítás indítása
		HAL_ADC_Start(&hadc1);
		error_code = HAL_ADC_PollForConversion(&hadc1, 100);
		if (error_code != HAL_OK)
		{
			DisplayTextAndInteger("Error:", error_code, NoUnit);
			HAL_Delay(1000);
			break;
		}
		cumulated_result += HAL_ADC_GetValue(&hadc1);
	}
	// kommunikáció sikerességének ellenőrzése
	if (error_code != HAL_OK)
	{
		return;
	}
	// átlagolt eredmény kiszámítása
	adc_result = cumulated_result/SAMPLE_NUMBER;
	// hőmérséklet kiszámítása a kapott adc értékből
	temperature_calculated = CalculateTemperature(adc_result);
	// kalibrálási korrekció figyelembevétele
	float temperature = temperature_calculated + temperature_correction;

	// az eredmény megjelenítése a kijelző módjától függően
	if (display_mode == DM_Temp)
	{
		DisplayTextAndFloat("Temp:", temperature, Celsius);
	}
	else if (display_mode == DM_Diff)
	{
		DisplayTextAndFloat("Diff:", temperature_correction, Celsius);
	}
	else if (display_mode == DM_ConvRes)
	{
		DisplayTextAndInteger("ADC:",  adc_result, NoUnit);
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
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  OLED_SelectDisplay(Display_1);
  OLED_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Állapotgép
	  switch (current_screen)
	  {
	  case SCRN_Start:
		  DisplayText("Start", "screen", NoUnit);
		  HAL_Delay(500);
		  break;
	  case SCRN_ADC:
		  ConvertAndDisplay();
		  HAL_Delay(500);
		  break;
	  default:
		  DisplayText("Unknown", "screen", NoUnit);
		  HAL_Delay(500);
		  break;
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

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 16000;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 50;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : BTN5_Pin BTN4_Pin BTN3_Pin BTN2_Pin */
  GPIO_InitStruct.Pin = BTN5_Pin|BTN4_Pin|BTN3_Pin|BTN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN1_Pin */
  GPIO_InitStruct.Pin = BTN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

//nyomógomb pergésmentesítése timer segítségével
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//timer indítása ha a megfelelő gombot megnyomták és még nincs folyamatban gombnyomás
	if ((GPIO_Pin == BTN5_Pin || GPIO_Pin == BTN4_Pin || GPIO_Pin == BTN3_Pin || GPIO_Pin == BTN2_Pin || GPIO_Pin == BTN1_Pin) && button_state == true)
	{
		//időzítő indítása
		HAL_TIM_Base_Start_IT(&htim1);
		//nyomógombok tiltása
		button_state = false;
	}
	else
	{
		__NOP();
	}
}

//nyomógombok kezelése
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//képernyők közötti váltás
	if (HAL_GPIO_ReadPin(GPIOD, BTN1_Pin) == GPIO_PIN_SET)
	{
		current_screen = (current_screen + 1) % NUMBER_OF_WORKING_MODES;
	}
	else if (HAL_GPIO_ReadPin(GPIOB, BTN5_Pin) == GPIO_PIN_SET)
	{
		current_screen = (current_screen + NUMBER_OF_WORKING_MODES - 1) % NUMBER_OF_WORKING_MODES;
	}
	//képernyő módok közötti váltás
	else if (HAL_GPIO_ReadPin(GPIOB, BTN4_Pin) == GPIO_PIN_SET && current_screen == SCRN_ADC)
	{
		display_mode = (display_mode + 1) % NUMBER_OF_DISPLAY_MODES;
	}
	else if (HAL_GPIO_ReadPin(GPIOB, BTN2_Pin) == GPIO_PIN_SET && current_screen == SCRN_ADC)
	{
		display_mode = (display_mode + NUMBER_OF_DISPLAY_MODES - 1) % NUMBER_OF_DISPLAY_MODES;
	}
	//művelet indítása
	else if (HAL_GPIO_ReadPin(GPIOB, BTN3_Pin) == GPIO_PIN_SET)
	{
		//Temp képernyőnél a kalibrálás indítása
		if (current_screen == SCRN_ADC && display_mode == DM_Temp)
		{
			//a mérendő érték és a mért érték különbségének tárolása
			temperature_correction = 0.0 - temperature_calculated;
		}
		//Diff képernyőnél a kalibrálás törlése
		else if (current_screen == SCRN_ADC && display_mode == DM_Diff)
		{
			temperature_correction = 0.0;
		}
	}

	//időzítő leállítása
	HAL_TIM_Base_Stop_IT(&htim1);
	//nyomógombok engedélyezése
	button_state = true;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
