/*
 * spi.h
 *
 *  Created on: 02 дек. 2015 г.
 *      Author: svalov
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "stdint.h"

void SPI_Init(int channel, int speed);
void SPI_RW_Data(int channel, uint8_t *tx, uint8_t *rx, uint8_t count);
void SPI_DeInit(int channel);

#endif /* INC_SPI_H_ */
