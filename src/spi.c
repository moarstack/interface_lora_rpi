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
#include <hwConfig.h>
static int spiChannel = 0;

//init
void SPI_Init(int channel, int speed)
{
#ifdef ENABLE_IO
	spiChannel = channel;
	int spiRes = wiringPiSPISetup(channel, speed);
#endif
}
//send
void SPI_RW_Data(int channel, uint8_t *tx, uint8_t *rx, uint8_t count){
#ifdef ENABLE_IO
	memcpy(rx,tx,count);
	int res = wiringPiSPIDataRW(channel, rx, count);
#endif
}

void SPI_DeInit(int channel){
	// no such function
}

