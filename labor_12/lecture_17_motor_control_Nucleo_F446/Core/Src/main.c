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
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "oled.h"
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
  MX_I2C2_Init();
  MX_TIM10_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */

  OLED_Init();
  Motor_init();

  char display_char[20];
  set_speed_percent = 0;
  OLED_VERTEX setpoint[OLED_WIDTH];
  OLED_VERTEX output[OLED_WIDTH];

  for(int i = 0; i < OLED_WIDTH; i++)
  {
	  setpoint[i].x = i;
	  setpoint[i].y = OLED_HEIGHT/2;
	  output[i].x = i;
	  output[i].y = OLED_HEIGHT/2;
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /* Increase speed by pushing Button 1 */
	  if(HAL_GPIO_ReadPin(HMI_BTN_1_GPIO_Port, HMI_BTN_1_Pin))
	  {
		  set_speed_percent += 10;
	  }

	  /* Decrease speed by pushing Button 5 */
	  if(HAL_GPIO_ReadPin(HMI_BTN_5_GPIO_Port, HMI_BTN_5_Pin))
	  {
		  set_speed_percent -= 10;
	  }

	  /* Stop motor by pushing Button 3 */
	  if(HAL_GPIO_ReadPin(HMI_BTN_3_GPIO_Port, HMI_BTN_3_Pin))
	  {
		  set_speed_percent = 0;
	  }
	  if (set_speed_percent > 100)
	  {
		  set_speed_percent = 100;
	  }
	  else if (set_speed_percent < -100)
	  {
		  set_speed_percent = -100;
	  }

	  /* Set the motor speed */
	  Motor_setPWMDutyPercent((float)set_speed_percent);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  /* Draw the speed chart */
	  for(int i = 0; i < OLED_WIDTH - 1; i++)
	  {
		  setpoint[i].y = setpoint[i+1].y;
		  output[i].y = output[i+1].y;
	  }

	  setpoint[OLED_WIDTH-1].y = OLED_HEIGHT / 2 - set_speed_percent * 0.25;
	  output[OLED_WIDTH-1].y = OLED_HEIGHT / 2 - output_speed_rpm * 0.25;

	  OLED_Fill(Black);
	  OLED_Polyline(setpoint, OLED_WIDTH, White);
	  OLED_Polyline(output, OLED_WIDTH, White);

	  /* Write text on the display */
	  OLED_SetCursor(2, 0);
	  OLED_WriteString("MOTOR", Font_11x18, White);

	  sprintf(display_char, "SetPWMout %ld", set_speed_percent);

	  OLED_SetCursor(2, 20);
	  OLED_WriteString(display_char, Font_7x10, White);

	  sprintf(display_char, "EncPos %ld", encoder_position_act);

	  OLED_SetCursor(2, 30);
	  OLED_WriteString(display_char, Font_7x10, White);

	  sprintf(display_char, "EncRPM %ld", (int32_t)encoder_speed_rpm);

	  OLED_SetCursor(2, 40);
	  OLED_WriteString(display_char, Font_7x10, White);

	  sprintf(display_char, "OutRPM %ld", (int32_t)output_speed_rpm);

	  OLED_SetCursor(2, 50);
	  OLED_WriteString(display_char, Font_7x10, White);

	  OLED_UpdateScreen();
	  HAL_Delay(100);
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
