/*
 * loraSettings.h
 *
 *  Created on: 03 февр. 2016 г.
 *      Author: svalov
 */

#ifndef LORASETTINGS_H_
#define LORASETTINGS_H_

#include "lora.h"

//default lora settings

//channel bandwidth
#define MIN_CHANNEL_BANDWIDTH 0xCCF
//min freq
#define MIN_FREQUENCY 0x00E10000
//max freq
#define MAX_FREQUENCY 0x00E4C000
//channels count
#define CHANNELS_COUNT 64

//#define LORA_PABOOST_FORCE

//power
#if defined(BOARD_NXP_LPCXPRESSO_1347) && !defined(LORA_PABOOST_FORCE)
#define LORA_PABOOST 0x00
#else
#define LORA_PABOOST 0x01
//#define LORA_PABOOST 0x00
#define LORA_PABOOST_ENABLE
#endif
#define LORA_MAXPOWER 0x00
#define LORA_OUTPUTPOWER 0x0F

#define LORA_MAXBOOSTOUTPUTPOWER 17
#define LORA_MINBOOSTOUTPUTPOWER 2

#define LORA_MAXNOBOOSTOUTPUTPOWER 15
#define LORA_MINNOBOOSTOUTPUTPOWER -4

#ifdef LORA_PABOOST_ENABLE
#define LORA_MAXOUTPUTPOWER LORA_MAXBOOSTOUTPUTPOWER
#else
#define LORA_MAXOUTPUTPOWER LORA_MAXNOBOOSTOUTPUTPOWER
#endif

#ifdef LORA_PABOOST_ENABLE
#define LORA_MINOUTPUTPOWER LORA_MINBOOSTOUTPUTPOWER
#else
#define LORA_MINOUTPUTPOWER LORA_MINNOBOOSTOUTPUTPOWER
#endif

//channel settings
#define LORA_BANDWIDTH Bw_125000Hz
#define LORA_CODDINGRATE CodingRate_4to5
#define LORA_IMPLICITHEADER 0
//preamble
#define LORA_PREAMBLE 32
#define LORA_HEADER_OVERHEAD 4
#define LORA_CRC_OVERHEAD 2
#define LORA_PREAMBLE_CONST_OVERHEAD 5
#define LORA_MESSAGE_OVERHEAD (LORA_HEADER_OVERHEAD + LORA_CRC_OVERHEAD + LORA_PREAMBLE_CONST_OVERHEAD)
//timeout
#define LORA_TIMEOUT (0x3FF>>6)
//hops
#define LORA_HOPPERIOD 8
//sf
#define LORA_SPREADINGFACTOR 7
//crc on rx
/*Enable CRC generation and check on payload:
0 - CRC disable
1 - CRC enable
If CRC is needed, RxPayloadCrcOn should be set:
- in Implicit header mode: on Tx and Rx side
- in Explicit header mode: on the Tx side alone (recovered from
the header in Rx side) */
#define LORA_RXPAYLOADCRC 0
#endif /* LORASETTINGS_H_ */
