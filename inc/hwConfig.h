/*
 * hwConfig.h
 *
 *  Created on: 03 февр. 2016 г.
 *      Author: svalov
 */

#ifndef HWCONFIG_H_
#define HWCONFIG_H_

#define ENABLE_IO

#define  SPI_CHANNEL  0
#define  SPI_SPEED 	  1000000

//interrupts
#define DIO0_ENABLE
#define DIO1_ENABLE
#define DIO2_ENABLE
#define DIO3_ENABLE
//#define DIO4_ENABLE
//#define DIO5_ENABLE
#define RESET_ENABLE

#define DIO0_PORT 0
#define DIO0_PIN 2

#define DIO1_PORT 0
#define DIO1_PIN 3

#define DIO2_PORT 0
#define DIO2_PIN 0

#define DIO3_PORT 0
#define DIO3_PIN 4

#define DIO4_PORT 0
#define DIO4_PIN -1

#define DIO5_PORT 0
#define DIO5_PIN -1

#define RESET_PORT 0
#define RESET_PIN 5 //7


#endif /* HWCONFIG_H_ */
