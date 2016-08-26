/*
 * lora.h
 *
 *  Created on: 05 дек. 2015 г.
 *      Author: svalov
 */

#ifndef INC_LORA_H_
#define INC_LORA_H_

#include "rfm9x.h"
#include "lora_reg.h"

#define LORA_MODE_IGNORE 0
#define LORA_MODE_RESET 1
#define LORA_MODE_STOP 2
#define LORA_RSSI_OFFSET -137

#define LORA_MAXBOOSTOUTPUTPOWER 17
#define LORA_MINBOOSTOUTPUTPOWER 2
#define LORA_MAXNOBOOSTOUTPUTPOWER 15
#define LORA_MINNOBOOSTOUTPUTPOWER -4

typedef enum{
	LORA_RX_None,
	LORA_RX_Single,
	LORA_RX_Continiuos,
}LORA_RxMode_T;

typedef struct{
	RFM9X_Settings_T RFM9X_Settings;
	//	//min freq
	//	#define MIN_FREQUENCY 0x00E10000
	uint32_t MinFrequency;
	//	//max freq
	//	#define MAX_FREQUENCY 0x00E4C000
	uint32_t MaxFrequency;
	//	//channel bandwidth
	//	#define MIN_CHANNEL_BANDWIDTH 0xCCF
	uint16_t MinChannelBandWidth;
	//	//channels count
	//	#define CHANNELS_COUNT 64
	uint8_t ChannelsCount;

//	//#define LORA_PABOOST_FORCE
//	//power
//	#if defined(BOARD_NXP_LPCXPRESSO_1347) && !defined(LORA_PABOOST_FORCE)
//	#define LORA_PABOOST 0x00
//	#else
//	//#define LORA_PABOOST 0x01
//	#define LORA_PABOOST 0x00
//	//#define LORA_PABOOST_ENABLE
//	#endif
//	#define LORA_MAXPOWER 0x00
//	#define LORA_OUTPUTPOWER 0x0F
	int8_t MaxPower; //default value for iface init
	int8_t OutputPower; //default value for iface init
//
//	#ifdef LORA_PABOOST_ENABLE
//	#define LORA_MAXOUTPUTPOWER LORA_MAXBOOSTOUTPUTPOWER
//	#else
//	#define LORA_MAXOUTPUTPOWER LORA_MAXNOBOOSTOUTPUTPOWER
//	#endif
//
//	#ifdef LORA_PABOOST_ENABLE
//	#define LORA_MINOUTPUTPOWER LORA_MINBOOSTOUTPUTPOWER
//	#else
//	#define LORA_MINOUTPUTPOWER LORA_MINNOBOOSTOUTPUTPOWER
//	#endif
	bool UseBoost; //default value for iface init
	//int8_t MaxOutputPower; //unused
	//int8_t MinOutputPower; //unused
//	//channel settings
//	#define LORA_BANDWIDTH Bw_125000Hz
	LORA_RegModemConfig_Bw_T Bandwidth;
//	#define LORA_CODDINGRATE CodingRate_4to5
	LORA_RegModemConfig_CoddingRate_T CoddingRate;
//	#define LORA_IMPLICITHEADER 0
	bool UseImplicitHeader;
//	//preamble
//	#define LORA_PREAMBLE 32
	uint16_t Preamble;
//	#define LORA_HEADER_OVERHEAD 4
//	#define LORA_CRC_OVERHEAD 2
//	#define LORA_PREAMBLE_CONST_OVERHEAD 5
//	#define LORA_MESSAGE_OVERHEAD (LORA_HEADER_OVERHEAD + LORA_CRC_OVERHEAD + LORA_PREAMBLE_CONST_OVERHEAD)
	//uint16_t LoraMessageOverhead;
//	//timeout
//	#define LORA_TIMEOUT (0x3FF>>6)
	uint16_t Timeout;
//	//hops
//	#define LORA_HOPPERIOD 8
	uint8_t HopPeriod;
//	//sf
//	#define LORA_SPREADINGFACTOR 7
	uint8_t SpreadingFactor;
//	//crc on rx
//	/*Enable CRC generation and check on payload:
//	0 - CRC disable
//	1 - CRC enable
//	If CRC is needed, RxPayloadCrcOn should be set:
//	- in Implicit header mode: on Tx and Rx side
//	- in Explicit header mode: on the Tx side alone (recovered from
//	the header in Rx side) */
//	#define LORA_RXPAYLOADCRC 0
	bool RxPayloadCRC;
}LORA_Settings_T;

