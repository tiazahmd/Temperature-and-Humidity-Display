/*
 * ssd1306_i2c.c
 *
 *  Created on: Oct 16, 2020
 *      Author: imtiaz
 */


#include "ssd1306_i2c.h"

static uint32_t ssd1306_I2C_Timeout;

#define I2C_TRANSMITTER_MODE   0
#define I2C_RECEIVER_MODE      1
#define I2C_ACK_ENABLE         1
#define I2C_ACK_DISABLE        0

int16_t ssd1306_I2C_Start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction, uint8_t ack)
{
	I2Cx->CR1 |= I2C_CR1_START;

	// Wait while I2C is busy
	ssd1306_I2C_Timeout = ssd1306_I2C_TIMEOUT;
	while (!(I2Cx->SR1 & I2C_SR1_SB)) {
		if (--ssd1306_I2C_Timeout == 0x00) {
			return 1;
		}
	}

	// Enable ack if selected
	if (ack) {
		I2Cx->CR1 |= I2C_ACK_ENABLE;
	}

	// Send write/read bit
	if (direction == I2C_TRANSMITTER_MODE) {
		// Send address with last bit == 0
		I2Cx->DR = address & ~I2C_OAR1_ADD0;

		// Wait till finished
		ssd1306_I2C_Timeout = ssd1306_I2C_TIMEOUT;
		while (!(I2Cx->SR1 & I2C_SR1_ADDR)) {
			if (--ssd1306_I2C_Timeout == 0x00) {
				return 1;
			}
		}

	}

	if (direction == I2C_RECEIVER_MODE) {
		// Send address with last bit == 1
		I2Cx->DR = address | I2C_OAR1_ADD0;

		// Wait till finished
		ssd1306_I2C_Timeout = ssd1306_I2C_TIMEOUT;
		while (!(I2Cx->SR1 & I2C_SR1_ADDR)) {
			if (--ssd1306_I2C_Timeout == 0x00) {
				return 1;
			}
		}
	}

	// Read SR to clear ADDR flag
	I2Cx->SR2;

	return 0;
}

void ssd1306_I2C_WriteData(I2C_TypeDef* I2Cx, uint8_t data)
{
	// Wait while I2C is busy
	ssd1306_I2C_Timeout = ssd1306_I2C_TIMEOUT;
	while (!(I2Cx->SR1 & I2C_SR1_TXE) && ssd1306_I2C_Timeout) {
		ssd1306_I2C_Timeout--;
	}

	// Send I2C data
	I2Cx->DR = data;
}

void ssd1306_I2C_Write(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data)
{
	ssd1306_I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	ssd1306_I2C_WriteData(I2Cx, reg);
	ssd1306_I2C_WriteData(I2Cx, data);
	ssd1306_I2C_Stop(I2Cx);
}

void ssd1306_I2C_WriteMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t *data, uint16_t count)
{
	uint8_t i;

	ssd1306_I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);

	ssd1306_I2C_WriteData(I2Cx, reg);

	for (i = 0; i < count; i++) {
		ssd1306_I2C_WriteData(I2Cx, data[i]);
	}

	ssd1306_I2C_Stop(I2Cx);
}

uint8_t ssd1306_I2C_Stop(I2C_TypeDef* I2Cx)
{
	// Wait while transmitter is full
	ssd1306_I2C_Timeout = ssd1306_I2C_TIMEOUT;
	while (((!(I2Cx->SR1 & I2C_SR1_TXE)) || (!(I2Cx->SR1 & I2C_SR1_BTF)))) {
		if (--ssd1306_I2C_Timeout == 0x00) {
			return 1;
		}
	}

	// Generate stop condition
	I2Cx->CR1 |= I2C_CR1_STOP;

	return 0;
}

uint8_t ssd1306_I2C_IsDeviceConnected(I2C_TypeDef* I2Cx, uint8_t address) {
	uint8_t connected = 0;

	// Try to start, function will return 0 in case device will send ACK
	if (!ssd1306_I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_ENABLE)) {
		connected = 1;
	}

	/* STOP I2C */
	ssd1306_I2C_Stop(I2Cx);

	/* Return status */
	return connected;
}









// End of file
