/*
 * fsk_reg.h
 *
 *  Created on: 02 дек. 2015 г.
 *      Author: svalov
 */

#ifndef INC_FSK_REG_H_
#define INC_FSK_REG_H_

#include "rfm9x_reg.h"

#define FSK_RegBitrateMsb (0x02)
/* Bit Rate setting, Least Significant Bits */
#define FSK_RegBitrateLsb (0x03)
/* Frequency Deviation setting, Most Significant Bits */
#define FSK_RegFdevMsb (0x04)
/* Frequency Deviation setting, Least Significant Bits */
#define FSK_RegFdevLsb (0x05)
/* FC, AGC, ctrl */
#define FSK_RegRxConfig  (0x0D)
/* RSSI */
#define FSK_RegRssiConfig (0x0E)
/* RSSI Collision detector */
#define FSK_RegRssiCollision  (0x0F)
/* RSSI Threshold control */
#define FSK_RegRssiThresh  (0x10)
/* RSSI value in dBm */
#define FSK_RegRssiValue  (0x11)
/* Channel Filter BW Control */
#define FSK_RegRxBw  (0x12)
/* AFC Channel Filter BW */
#define FSK_RegAfcBw  (0x13)
/* OOK demodulator */
#define FSK_RegOokPeak  (0x14)
/* Threshold of the OOK demod */
#define FSK_RegOokFix  (0x15)
/* Average of the OOK demod */
#define FSK_RegOokAvg  (0x16)
/* AFC and FEI control */
#define FSK_RegAfcFei  (0x1A)
/* Frequency correction value of the AFC MSB */
#define FSK_RegAfcMsb (0x1B)
/* Frequency correction value of the AFC MSB */
#define FSK_RegAfcLsb (0x1C)
/* Value of the calculated frequency error */
#define FSK_RegFeiMsb (0x1D)
/* Value of the calculated frequency error */
#define FSK_RegFeiLsb (0x1E)
/* Settings of the Preamble Detector */
#define FSK_RegPreambleDetect (0x1F)
/* Timeout Rx request and RSSI */
#define FSK_RegRxTimeout1 (0x20)
/* Timeout RSSI and PayloadReady */
#define FSK_RegRxTimeout2 (0x21)
/* Timeout RSSI and SyncAddress */
#define FSK_RegRxTimeout3 (0x22)
/* Delay between Rx cycles */
#define FSK_RegRxDelay (0x23)
/* RC Oscillators Settings, CLKOUT frequency */
#define FSK_RegOsc (0x24)
/* Preamble length, MSB */
#define FSK_RegPreambleMsb (0x25)
/* Preamble length, LSB */
#define FSK_RegPreambleLsb (0x26)
/* Sync Word Recognition control */
#define FSK_RegSyncConfig (0x27)
//regsync
/* Packet mode settings */
#define FSK_RegPacketConfig1 (0x30)
/* Packet mode settings */
#define FSK_RegPacketConfig2 (0x31)
/* Payload length setting */
#define FSK_RegPayloadLength (0x32)
/* Node address */
#define FSK_RegNodeAdrs (0x33)
/* Broadcast address */
#define FSK_RegBroadcastAdrs (0x34)
/* Fifo threshold, Tx start condition */
#define FSK_RegFifoThresh (0x35)
/* Top level Sequencer settings */
#define FSK_RegSeqConfig1 (0x36)
/* Top level Sequencer settings */
#define FSK_RegSeqConfig2 (0x37)
/* Timer 1 and 2 resolution control */
#define FSK_RegTimerResol (0x38)
/* Timer 1 setting */
#define FSK_RegTimer1Coef  (0x39)
/* Timer 1 setting */
#define FSK_RegTimer2Coef  (0x3A)
/* Image calibration engine control */
#define FSK_RegImageCal (0x3B)
/* Temperature Sensor value */
#define FSK_RegTemp (0x3C)
/* Low Battery Indicator Setings */
#define FSK_RegLowBat (0x3D)
/* Status register: PLL Lock state, Timeout, RSSI */
#define FSK_RegIrqFlags1 (0x3E)
/* Status register: FIFO handling flags, Low Battery */
#define FSK_RegIrqFlags2 (0x3F)
/* Control the fast frequency hopping mode */
#define FSK_RegPllHop (0x44)
/* TCXO or XTAL input setting */
#define FSK_RegBitRateFrac (0x5D)

