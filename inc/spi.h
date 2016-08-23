/*
 * spi.h
 *
 *  Created on: 02 дек. 2015 г.
 *      Author: svalov
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "board.h"
void SPI_InitLib();
void SPI_Init(LPC_SSP_T *port);
void SPI_AltCsInit(LPC_GPIO_T* gpio, uint8_t port, uint8_t pin);
void SPI_Transaction(LPC_SSP_T *port, Chip_SSP_DATA_SETUP_T xf);
void SPI_RW_Data(LPC_SSP_T *port, uint8_t *tx, uint8_t *rx, uint8_t count);
void SPI_DeInit(LPC_SSP_T *port);

#endif /* INC_SPI_H_ */
