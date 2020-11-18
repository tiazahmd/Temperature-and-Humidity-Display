/*
 * sensor.h
 *
 *  Created on: Oct 19, 2020
 *      Author: imtiaz
 */

#ifndef SRC_SENSOR_H_
#define SRC_SENSOR_H_

#include "main.h"

#define DHT11_GPIO_PORT				GPIOA
#define DHT11_GPIO_PIN				GPIO_PIN_1

typedef struct
{
	float temperature;
	float humidity;
} DHT_DataTypedef;

void DHT_GetData (DHT_DataTypedef *DHT_Data);
uint32_t US_Delay_Init(void);
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void DHT_Start(void);
uint8_t DHT_Check_Response(void);
uint8_t DHT_Read(void);

#endif /* SRC_SENSOR_H_ */
