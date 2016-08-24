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
#include <string.h>

static int spiChannel = 0;

//init
void SPI_Init(int channel, int speed)
{
	spiChannel = channel;
	int spiRes = wiringPiSPISetup(channel, speed);
}
//send
void SPI_RW_Data(int channel, uint8_t *tx, uint8_t *rx, uint8_t count){
	memcpy(rx,tx,count);
	int res = wiringPiSPIDataRW(channel, rx, count);
}

void SPI_DeInit(int channel){
	// no such function
}

