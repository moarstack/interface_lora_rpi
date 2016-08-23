/*
 * lora_reg.h
 *
 *  Created on: 02 дек. 2015 г.
 *      Author: svalov
 */

#ifndef INC_LORA_REG_H_
#define INC_LORA_REG_H_

#include "rfm9x_reg.h"

/* FIFO SPI pointer */
#define LORA_RegFifoAddrPtr (0x0D)
/* Start Tx data */
#define LORA_RegFifoTxBaseAddr (0x0E)
/* Start Rx data */
#define LORA_RegFifoRxBaseAddr (0x0F)
/* LoRaTM state flags */
//#define LORA_RegIrqFlags (0x10)
#define LORA_RegFifoRxCurrentAddr (0x10)
/* Optional flag mask */
#define LORA_RegIrqFlagsMask (0x11)
//#define LORA_ALT_RegIrqFlagsMask (0x11)
/* IF Frequency MSB */
//#define LORA_RegFreqIfMsb (0x12)
#define LORA_RegIrqFlags (0x12)
/* IF Frequency LSB */
//#define LORA_RegFreqIFLsb (0x13)
#define LORA_RegRxNbBytes (0x13)
/* Receiver timeout value MSB */
//#define LORA_RegSymbTimeoutMsb (0x14)
#define LORA_RegRxHeaderCntValueMsb (0x14)
/* Receiver timeout value LSB */
//#define LORA_RegSymbTimeoutLsb (0x15)
#define LORA_RegRxHeaderCntValueLsb (0x15)
/* LoRaTM transmit parameters */
//#define LORA_RegTxCfg (0x16)
#define LORA_RegRxPacketCntValueMsb (0x16)
/* LoRaTM transmit parameters */
//#define LORA_RegPayloadLength (0x17)
#define LORA_RegRxPacketCntValueLsb (0x17)
/* Size of preamble MSB */
//#define LORA_RegPreambleMsb (0x18)
#define LORA_RegModemStat (0x18)
/* Size of preamble LSB */
//#define LORA_RegPreambleLsb (0x19)
#define LORA_RegPktSnrValue (0x19)
/* Modem PHY config */
//#define LORA_RegModulationCfg (0x1A)
#define LORA_RegPktRssiValue (0x1A)
/* Test register */
//#define LORA_RegRfMode (0x1B)
#define LORA_RegRssiValue (0x1B)
/* FHSS Hop period */
//#define LORA_RegHopPeriod (0x1C)
#define LORA_RegHopChannel (0x1C)
/* Number of received bytes */
//#define LORA_RegNbRxBytes (0x1D)
#define LORA_RegModemConfig1 (0x1D)
/* Info from last header */
//#define LORA_RegRxHeaderInfo (0x1E)
#define LORA_RegModemConfig2 (0x1E)
/* Number of valid headers received */
//#define LORA_RegRxHeaderCntValue (0x1F)
#define LORA_RegSymbTimeoutLsb (0x1F)
/* Number of valid packets received */
//#define LORA_RegRxPacketCntValue (0x20)
#define LORA_RegPreambleMsb (0x20)
/* Live LoRaTM modem status */
//#define LORA_RegModemStat (0x21)
#define LORA_RegPreambleLsb (0x21)
/* Estimation of last packet SNR */
//#define LORA_RegPktSnrValue (0x22)
#define LORA_RegPayloadLength (0x22)
/* Current RSSI */
//#define LORA_RegRssiValue (0x23)
#define LORA_RegMaxPayloadLength (0x23)
/* RSSi of last packet */
//#define LORA_RegPktRssiValue (0x24)
#define LORA_RegHopPeriod (0x24)
/* FHSS start channel */
//#define LORA_RegHopChannel (0x25)
#define LORA_RegFifoRxByteAddr (0x25)
/* LoRaTM rx data pointer */
//#define LORA_RegRxDataAddr (0x26)
#define LORA_RegModemConfig3 (0x26)


#define LORA_ENABLE_IRQ			0x00
#define LORA_DISABLE_IRQ		0xFF

#define LORA_IRQ_CAD_DETECTED 			0b00000001
#define LORA_IRQ_FHSS_CHANGE_CHANNEL	0b00000010
#define LORA_IRQ_CAD_DONE				0b00000100
#define LORA_IRQ_TX_DONE			 	0b00001000
#define LORA_IRQ_VALID_HEADER			0b00010000
#define LORA_IRQ_PAYLOAD_CRC_ERROR	 	0b00100000
#define LORA_IRQ_RX_DONE			 	0b01000000
#define LORA_IRQ_RX_TIMEOUT			 	0b10000000

