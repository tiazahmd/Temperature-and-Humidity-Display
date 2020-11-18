/*
 * sensor.c
 *
 *  Created on: Oct 19, 2020
 *      Author: imtiaz
 */

#include "sensor.h"

uint8_t rh_byte1, rh_byte2, temp_byte1, temp_byte2;
uint16_t checksum; uint8_t response = 0;

uint32_t US_Delay_Init(void)
{
	CoreDebug->DEMCR &= ~(CoreDebug_DEMCR_TRCENA_Msk);
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	DWT->CTRL &= ~(DWT_CTRL_CYCCNTENA_Msk);
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	DWT->CYCCNT = 0;

	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");

	if (DWT->CYCCNT) return 0;
	else return 1;
}


__STATIC_INLINE void US_Delay(uint32_t microseconds)
{
	uint32_t clk_cycle_start = DWT->CYCCNT;

	microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

	while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}

void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_A;
	GPIO_A.Pin = DHT11_GPIO_PIN;
	GPIO_A.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_A.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_A);
}

void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_A;
	GPIO_A.Pin = DHT11_GPIO_PIN;
	GPIO_A.Mode = GPIO_MODE_INPUT;
	GPIO_A.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_A);
}

void DHT_Start(void)
{
	US_Delay_Init();
	Set_Pin_Output(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
	HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, RESET);

	US_Delay(18000);

	HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, SET);
	US_Delay(30);
	Set_Pin_Input(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
}

uint8_t DHT_Check_Response(void)
{
	uint8_t dht_response = 0;
	US_Delay(40);
	if (!(HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN))) {
		US_Delay(80);
		if ((HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN))) dht_response = 1;
		else dht_response = -1;
	}

	while ((HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN)));

	return dht_response;
}

uint8_t DHT_Read(void)
{
	uint8_t i, data;
	for (i = 0; i < 8; i++) {
		while (!(HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN)));
		US_Delay(40);
		if (!(HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN))) data &= ~(1 << (7 - i));
		else data |= (1 << (7 - i));
		while ((HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN)));
	}

	return data;
}

void DHT_GetData (DHT_DataTypedef *DHT_Data)
{
	DHT_Start();
	response = DHT_Check_Response();
	rh_byte1 = DHT_Read();
	rh_byte2 = DHT_Read();
	temp_byte1 = DHT_Read();
	temp_byte2 = DHT_Read();
	checksum = DHT_Read();

	if (checksum == (rh_byte1 + rh_byte2 + temp_byte1 + temp_byte2)) {
		DHT_Data->temperature = temp_byte1;
		DHT_Data->humidity = rh_byte1;
	}
}













// End of file