typedef uint8_t (*irqHandler)(LORA_RegIrqFlags_T flags);
typedef uint32_t (*fhssFreqRequest)(uint8_t channel);
//event handlers
typedef void (*onRxDoneHandler)(uint8_t *data, uint16_t size, int16_t rssi, LORA_RxMode_T mode);
typedef void (*onRxTimeoutHandler)();
typedef void (*onTxDoneHandler)();
typedef void (*onCadDoneHandler)();
typedef void (*onCadDetectedHandler)();

void LORA_SetIrqHandler(irqHandler handler);
void LORA_SetRxDoneHandler(onRxDoneHandler handler);
void LORA_SetRxTimeoutHandler(onRxTimeoutHandler handler);
void LORA_SetTxDoneHandler(onTxDoneHandler handler);
void LORA_SetCadDoneHandler(onCadDoneHandler handler);
void LORA_SetCadDetectedHandler(onCadDetectedHandler handler);

uint8_t LORA_GetHopChannel();
uint8_t LORA_GetHopPeriod();
void LORA_SetHopPeriod(fhssFreqRequest request, uint8_t period);

LORA_RegOpMode_T LORA_GetOpMode();
void LORA_SetOpMode(LORA_RegOpMode_T mode);

void LORA_SwitchToStandby();

LORA_RegOpMode_AccessSharedReg_T LORA_GetRegMap();
void LORA_SetRegMap(LORA_RegOpMode_AccessSharedReg_T reg);

LORA_RegIrqFlags_T LORA_GetIrqFlagsMask();
uint8_t LORA_GetIrqFlagsMaskUint();

void LORA_SetIrqFlagsMask(LORA_RegIrqFlags_T mask);
void LORA_SetIrqFlagsMaskUint(uint8_t mask);

void LORA_EnableIrq();
void LORA_DisableIrq();

LORA_RegIrqFlags_T LORA_GetIrqFlags();
uint8_t LORA_GetIrqFlagsUint();

void LORA_ResetIrqFlags();
void LORA_ResetIrqFlag(uint8_t flags);

int16_t LORA_ReadRssi();
int16_t LORA_ReadPacketRssi();

void LORA_ReadBuffer(uint8_t start, uint16_t length, uint8_t* buffer);
void LORA_WriteBuffer(uint8_t start, uint16_t length, uint8_t* buffer);
uint8_t LORA_GetTxBase();
uint8_t LORA_GetRxBase();
uint8_t LORA_GetPayloadLength();
void LORA_SetPayloadLength(uint8_t length);
uint8_t LORA_GetMaxPayloadLength();
void LORA_SetMaxPayloadLength(uint8_t length);

LORA_RegModemConfig1_T LORA_GetModemConfig1();
void LORA_SetModemConfig1(LORA_RegModemConfig1_T val);
uint8_t LORA_GetModemConfig1Uint();
void LORA_SetModemConfig1Uint(uint8_t val);

LORA_RegModemConfig2_T LORA_GetModemConfig2();
void LORA_SetModemConfig2(LORA_RegModemConfig2_T val);
uint8_t LORA_GetModemConfig2Uint();
void LORA_SetModemConfig2Uint(uint8_t val);

LORA_RegModemConfig3_T LORA_GetModemConfig3();
void LORA_SetModemConfig3(LORA_RegModemConfig3_T val);
uint8_t LORA_GetModemConfig3Uint();
void LORA_SetModemConfig3Uint(uint8_t val);

LORA_RegModemStat_T LORA_GetModemStat();
uint8_t LORA_GetModemStatUint();

uint16_t LORA_GetPreamble();
void LORA_SetPreamble(uint16_t preamble);

uint16_t LORA_GetSymbTimeout();
void LORA_SetSymbTimeout(uint16_t timeout);

uint8_t LORA_GetFifoRxByteAddr();
uint8_t LORA_GetFifoRxBytesNb();

void LORA_SetDIOMapping(LORA_DIO0_T dio0, LORA_DIO1_T dio1, LORA_DIO2_T dio2, LORA_DIO3_T dio3, LORA_DIO4_T dio4, LORA_DIO5_T dio5);
void LORA_DIOHandler(RFM9X_DIO_T dio);

void LORA_StartTx(uint32_t freq, uint8_t *data, uint16_t length);
void LORA_StartRx(uint32_t freq, LORA_RxMode_T mode);
void LORA_StartCad(uint32_t freq);
#endif /* INC_LORA_H_ */