//typedef struct{
//	uint8_t CadDetectedMask:1;
//	uint8_t FhssChangeChannelMask:1;
//	uint8_t CadDoneMask:1;
//	uint8_t TxDoneMask:1;
//	uint8_t ValidHeaderMask:1;
//	uint8_t PayloadCrcErrorMask:1;
//	uint8_t RxDoneMask:1;
//	uint8_t RxTimeoutMask:1;
//} LORA_RegIrqFlagsMask_T;

typedef struct{
	uint8_t CadDetected:1;
	uint8_t FhssChangeChannel:1;
	uint8_t CadDone:1;
	uint8_t TxDone:1;
	uint8_t ValidHeader:1;
	uint8_t PayloadCrcError:1;
	uint8_t RxDone:1;
	uint8_t RxTimeout:1;
} LORA_RegIrqFlags_T;
//
//typedef enum{
//	LORA_IRQ_CadDetected=0,
//	LORA_IRQ_FhssChangeChannel=1,
//	LORA_IRQ_CadDone=2,
//	LORA_IRQ_TxDone=3,
//	LORA_IRQ_ValidHeader=4,
//	LORA_IRQ_PayloadCrcError=5,
//	LORA_IRQ_RxDone=6,
//	LORA_IRQ_RxTimeout=7,
//} LORA_Irq_T;

typedef enum{
	Bw_7800Hz = 	0b0000000,
	Bw_10400Hz = 	0b0000001,
	Bw_15600Hz = 	0b0000010,
	Bw_20800Hz = 	0b0000011,
	Bw_312500Hz = 	0b0000100,
	Bw_41700Hz= 	0b0000101,
	Bw_62500HZ= 	0b0000110,
	Bw_125000Hz= 	0b0000111,
	Bw_250000Hz= 	0b0001000,
	Bw_500000Hz= 	0b0001001,
} LORA_RegModemConfig_Bw_T;

typedef enum{
	CodingRate_4to5 = 	0b0000001,
	CodingRate_4to6 = 	0b0000010,
	CodingRate_4to7 = 	0b0000011,
	CodingRate_4to8 = 	0b0000100,
} LORA_RegModemConfig_CoddingRate_T;

typedef enum{
	AccessSharedReg_LORA = 0x0,
	AccessSharedReg_FSK = 0x1,
} LORA_RegOpMode_AccessSharedReg_T;

typedef enum{
	LORA_DIO0_RxDone = 0b00,
	LORA_DIO0_TxDone = 0b01,
	LORA_DIO0_CadDone = 0b10,
	LORA_DIO0_Reserved = 0b11,
} LORA_DIO0_T;

typedef enum{
	LORA_DIO1_RxTimeout = 0b00,
	LORA_DIO1_FhssChangeChannel = 0b01,
	LORA_DIO1_CadDetected = 0b10,
	LORA_DIO1_Reserved = 0b11,
} LORA_DIO1_T;

typedef enum{
	LORA_DIO2_FhssChangeChannel = 0b00,
	LORA_DIO2_Reserved = 0b11,
} LORA_DIO2_T;

typedef enum{
	LORA_DIO3_CadDone = 0b00,
	LORA_DIO3_ValidHeader = 0b01,
	LORA_DIO3_PayloadCrcError = 0b10,
	LORA_DIO3_Reserved = 0b11,
} LORA_DIO3_T;

typedef enum{
	LORA_DIO4_CadDetected = 0b00,
	LORA_DIO4_PllLock = 0b01,
	LORA_DIO4_Reserved = 0b11,
} LORA_DIO4_T;

typedef enum{
	LORA_DIO5_ModeReady = 0b00,
	LORA_DIO5_ClkOut = 0b01,
	LORA_DIO5_Reserved = 0b11,
} LORA_DIO5_T;

/* 0x01 - Operating mode & LoRaTM / FSK selection */
typedef struct{
	RFM9X_RegOpMode_Mode_T Mode:3;
	RFM9X_RegOpMode_LowFrequencyModeOn_T LowFrequencyModeOn:1;
	uint8_t Reserved:2;
	LORA_RegOpMode_AccessSharedReg_T AccessSharedReg:1;
	RFM9X_RegOpMode_LongRangeMode_T LongRangeMode:1;
} LORA_RegOpMode_T;

typedef struct{
	uint8_t FhssPresentChannel:6;
	uint8_t RxPayloadCrc:1;
	uint8_t PllTimeout:1;
} LORA_RegHopChannel_T;

typedef struct{
	uint8_t ImplicitHeaderModeOn:1;
	LORA_RegModemConfig_CoddingRate_T CodingRate:3;
	LORA_RegModemConfig_Bw_T Bw:4;
} LORA_RegModemConfig1_T;

typedef struct{
	uint8_t SymbTimeout:2;
	uint8_t RxPayloadCrcOn:1;
	uint8_t TxContinuousMode:1;
	uint8_t SpreadingFactor:4;
} LORA_RegModemConfig2_T;

