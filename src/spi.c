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
#include <stdio.h>
static int spiChannel = 0;

//init
void SPI_Init(int channel, int speed)
{
	printf("SPI INTERNAL INIT\n");
	spiChannel = channel;
	int spiRes = wiringPiSPISetup(channel, speed);
	printf("SPI %d\n",spiRes);
}
//send
void SPI_RW_Data(int channel, uint8_t *tx, uint8_t *rx, uint8_t count){
//	//printf("Tx: ");
//	for(int i=0;i<count; i++){
//		//printf("%x",tx[i]);
//	}
//	//printf("\n");
	memcpy(rx,tx,count);
	int res = wiringPiSPIDataRW(channel, rx, count);
//	//printf("Rx: ");
//	for(int i=0;i<count; i++){
//		//printf("%x",rx[i]);
//	}
//	printf("\n");
}

void SPI_DeInit(int channel){
	// no such function
}

