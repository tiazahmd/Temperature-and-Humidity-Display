/*
 * ssd1306_i2c.h
 *
 *  Created on: Oct 16, 2020
 *      Author: imtiaz
 */

#ifndef INC_SSD1306_I2C_H_
#define INC_SSD1306_I2C_H_

#ifndef ssd1306_I2C_TIMEOUT
#define ssd1306_I2C_TIMEOUT					20000
#endif

#include "stm32f407xx.h"

void ssd1306_I2C_Write(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data);
void ssd1306_I2C_WriteMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);
int16_t ssd1306_I2C_Start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction, uint8_t ack);
uint8_t ssd1306_I2C_Stop(I2C_TypeDef* I2Cx);
void ssd1306_I2C_WriteData(I2C_TypeDef* I2Cx, uint8_t data);
uint8_t ssd1306_I2C_IsDeviceConnected(I2C_TypeDef* I2Cx, uint8_t address);


#endif /* INC_SSD1306_I2C_H_ */
