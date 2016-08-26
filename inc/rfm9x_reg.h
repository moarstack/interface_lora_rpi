/*
 * rfm9x_reg.h
 *
 *  Created on: 03 дек. 2015 г.
 *      Author: svalov
 */

#ifndef INC_RFM9X_REG_H_
#define INC_RFM9X_REG_H_

/* FIFO read/write access */
#define RFM9X_RegFifo (0x00)
/* Operating mode & RFM9XTM / FSK selection */
#define RFM9X_RegOpMode (0x01)
/* RF Carrier Frequency, Most Significant Bits */
#define RFM9X_RegFrfMsb (0x06)
/* RF Carrier Frequency, Intermediate Bits */
#define RFM9X_RegFrfMid (0x07)
/* RF Carrier Frequency, Least Significant Bits */
#define RFM9X_RegFrfLsb  (0x08)
/* PA selection and Output Power control */
#define RFM9X_RegPaConfig (0x09)
/* Control of PA ramp time, low phase noise PLL */
#define RFM9X_RegPaRamp  (0x0A)
/* Over Current Protection control */
#define RFM9X_RegOcp (0x0B)
/* LNA settings */
#define RFM9X_RegLna (0x0C)
/* Mapping of pins DIO0 to DIO3 */
#define RFM9X_RegDioMapping1 (0x40)
/* Mapping of pins DIO4 and DIO5, ClkOut frequency */
#define RFM9X_RegDioMapping2 (0x41)
/* Hope RF ID relating the silicon revision */
#define RFM9X_RegVersion (0x42)
/* TCXO or XTAL input setting */
#define RFM9X_RegTcxo (0x4B)
/* Higher power settings of the PA */
#define RFM9X_RegPaDac (0x4D)
/* Stored temperature during the former IQ Calibration */
#define RFM9X_RegFormerTemp (0x5B)
/* Adjustment of the AGC thresholds */
#define RFM9X_RegAgcRef (0x61)
/* Adjustment of the AGC thresholds */
#define RFM9X_RegAgcThresh1 (0x62)
/* Adjustment of the AGC thresholds */
#define RFM9X_RegAgcThresh2 (0x63)
/* Adjustment of the AGC thresholds */
#define RFM9X_RegAgcThresh3 (0x64)

typedef enum{
	Mode_SLEEP = 0b00000000,
	Mode_STANDBY = 0b00000001,
	Mode_FSTX = 0b00000010,
	Mode_TX =	0b00000011,
	Mode_FSRX	= 0b00000100,
	Mode_RX =	0b00000101,
	Mode_RXCONTINUOUS = 0b00000101,
	//Mode_RESERVED	= 0b00000110,
	Mode_RXSINGLE = 0b00000110,
	//Mode_RESERVED1 = 0b00000111,
	Mode_CAD =	0b00000111,
} RFM9X_RegOpMode_Mode_T;

typedef enum{
	LongRangeMode_FSK	= 0x0,
	LongRangeMode_LORA = 0x1,
} RFM9X_RegOpMode_LongRangeMode_T;

typedef enum{
	ModulationType_FSK = 0x0,
	ModulationType_OOK = 0x1,
	ModulationType_Reserved = 0x02,
	ModulationType_Reserved1 = 0x03,
} RFM9X_RegOpMode_ModulationType_T;

typedef enum{
	LowFrequencyModeOnE_HF = 0x0,
	LowFrequencyModeOn_LF = 0x1,
} RFM9X_RegOpMode_LowFrequencyModeOn_T;

typedef enum{
	RFM9X_DIO0=0,
	RFM9X_DIO1=1,
	RFM9X_DIO2=2,
	RFM9X_DIO3=3,
	RFM9X_DIO4=4,
	RFM9X_DIO5=5,
} RFM9X_DIO_T;

/* 0x01 - Operating mode & LoRaTM / FSK selection */
typedef struct{
	RFM9X_RegOpMode_Mode_T Mode:3;
	RFM9X_RegOpMode_LowFrequencyModeOn_T LowFrequencyModeOn:1;
	uint8_t Reserved:1;
	RFM9X_RegOpMode_ModulationType_T ModulationType:2;
	RFM9X_RegOpMode_LongRangeMode_T LongRangeMode:1;
} RFM9X_RegOpMode_T;

typedef struct{
	uint8_t OutputPower:4;
	uint8_t MaxPower:3;
	uint8_t PaSelect:1;
} RFM9X_RegPaConfig_T;

typedef struct{
	uint8_t LnaBoostHf:2;
	uint8_t Reserved:1;
	uint8_t LnaBoostLf:2;
	uint8_t LnaGain:3;
} RFM9X_RegLna_T;

typedef struct {
	uint8_t RegFifo; /* 0x00 - FIFO read/write access */

	//	uint8_t RegOpMode_MODE:3; /* 0x01 - Operating mode & LoRaTM / FSK selection */
	//	uint8_t RegOpMode_FREQ:1; /* 0x01 - Operating mode & LoRaTM / FSK selection */
	//	uint8_t RegOpMode_RESERVED:1; /* 0x01 - Operating mode & LoRaTM / FSK selection */
	//	uint8_t RegOpMode_MODULATION:2; /* 0x01 - Operating mode & LoRaTM / FSK selection */
	//	uint8_t RegOpMode_LORA:1; /* 0x01 - Operating mode & LoRaTM / FSK selection */
	//uint8_t RegOpMode; /* 0x01 - Operating mode & LoRaTM / FSK selection */
	RFM9X_RegOpMode_T RegOpMode;
	uint8_t Unsed0[0x04];
	uint8_t RegFrfMsb; /* 0x06 - RF Carrier Frequency, Most Significant Bits */
	uint8_t RegFrfMid; /* 0x07 - RF Carrier Frequency, Intermediate Bits */
	uint8_t RegFrfLsb; /* 0x08 - RF Carrier Frequency, Least Significant Bits */
	RFM9X_RegPaConfig_T RegPaConfig; /* 0x09 - PA selection and Output Power control */
	uint8_t RegPaRamp ; /* 0x0A - Control of PA ramp time, low phase noise PLL */
	uint8_t RegOcp; /* 0x0B - Over Current Protection control */
	RFM9X_RegLna_T RegLna; /* 0x0C - LNA settings */
	uint8_t Unsed1[0x33];
	uint8_t RegDioMapping1; /* 0x40 - Mapping of pins DIO0 to DIO3 */
	uint8_t RegDioMapping2; /* 0x41 - Mapping of pins DIO4 and DIO5, ClkOut frequency */
	uint8_t RegVersion; /* 0x42 - Hope RF ID relating the silicon revision */
	uint8_t Unused2[0x08];
	uint8_t RegTcxo; /* 0x4B - TCXO or XTAL input setting */
	uint8_t RegPaDac; /* 0x4D - Higher power settings of the PA */
	uint8_t Unused3[0x0E];
	uint8_t RegFormerTemp; /* 0x5B - Stored temperature during the former IQ Calibration */
	uint8_t Unused4[0x05];
	uint8_t RegAgcRef; /* 0x61 - Adjustment of the AGC thresholds */
	uint8_t RegAgcThresh1; /* 0x62 - Adjustment of the AGC thresholds */
	uint8_t RegAgcThresh2; /* 0x63 - Adjustment of the AGC thresholds */
	uint8_t RegAgcThresh3; /* 0x64 - Adjustment of the AGC thresholds */

} RFM9X_REG_T;

#endif /* INC_RFM9X_REG_H_ */