typedef struct{
	uint8_t Reserved:2;
	uint8_t AgcAutoOn:1;
	uint8_t MobileNode:1;
	uint8_t Unused:4;
} LORA_RegModemConfig3_T;

typedef struct{
	uint8_t SignalDetected:1;
	uint8_t SignalSync:1;
	uint8_t RxOngoing:1;
	uint8_t HeaderInfoValid:1;
	uint8_t ModemClear:1;
	LORA_RegModemConfig_CoddingRate_T CoddingRateLastHeader:3;
} LORA_RegModemStat_T;

typedef struct{
	uint8_t RegFifo; /* 0x00 - FIFO read/write access */
	LORA_RegOpMode_T RegOpMode; /* 0x01 - Operating mode & LoRaTM / FSK selection */
	uint8_t Unused0[0x04];
	uint8_t RegFrfMsb; /* 0x06 - RF Carrier Frequency, Most Significant Bits */
	uint8_t RegFrfMid; /* 0x07 - RF Carrier Frequency, Intermediate Bits */
	uint8_t RegFrfLsb ; /* 0x08 - RF Carrier Frequency, Least Significant Bits */
	RFM9X_RegPaConfig_T RegPaConfig; /* 0x09 - PA selection and Output Power control */
	uint8_t RegPaRamp ; /* 0x0A - Control of PA ramp time, low phase noise PLL */
	uint8_t RegOcp; /* 0x0B - Over Current Protection control */
	RFM9X_RegLna_T RegLna; /* 0x0C - LNA settings */
	uint8_t RegFifoAddrPtr; /* 0x0D - FIFO SPI pointer */
	uint8_t RegFifoTxBaseAddr; /* 0x0E - Start Tx data */
	uint8_t RegFifoRxBaseAddr; /* 0x0F - Start Rx data */
	uint8_t RegFifoRxCurrentAddr; /* 0x10 -  */
	LORA_RegIrqFlags_T RegIrqFlagsMask; /* 0x11 - Optional flag mask */
	LORA_RegIrqFlags_T RegIrqFlags; /* 0x12 - */
	uint8_t RegRxNbBytes; /* 0x13 -  */
	uint8_t RegRxHeaderCntValueMsb; /* 0x14 - */
	uint8_t RegRxHeaderCntValueLsb; /* 0x15 - */
	uint8_t RegRxPacketCntValueMsb; /* 0x16 - */
	uint8_t RegRxPacketCntValueLsb; /* 0x17 - */
	LORA_RegModemStat_T RegModemStat; /* 0x18 - */
	uint8_t RegPktSnrValue; /* 0x19 - */
	uint8_t RegPktRssiValue; /* 0x1A - */
	uint8_t RegRssiValue; /* 0x1B - */
	LORA_RegHopChannel_T RegHopChannel; /* 0x1C - FHSS Hop period */
	LORA_RegModemConfig1_T RegModemConfig1; /* 0x1D - */
	LORA_RegModemConfig2_T RegModemConfig2; /* 0x1E - */
	uint8_t RegSymbTimeoutLsb; /* 0x1F - */
	uint8_t RegPreambleMsb; /* 0x20 - */
	uint8_t RegPreambleLsb; /* 0x21 - */
	uint8_t RegPayloadLength; /* 0x22 - */
	uint8_t RegMaxPayloadLength; /* 0x23 - */
	uint8_t RegHopPeriod; /* 0x24 - */
	uint8_t RegFifoRxByteAddr; /* 0x25 - */
	LORA_RegModemConfig3_T RegModemConfig3; /* 0x26 - */
	uint8_t Unused1[0x19];
	uint8_t RegDioMapping1; /* 0x40 - Mapping of pins DIO0 to DIO3 */
	uint8_t RegDioMapping2; /* 0x41 - Mapping of pins DIO4 and DIO5, ClkOut frequency */
	uint8_t RegVersion; /* 0x42 - Hope RF ID relating the silicon revision */
	uint8_t Unused2[0x08];
	uint8_t RegTcxo; /* 0x4B - TCXO or XTAL input setting */
	uint8_t Unused3[0x01];
	uint8_t RegPaDac; /* 0x4D - Higher power settings of the PA */
	uint8_t Unused4[0x0D];
	uint8_t RegFormerTemp; /* 0x5B - Stored temperature during the former IQ Calibration */
	uint8_t Unused5[0x05];
	uint8_t RegAgcRef; /* 0x61 - Adjustment of the AGC thresholds */
	uint8_t RegAgcThresh1; /* 0x62 - Adjustment of the AGC thresholds */
	uint8_t RegAgcThresh2; /* 0x63 - Adjustment of the AGC thresholds */
	uint8_t RegAgcThresh3; /* 0x64 - Adjustment of the AGC thresholds */
}LORA_REG_T;