typedef struct {
	uint8_t RegFifo; /* 0x00 - FIFO read/write access */
	RFM9X_RegOpMode_T RegOpMode; /* 0x01 - Operating mode & LoRaTM / FSK selection */
	uint8_t RegBitrateMsb; /* 0x02 - Bit Rate setting, Most Significant Bits */
	uint8_t RegBitrateLsb; /* 0x03 - Bit Rate setting, Least Significant Bits */
	uint8_t RegFdevMsb; /* 0x04 - Frequency Deviation setting, Most Significant Bits */
	uint8_t RegFdevLsb; /* 0x05 - Frequency Deviation setting, Least Significant Bits */
	uint8_t RegFrfMsb; /* 0x06 - RF Carrier Frequency, Most Significant Bits */
	uint8_t RegFrfMid; /* 0x07 - RF Carrier Frequency, Intermediate Bits */
	uint8_t RegFrfLsb; /* 0x08 - RF Carrier Frequency, Least Significant Bits */
	RFM9X_RegPaConfig_T RegPaConfig; /* 0x09 - PA selection and Output Power control */
	uint8_t RegPaRamp ; /* 0x0A - Control of PA ramp time, low phase noise PLL */
	uint8_t RegOcp; /* 0x0B - Over Current Protection control */
	RFM9X_RegLna_T RegLna; /* 0x0C - LNA settings */
	uint8_t RegRxConfig ; /* 0x0D - FC, AGC, ctrl */
	uint8_t RegRssiConfig; /* 0x0E - RSSI */
	uint8_t RegRssiCollision ; /* 0x0F - RSSI Collision detector */
	uint8_t RegRssiThresh ; /* 0x10 - RSSI Threshold control */
	uint8_t RegRssiValue ; /* 0x11 - RSSI value in dBm */
	uint8_t RegRxBw ; /* 0x12 - Channel Filter BW Control */
	uint8_t RegAfcBw ; /* 0x13 - AFC Channel Filter BW */
	uint8_t RegOokPeak ; /* 0x14 - OOK demodulator */
	uint8_t RegOokFix ; /* 0x15 - Threshold of the OOK demod */
	uint8_t RegOokAvg ; /* 0x16 - Average of the OOK demod */
	uint8_t Unused0[0x03];
	uint8_t RegAfcFei ; /* 0x1A - AFC and FEI control */
	uint8_t RegAfcMsb; /* 0x1B - Frequency correction value of the AFC MSB */
	uint8_t RegAfcLsb; /* 0x1C - Frequency correction value of the AFC MSB */
	uint8_t RegFeiMsb; /* 0x1D - Value of the calculated frequency error */
	uint8_t RegFeiLsb; /* 0x1E - Value of the calculated frequency error */
	uint8_t RegPreambleDetect; /* 0x1F - Settings of the Preamble Detector */
	uint8_t RegRxTimeout1; /* 0x20 - Timeout Rx request and RSSI */
	uint8_t RegRxTimeout2; /* 0x21 - Timeout RSSI and PayloadReady */
	uint8_t RegRxTimeout3; /* 0x22 - Timeout RSSI and SyncAddress */
	uint8_t RegRxDelay; /* 0x23 - Delay between Rx cycles */
	uint8_t RegOsc; /* 0x24 - RC Oscillators Settings, CLKOUT frequency */
	uint8_t RegPreambleMsb; /* 0x25 - Preamble length, MSB */
	uint8_t RegPreambleLsb; /* 0x26 - Preamble length, LSB */
	uint8_t RegSyncConfig; /* 0x27 - Sync Word Recognition control */
	uint8_t RegSyncValue1; /* 0x28 - Sync Word bytes 1 */
	uint8_t RegSyncValue2; /* 0x29 - Sync Word bytes 2 */
	uint8_t RegSyncValue3; /* 0x2A - Sync Word bytes 3 */
	uint8_t RegSyncValue4; /* 0x2B - Sync Word bytes 4 */
	uint8_t RegSyncValue5; /* 0x2C - Sync Word bytes 5 */
	uint8_t RegSyncValue6; /* 0x2D - Sync Word bytes 6 */
	uint8_t RegSyncValue7; /* 0x2E - Sync Word bytes 7 */
	uint8_t RegSyncValue8; /* 0x2F - Sync Word bytes 8 */
	uint8_t RegPacketConfig1; /* 0x30 - Packet mode settings */
	uint8_t RegPacketConfig2; /* 0x31 - Packet mode settings */
	uint8_t RegPayloadLength; /* 0x32 - Payload length setting */
	uint8_t RegNodeAdrs; /* 0x33 - Node address */
	uint8_t RegBroadcastAdrs; /* 0x34 - Broadcast address */
	uint8_t RegFifoThresh; /* 0x35 - Fifo threshold, Tx start condition */
	uint8_t RegSeqConfig1; /* 0x36 - Top level Sequencer settings */
	uint8_t RegSeqConfig2; /* 0x37 - Top level Sequencer settings */
	uint8_t RegTimerResol; /* 0x38 - Timer 1 and 2 resolution control */
	uint8_t RegTimer1Coef ; /* 0x39 - Timer 1 setting */
	uint8_t RegTimer2Coef ; /* 0x3A - Timer 1 setting */
	uint8_t RegImageCal; /* 0x3B - Image calibration engine control */
	uint8_t RegTemp; /* 0x3C - Temperature Sensor value */
	uint8_t RegLowBat; /* 0x3D - Low Battery Indicator Setings */
	uint8_t RegIrqFlags1; /* 0x3E - Status register: PLL Lock state, Timeout, RSSI */
	uint8_t RegIrqFlags2; /* 0x3F - Status register: FIFO handling flags, Low Battery */
	uint8_t RegDioMapping1; /* 0x40 - Mapping of pins DIO0 to DIO3 */
	uint8_t RegDioMapping2; /* 0x41 - Mapping of pins DIO4 and DIO5, ClkOut frequency */
	uint8_t RegVersion; /* 0x42 - Hope RF ID relating the silicon revision */
	uint8_t Unused1[0x01];
	uint8_t RegPllHop; /* 0x44 - Control the fast frequency hopping mode */
	uint8_t Unused2[0x06];
	uint8_t RegTcxo; /* 0x4B - TCXO or XTAL input setting */
	uint8_t RegPaDac; /* 0x4D - Higher power settings of the PA */
	uint8_t Unused3[0x0E];
	uint8_t RegFormerTemp; /* 0x5B - Stored temperature during the former IQ Calibration */
	uint8_t Unused4[0x01];
	uint8_t RegBitRateFrac; /* 0x5D - Fractional part in the Bit Rate division ratio */
	uint8_t Unused5[0x03];
	uint8_t RegAgcRef; /* 0x61 - Adjustment of the AGC thresholds */
	uint8_t RegAgcThresh1; /* 0x62 - Adjustment of the AGC thresholds */
	uint8_t RegAgcThresh2; /* 0x63 - Adjustment of the AGC thresholds */
	uint8_t RegAgcThresh3; /* 0x64 - Adjustment of the AGC thresholds */

} FSK_REG_T;

#endif /* INC_FSK_REG_H_ */
