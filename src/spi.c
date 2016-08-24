/*
 * spi.c
 *
 *  Created on: 02 дек. 2015 г.
 *      Author: svalov
 */

#include "spi.h"
#include <stdint.h>
#include <stddef.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

int spiChannel = 0;
uint8_t csPort;
uint8_t csPin;

//init
void SPI_Init(int channel, int speed)
{

}
//send
void SPI_RW_Data(void *port, uint8_t *tx, uint8_t *rx, uint8_t count){

}

void SPI_DeInit(int channel){

}

