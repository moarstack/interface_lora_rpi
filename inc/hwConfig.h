/*
 * hwConfig.h
 *
 *  Created on: 03 февр. 2016 г.
 *      Author: svalov
 */

#ifndef HWCONFIG_H_
#define HWCONFIG_H_



//interrupts

#define ADC_CHANNEL_RND ADC_CH0

#define DIO0_ENABLE
#define DIO1_ENABLE
#define DIO2_ENABLE
#define DIO3_ENABLE
//#define DIO4_ENABLE
//#define DIO5_ENABLE
#define RESET_ENABLE

#define DIO0_PORT 0
#define DIO0_PIN 0

#define DIO1_PORT 0
#define DIO1_PIN 0

#define DIO2_PORT 0
#define DIO2_PIN 0

#define DIO3_PORT 0
#define DIO3_PIN 0

#define DIO4_PORT 0
#define DIO4_PIN 0

#define DIO5_PORT 0
#define DIO5_PIN 0

#ifdef CHIP_LPC175X_6X
#define ALT_CS_ENABLE
#endif

#ifndef RESET_ENABLE
#define RESET_PORT		  -1
#define RESET_PIN		  -1
#endif

#ifdef BOARD_NXP_LPCXPRESSO_1347

#ifdef DIO0_ENABLE
#define DIO0_PORT		  1
#define DIO0_PIN		  26
#endif

#ifdef DIO1_ENABLE
#define DIO1_PORT		  1
#define DIO1_PIN		  25
#endif

#ifdef DIO2_ENABLE
#define DIO2_PORT		  1
#define DIO2_PIN		  27
#endif

#ifdef DIO3_ENABLE
#define DIO3_PORT		  1
#define DIO3_PIN		  24
#endif

#ifdef DIO4_ENABLE
#define DIO4_PORT		  1
#define DIO4_PIN		  33
#endif

#ifdef DIO5_ENABLE
#define DIO5_PORT		  1
#define DIO5_PIN		  33
#endif

#ifdef RESET_ENABLE
#define RESET_PORT		  1
#define RESET_PIN		  28
#endif

#endif

#ifdef BOARD_NXP_LPCXPRESSO_1343

#ifdef DIO0_ENABLE
#define DIO0_PORT		  1
#define DIO0_PIN		  5
#endif

#ifdef DIO1_ENABLE
#define DIO1_PORT		  1
#define DIO1_PIN		  8
#endif

#ifdef DIO2_ENABLE
#define DIO2_PORT		  1
#define DIO2_PIN		  4
#endif

#ifdef DIO3_ENABLE
#define DIO3_PORT		  1
#define DIO3_PIN		  9
#endif

#ifdef DIO4_ENABLE
#define DIO4_PORT		  1
#define DIO4_PIN		  33
#endif

#ifdef DIO5_ENABLE
#define DIO5_PORT		  1
#define DIO5_PIN		  33
#endif

#ifdef RESET_ENABLE
#define RESET_PORT		  1
#define RESET_PIN		  11
#endif

#endif

#ifdef BOARD_NXP_LPCXPRESSO_1769
#ifdef DIO0_ENABLE
#define DIO0_PORT		  0
#define DIO0_PIN		  15
#endif

#ifdef DIO1_ENABLE
#define DIO1_PORT		  0
#define DIO1_PIN		  16
#endif

#ifdef DIO2_ENABLE
#define DIO2_PORT		  0
#define DIO2_PIN		  17
#endif

#ifdef DIO3_ENABLE
#define DIO3_PORT		  0
#define DIO3_PIN		  18
#endif

#ifdef DIO4_ENABLE
#define DIO4_PORT		  0
#define DIO4_PIN		  19
#endif

#ifdef DIO5_ENABLE
#define DIO5_PORT		  0
#define DIO5_PIN		  20
#endif

#ifdef RESET_ENABLE
#define RESET_PORT		  0
#define RESET_PIN		  2
#endif

#endif

//spi
#ifdef BOARD_NXP_LPCXPRESSO_1347
#define LPC_SSP           LPC_SSP1 //LPC_SSP_T
#endif
#ifdef BOARD_NXP_LPCXPRESSO_1343
#define LPC_SSP           LPC_SSP0 //LPC_SSP_T
#endif
#ifdef BOARD_NXP_LPCXPRESSO_1769
#define LPC_SSP           LPC_SSP1 //LPC_SSP_T

#ifdef ALT_CS_ENABLE
#define CS_PORT		  		0
#define CS_PIN		  		3
#endif

#endif

#ifdef CHIP_LPC175X_6X
#define LPC_GPIO_ALLIAS        LPC_GPIO
#endif
#if defined (CHIP_LPC1347) || defined (CHIP_LPC1343)
#define LPC_GPIO_ALLIAS        LPC_GPIO_PORT
#endif


//i2c eeprom settings

#ifdef BOARD_NXP_LPCXPRESSO_1769
	#define EEPROM_I2C          I2C1
	#define SPEED_100KHZ        100000
	#define SPEED_400KHZ        400000
	#define I2C_SPEED			SPEED_100KHZ
	#define EEPROM_ADDRESS		80
	#define EEPROM_SIZE			8096
#endif



#endif /* HWCONFIG_H_ */
