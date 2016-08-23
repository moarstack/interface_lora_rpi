/*
 * spi.h
 *
 *  Created on: 02 дек. 2015 г.
 *      Author: svalov
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

//#include "board.h"
void SPI_InitLib();
void SPI_Init(void *port);
void SPI_RW_Data(void *port, uint8_t *tx, uint8_t *rx, uint8_t count);
void SPI_DeInit(void *port);

#endif /* INC_SPI_H_ */
