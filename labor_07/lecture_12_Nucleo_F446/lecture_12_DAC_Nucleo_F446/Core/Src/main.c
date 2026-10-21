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
#include "dac.h"
#include "i2c.h"
#include "gpio.h"
#include "math.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* Add the math library */


#define HEART_POINTS      200u   /* samples per outline */
#define HEART_MAX_POINTS  512u   /* scratch buffer size */
#define DAC_FULL_SCALE    4095.0f
#define DAC_MARGIN        140.0f /* keeps the trace off the screen edges */
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
// Sine generation function
//void GenerateSine (uint32_t* sine,uint32_t length)
//{
// // Generate a specified number of points
// for (int i=0; i<length; i++)
// {
//	 // Calculate the values for the given points
//	 sine[i] = ((sin(i*2*M_PI/length) + 1)*(4095/2));
// }
//}


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
  MX_DAC_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  /* Output value in volt */
  float u_kimeneti = 1.0;
  /* Calculation of the digital input value, take care of the operational amplifier */
  uint32_t d_bemenet = (uint32_t)((u_kimeneti)*4095)/(3.3);
  /* Set the digital input value */
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, d_bemenet);
  /* Start the analog-digital conversion */
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

  /* array to store the point of the sine wave */
  uint32_t sine_data[100];

  /* Calculate the points of the sine wave*/
//  GenerateSine(sine_data,100);
//  for(int i = 0; i < 100; i++)
//  {
//	  sine_data[i] = (sine_data[i]);
//  }
  /* USER CODE BEGIN PD */

  /* USER CODE END PD */

  /* USER CODE BEGIN 0 */

  /**
    * @brief  Generates a heart outline as two coordinate tables for X-Y display.
    * @param  x_out  array receiving the X samples (DAC channel 1), 0..4095
    * @param  y_out  array receiving the Y samples (DAC channel 2), 0..4095
    * @param  length number of points per revolution
    */

  static uint32_t heart_x[HEART_POINTS];
  static uint32_t heart_y[HEART_POINTS];

  void GenerateHeart(uint32_t *x_out, uint32_t *y_out, uint32_t length)
  {
    static float xr[HEART_MAX_POINTS];
    static float yr[HEART_MAX_POINTS];
    const float two_pi = 6.28318530718f;
    float xmin =  1.0e9f, xmax = -1.0e9f;
    float ymin =  1.0e9f, ymax = -1.0e9f;

    if (length == 0u) return;
    if (length > HEART_MAX_POINTS) length = HEART_MAX_POINTS;

    /* ---- pass 1: evaluate the curve and track its extents ---- */
    for (uint32_t i = 0; i < length; i++)
    {
      float t = two_pi * (float)i / (float)length;
      float s = sinf(t);

      float x = 16.0f * s * s * s;
      float y = 13.0f * cosf(t)
              -  5.0f * cosf(2.0f * t)
              -  2.0f * cosf(3.0f * t)
              -        cosf(4.0f * t);

      xr[i] = x;
      yr[i] = y;

      if (x < xmin) xmin = x;
      if (x > xmax) xmax = x;
      if (y < ymin) ymin = y;
      if (y > ymax) ymax = y;
    }

    /* ---- pass 2: scale into the DAC range ---- */
    float xspan = (xmax - xmin) > 0.0f ? (xmax - xmin) : 1.0f;
    float yspan = (ymax - ymin) > 0.0f ? (ymax - ymin) : 1.0f;
    float usable = DAC_FULL_SCALE - 2.0f * DAC_MARGIN;

    for (uint32_t i = 0; i < length; i++)
    {
      /* y already increases upwards, and a larger DAC code is a higher
         voltage, which is also upwards on the scope -> no inversion needed */
      x_out[i] = (uint32_t)(DAC_MARGIN + (xr[i] - xmin) / xspan * usable);
      y_out[i] = (uint32_t)(DAC_MARGIN + (yr[i] - ymin) / yspan * usable);
    }
  }
  /* USER CODE END 0 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  for (int i = 0; i < 100; i++)
//	  {
//		  /* Set the digital input value */
//		 HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, sine_data[i]);
//		  /* Start the analog-digital conversion */
//		  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
//		  /* Set the delay, this will be the time between two points */
//		  HAL_Delay(0);
//	  }

	  for (uint32_t i = 0; i < HEART_POINTS; i++)
	      {
	        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, heart_x[i]);
	        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, heart_y[i]);
	        //delay_us(60);   /* 200 pts x 60 us = 12 ms per revolution (~83 Hz) */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
