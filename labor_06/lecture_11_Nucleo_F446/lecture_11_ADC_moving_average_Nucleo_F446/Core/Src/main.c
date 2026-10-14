/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "adc.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "stdio.h"
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
uint32_t adc;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();

  /* USER CODE BEGIN 2 */
  OLED_Init();

  char adc_char[11];
  uint32_t adc_avg[25] = {};
  uint8_t p_avg_cnt = 0;
  uint32_t avg = 0;
  uint16_t line_len;
  uint16_t y_center;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE WHILE 1 */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* Start the conversion in blocking mode */
    HAL_ADC_Start(&hadc1);

    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
    {
      adc = HAL_ADC_GetValue(&hadc1);

      if (p_avg_cnt == 25)
        p_avg_cnt = 0;

      adc_avg[p_avg_cnt] = adc;
      p_avg_cnt++;

      avg = 0;
      for (uint8_t i = 0; i < 25; i++)
        avg += adc_avg[i];
      avg = avg / 25;
    }

    /* Map the 12-bit ADC value (0..4095) to the display width (0..127) */
    line_len = (uint16_t)((avg * (OLED_WIDTH - 1)) / 4095);
    y_center = OLED_HEIGHT / 2;

    OLED_Fill(Black);

    /* Main line: length follows the pot position (3px thick for visibility) */
    OLED_Line(0, y_center - 1, line_len, y_center - 1, White);
    OLED_Line(0, y_center,     line_len, y_center,     White);
    OLED_Line(0, y_center + 1, line_len, y_center + 1, White);

    /* End cap/marker at the tip of the line */
    OLED_Line(line_len - 1, y_center - 2, line_len + 1, y_center - 2, White);
    OLED_Line(line_len - 1, y_center + 2, line_len + 1, y_center + 2, White);

    /* Show raw ADC value at bottom-left */
    sprintf(adc_char, "%lu", (unsigned long)avg);
    OLED_SetCursor(2, OLED_HEIGHT - 20);
    OLED_WriteString(adc_char, Font_7x10, White);

    /* Show voltage at bottom-right */
    sprintf(adc_char, "%lumV", (unsigned long)(avg * 0.806f));
    OLED_SetCursor(OLED_WIDTH / 2 + 10, OLED_HEIGHT - 20);
    OLED_WriteString(adc_char, Font_7x10, White);

    OLED_UpdateScreen();

    HAL_Delay(25);

    /* USER CODE END 3 */
  }
  /* USER CODE END 3 */
}
/* MAIN FUNCTION CLOSING BRACE */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