//typedef struct{
//	uint8_t RegFifo; /* 0x00 - FIFO read/write access */
//	LORA_RegOpMode_T RegOpMode; /* 0x01 - Operating mode & LoRaTM / FSK selection */
//	uint8_t Unused0[0x04];
//	uint8_t RegFrfMsb; /* 0x06 - RF Carrier Frequency, Most Significant Bits */
//	uint8_t RegFrfMid; /* 0x07 - RF Carrier Frequency, Intermediate Bits */
//	uint8_t RegFrfLsb ; /* 0x08 - RF Carrier Frequency, Least Significant Bits */
//	RFM9X_RegPaConfig_T RegPaConfig; /* 0x09 - PA selection and Output Power control */
//	uint8_t RegPaRamp ; /* 0x0A - Control of PA ramp time, low phase noise PLL */
//	uint8_t RegOcp; /* 0x0B - Over Current Protection control */
//	RFM9X_RegLna_T RegLna; /* 0x0C - LNA settings */
//	uint8_t RegFifoAddrPtr; /* 0x0D - FIFO SPI pointer */
//	uint8_t RegFifoTxBaseAddr; /* 0x0E - Start Tx data */
//	uint8_t RegFifoRxBaseAddr; /* 0x0F - Start Rx data */
//	//RegFifoRxCurrentAddr here?
//	LORA_RegIrqFlags_T RegIrqFlags; /* 0x10 - LoRaTM state flags */
//	LORA_RegIrqFlags_T RegIrqFlagsMask; /* 0x11 - Optional flag mask */
//	uint8_t RegFreqIfMsb; /* 0x12 - IF Frequency MSB */
//	uint8_t RegFreqIFLsb; /* 0x13 - IF Frequency LSB */
//	uint8_t RegSymbTimeoutMsb; /* 0x14 - Receiver timeout value MSB */
//	uint8_t RegSymbTimeoutLsb; /* 0x15 - Receiver timeout value LSB */
//	uint8_t RegTxCfg; /* 0x16 - LoRaTM transmit parameters */
//	uint8_t RegPayloadLength; /* 0x17 - LoRaTM transmit parameters */
//	uint8_t RegPreambleMsb; /* 0x18 - Size of preamble MSB */
//	uint8_t RegPreambleLsb; /* 0x19 - Size of preamble LSB */
//	uint8_t RegModulationCfg; /* 0x1A - Modem PHY config */
//	uint8_t RegRfMode; /* 0x1B - Test register */
//	uint8_t RegHopPeriod; /* 0x1C - FHSS Hop period */
//	uint8_t RegNbRxBytes; /* 0x1D - Number of received bytes */
//	uint8_t RegRxHeaderInfo; /* 0x1E - Info from last header */
//	uint8_t RegRxHeaderCntValue; /* 0x1F - Number of valid headers received */
//	uint8_t RegRxPacketCntValue; /* 0x20 - Number of valid packets received */
//	uint8_t RegModemStat; /* 0x21 - Live LoRaTM modem status */
//	uint8_t RegPktSnrValue; /* 0x22 - Estimation of last packet SNR */
//	uint8_t RegRssiValue; /* 0x23 - Current RSSI */
//	uint8_t RegPktRssiValue; /* 0x24 - RSSi of last packet */
//	uint8_t RegHopChannel; /* 0x25 - FHSS start channel */
//	uint8_t RegRxDataAddr; /* 0x26 - LoRaTM rx data pointer */
//	uint8_t Unused1[0x19];
//	uint8_t RegDioMapping1; /* 0x40 - Mapping of pins DIO0 to DIO3 */
//	uint8_t RegDioMapping2; /* 0x41 - Mapping of pins DIO4 and DIO5, ClkOut frequency */
//	uint8_t RegVersion; /* 0x42 - Hope RF ID relating the silicon revision */
//	uint8_t Unused2[0x08];
//	uint8_t RegTcxo; /* 0x4B - TCXO or XTAL input setting */
//	uint8_t Unused3[0x01];
//	uint8_t RegPaDac; /* 0x4D - Higher power settings of the PA */
//	uint8_t Unused4[0x0D];
//	uint8_t RegFormerTemp; /* 0x5B - Stored temperature during the former IQ Calibration */
//	uint8_t Unused5[0x05];
//	uint8_t RegAgcRef; /* 0x61 - Adjustment of the AGC thresholds */
//	uint8_t RegAgcThresh1; /* 0x62 - Adjustment of the AGC thresholds */
//	uint8_t RegAgcThresh2; /* 0x63 - Adjustment of the AGC thresholds */
//	uint8_t RegAgcThresh3; /* 0x64 - Adjustment of the AGC thresholds */
//}LORA_REG_OLD_T;
#endif /* INC_LORA_REG_H_ */
