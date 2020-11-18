#include "sensor.h"
#include "stm32f4xx_hal_i2c.h"
#include <stdio.h>
#include "ssd1306.h"

#define GPIO_AF4_I2C1          ((uint8_t)0x04)

TIM_HandleTypeDef htim1;

void SystemClock_Config(void);
static void MX_TIM1_Init(void);
static void I2C1_Init(void);
static uint32_t ctof(uint32_t cel);

DHT_DataTypedef DHT11_Data;
float temperature, humidity;
uint32_t int_temp, int_hum, int_temp_f;

int main(void)
{
	char temp_str[4];
	char hum_str[3];

	HAL_Init();

	__HAL_RCC_GPIOA_CLK_ENABLE();

	SystemClock_Config();

	MX_TIM1_Init();
	I2C1_Init();

	uint8_t res = SSD1306_Init();
	printf("OLED init: %d\n", res);
	SSD1306_GotoXY(10,27);
	SSD1306_Puts("OLED inited", &Font_7x10, 1);
	SSD1306_GotoXY(10,52);
	SSD1306_Puts("Imtiaz Ahmed", &Font_7x10, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(300);


	while (1) {
		DHT_GetData(&DHT11_Data);
		temperature = DHT11_Data.temperature;
		humidity = DHT11_Data.humidity;

		int_temp = (uint32_t) temperature;
		int_hum = (uint32_t) humidity;
		int_temp_f = ctof(int_temp);

		if (int_temp_f < 100) {
			temp_str[0] = ' ';
			temp_str[1] = (int_temp_f / 10) + '0';
			temp_str[2] = (int_temp_f % 10) + '0';
			temp_str[3] = '\0';
		} else {
			temp_str[0] = (int_temp_f / 100) + '0';
			temp_str[1] = ((int_temp_f / 10) % 10) + '0';
			temp_str[2] = (int_temp_f % 10) + '0';
			temp_str[3] = '\0';
		}

		hum_str[0] = (int_hum / 10) + '0';
		hum_str[1] = (int_hum % 10) + '0';
		hum_str[2] = '\0';

		SSD1306_Fill(0);
		SSD1306_GotoXY(10, 0);
		SSD1306_Puts("Temp & Hum", &Font_7x10, 1);
		SSD1306_GotoXY(10, 20);
		SSD1306_Puts("Temp: ", &Font_7x10, 1);
		SSD1306_GotoXY(50, 20);
		SSD1306_Puts((char *) temp_str, &Font_7x10, 1);
		SSD1306_GotoXY(10, 40);
		SSD1306_Puts("Hum: ", &Font_7x10, 1);
		SSD1306_GotoXY(50, 40);
		SSD1306_Puts((char *) hum_str, &Font_7x10, 1);
		SSD1306_GotoXY(65, 40);
		SSD1306_Puts("%", &Font_7x10, 1);
		SSD1306_UpdateScreen();
		HAL_Delay(300);

		HAL_Delay(3000);
	}

}


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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  htim1.Init.Prescaler = 25-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535-1;
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

/* USER CODE BEGIN 4 */
static void I2C1_Init(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_B;

	GPIO_B.Pin = GPIO_PIN_6 | GPIO_PIN_9;
	GPIO_B.Mode = GPIO_MODE_AF_OD;
	GPIO_B.Pull = GPIO_PULLUP;
	GPIO_B.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_B.Alternate = GPIO_AF4_I2C1;

	HAL_GPIO_Init(GPIOB, &GPIO_B);

	__HAL_RCC_I2C1_CLK_ENABLE();

	I2C_HandleTypeDef hi2c1;

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 400000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
	hi2c1.Init.OwnAddress1 = 0x15;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;

	HAL_I2C_Init(&hi2c1);
}

static uint32_t ctof(uint32_t cel)
{
	return ((cel * 1.80) + 32);
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
