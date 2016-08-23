/*
 * lora.c
 *
 *  Created on: 05 дек. 2015 г.
 *      Author: svalov
 */

#include "../inc/lora.h"

static fhssFreqRequest freqRequest=0;
static irqHandler IrqHandler=0;
static onRxDoneHandler rxDoneHandler=0;
static onRxTimeoutHandler rxTimeoutHandler=0;
static onTxDoneHandler txDoneHandler=0;
static onCadDoneHandler cadDoneHandler=0;
static onCadDetectedHandler cadDetectedHandler=0;

static LORA_DIO0_T dio0Mode;
static LORA_DIO1_T dio1Mode;
static LORA_DIO2_T dio2Mode;
static LORA_DIO3_T dio3Mode;
static LORA_DIO4_T dio4Mode;
static LORA_DIO5_T dio5Mode;

static uint8_t fifoBuffer[RFM9X_FIFO_SIZE];
static LORA_RxMode_T rxMode = LORA_RX_None;

void LORA_SetIrqHandler(irqHandler handler){
	IrqHandler = handler;
}

void LORA_SetRxDoneHandler(onRxDoneHandler handler){
	rxDoneHandler = handler;
}

void LORA_SetRxTimeoutHandler(onRxTimeoutHandler handler){
	rxTimeoutHandler = handler;
}

void LORA_SetTxDoneHandler(onTxDoneHandler handler){
	txDoneHandler = handler;
}

void LORA_SetCadDoneHandler(onCadDoneHandler handler){
	cadDoneHandler = handler;
}
void LORA_SetCadDetectedHandler(onCadDetectedHandler handler){
	cadDetectedHandler = handler;
}

uint8_t LORA_GetHopChannel(){
	uint8_t channel;
	RFM9X_ReadRegister(LORA_RegHopChannel,&channel);
	channel &=0x3F;
	return channel;
}

uint8_t LORA_GetHopPeriod(){
	uint8_t period;
	RFM9X_ReadRegister(LORA_RegHopPeriod,&period);
	return period;
}
void LORA_SetHopPeriod(fhssFreqRequest request, uint8_t period){
	freqRequest = request;
	RFM9X_WriteRegister(LORA_RegHopPeriod, period);
}
LORA_RegOpMode_T LORA_GetOpMode(){
	uint8_t val;
	RFM9X_ReadRegister(RFM9X_RegOpMode,&val);
	LORA_RegOpMode_T* value = (LORA_RegOpMode_T*)((void *)&val);
	return *value;
}
void LORA_SetOpMode(LORA_RegOpMode_T mode){
	uint8_t value = *(uint8_t*)((void *)&mode);
	RFM9X_WriteRegister(RFM9X_RegOpMode,value);
}

void LORA_SwitchToStandby(){
#ifdef DEBUG_LEVEL3
	DEBUGOUT("Set mode Mode_STANDBY\n");
#endif
	RFM9X_SetOpMode(Mode_STANDBY);
	rxMode = LORA_RX_None;
}

LORA_RegOpMode_AccessSharedReg_T LORA_GetRegMap(){
	LORA_RegOpMode_T mode = LORA_GetOpMode();
	return mode.AccessSharedReg;
}
void LORA_SetRegMap(LORA_RegOpMode_AccessSharedReg_T reg){
	LORA_RegOpMode_T mode = LORA_GetOpMode();
	mode.AccessSharedReg = reg;
	LORA_SetOpMode(mode);
}

LORA_RegIrqFlags_T LORA_GetIrqFlagsMask(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegIrqFlagsMask,&val);
	LORA_RegIrqFlags_T* flag = (LORA_RegIrqFlags_T*)((void *)&val);
	return *flag;
}
uint8_t LORA_GetIrqFlagsMaskUint(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegIrqFlagsMask,&val);
	return val;
}
void LORA_SetIrqFlagsMask(LORA_RegIrqFlags_T mask){
	uint8_t val = *(uint8_t*)((void *)&mask);
	RFM9X_WriteRegister(LORA_RegIrqFlagsMask,val);
}
void LORA_SetIrqFlagsMaskUint(uint8_t mask){
	RFM9X_WriteRegister(LORA_RegIrqFlagsMask,mask);
}
void LORA_EnableIrq(){
	RFM9X_WriteRegister(LORA_RegIrqFlagsMask,LORA_ENABLE_IRQ);
}
void LORA_DisableIrq(){
	RFM9X_WriteRegister(LORA_RegIrqFlagsMask,LORA_DISABLE_IRQ);
}

LORA_RegIrqFlags_T LORA_GetIrqFlags(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegIrqFlags,&val);
	LORA_RegIrqFlags_T* flag = (LORA_RegIrqFlags_T*)((void *)&val);
	return *flag;
}
uint8_t LORA_GetIrqFlagsUint(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegIrqFlags,&val);
	return val;
}
void LORA_ResetIrqFlags(){
#ifdef DEBUG_LEVEL3
	DEBUGOUT("Reset IRQs\n");
#endif
	LORA_ResetIrqFlag(LORA_DISABLE_IRQ);
}
void LORA_ResetIrqFlag(uint8_t flags){
	RFM9X_WriteRegister(LORA_RegIrqFlags,flags);
}

int16_t LORA_ReadRssi(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegRssiValue,&val);
	return val+LORA_RSSI_OFFSET;
}
int16_t LORA_ReadPacketRssi(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegPktRssiValue,&val);
	return val+LORA_RSSI_OFFSET;
}
void LORA_ReadBuffer(uint8_t start, uint16_t length, uint8_t* buffer){
	RFM9X_WriteRegister(LORA_RegFifoAddrPtr,start);
	uint16_t i;
	for(i=0;i<length;i++)
		RFM9X_ReadRegister(RFM9X_RegFifo,&buffer[i]);
}
void LORA_WriteBuffer(uint8_t start, uint16_t length, uint8_t* buffer){
	RFM9X_WriteRegister(LORA_RegFifoAddrPtr,start);
	uint16_t i;
	for(i=0;i<length;i++)
		RFM9X_WriteRegister(RFM9X_RegFifo,buffer[i]);

}
void LORA_ClearBuffer(){
	//DEBUGOUT("Clear buffer\n");
	RFM9X_WriteRegister(LORA_RegFifoAddrPtr,0x00);
	uint16_t i;
	for(i=0;i<RFM9X_FIFO_SIZE;i++)
		RFM9X_WriteRegister(RFM9X_RegFifo,0x00);
}
uint8_t LORA_GetTxBase(){
	uint8_t base;
	RFM9X_ReadRegister(LORA_RegFifoTxBaseAddr,&base);
	return base;
}
uint8_t LORA_GetRxBase(){
	uint8_t base;
	RFM9X_ReadRegister(LORA_RegFifoRxBaseAddr,&base);
	return base;
}

uint8_t LORA_GetPayloadLength(){
	uint8_t length;
	RFM9X_ReadRegister(LORA_RegPayloadLength,&length);
	return length;
}
void LORA_SetPayloadLength(uint8_t length){
	RFM9X_WriteRegister(LORA_RegPayloadLength,length);
}

uint8_t LORA_GetMaxPayloadLength(){
	uint8_t length;
	RFM9X_ReadRegister(LORA_RegMaxPayloadLength,&length);
	return length;
}
void LORA_SetMaxPayloadLength(uint8_t length){
	RFM9X_WriteRegister(LORA_RegMaxPayloadLength,length);
}

LORA_RegModemConfig1_T LORA_GetModemConfig1(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegModemConfig1,&val);
	LORA_RegModemConfig1_T* value = (LORA_RegModemConfig1_T*)(&val);
	return *value;
}
void LORA_SetModemConfig1(LORA_RegModemConfig1_T val){
	uint8_t value = *(uint8_t*)((void *)&val);
	RFM9X_WriteRegister(LORA_RegModemConfig1,value);
}

uint8_t LORA_GetModemConfig1Uint(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegModemConfig1,&val);
	return val;
}
void LORA_SetModemConfig1Uint(uint8_t val){
	RFM9X_WriteRegister(LORA_RegModemConfig1,val);
}

LORA_RegModemConfig2_T LORA_GetModemConfig2(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegModemConfig2,&val);
	LORA_RegModemConfig2_T* value = (LORA_RegModemConfig2_T*)(&val);
	return *value;
}
void LORA_SetModemConfig2(LORA_RegModemConfig2_T val){
	uint8_t value = *(uint8_t*)((void *)&val);
	RFM9X_WriteRegister(LORA_RegModemConfig2,value);
}

uint8_t LORA_GetModemConfig2Uint(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegModemConfig2,&val);
	return val;
}
void LORA_SetModemConfig2Uint(uint8_t val){
	RFM9X_WriteRegister(LORA_RegModemConfig2,val);
}

LORA_RegModemConfig3_T LORA_GetModemConfig3(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegModemConfig3,&val);
	LORA_RegModemConfig3_T* value = (LORA_RegModemConfig3_T*)(&val);
	return *value;
}
void LORA_SetModemConfig3(LORA_RegModemConfig3_T val){
	uint8_t value = *(uint8_t*)((void *)&val);
	RFM9X_WriteRegister(LORA_RegModemConfig3,value);
}

uint8_t LORA_GetModemConfig3Uint(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegModemConfig3,&val);
	return val;
}

void LORA_SetModemConfig3Uint(uint8_t val){
	RFM9X_WriteRegister(LORA_RegModemConfig3,val);
}

LORA_RegModemStat_T LORA_GetModemStat(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegModemStat,&val);
	LORA_RegModemStat_T* value = (LORA_RegModemStat_T*)(&val);
	return *value;
}
uint8_t LORA_GetModemStatUint(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegModemStat,&val);
	return val;
}

uint16_t LORA_GetPreamble(){
	uint16_t preamble;
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegPreambleLsb, &val);
	preamble=val;
	RFM9X_ReadRegister(LORA_RegPreambleMsb, &val);
	preamble|=val<<8;
	return preamble;
}
void LORA_SetPreamble(uint16_t preamble){
	RFM9X_WriteRegister(LORA_RegPreambleLsb, preamble&0xFF);
	RFM9X_WriteRegister(LORA_RegPreambleMsb, (preamble>>8)&0xFF);
}
uint16_t LORA_GetSymbTimeout(){
	uint16_t timeout;
	uint8_t conf;
	RFM9X_ReadRegister(LORA_RegModemConfig3,&conf);
	uint8_t time;
	RFM9X_ReadRegister(LORA_RegSymbTimeoutLsb,&time);
	timeout = time | ((conf<<8)&0x03);
	return timeout;
}
void LORA_SetSymbTimeout(uint16_t timeout){
	uint8_t conf;
	RFM9X_ReadRegister(LORA_RegModemConfig3,&conf);
	conf&=~0x03;
	conf|= (timeout>>8) & 0x03;
	RFM9X_WriteRegister(LORA_RegModemConfig3,conf);
	uint8_t lsb = timeout &0xff;
	RFM9X_WriteRegister(LORA_RegSymbTimeoutLsb,lsb);
}

uint8_t LORA_GetFifoRxByteAddr(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegFifoRxByteAddr,&val);
	return val;
}
uint8_t LORA_GetFifoRxBytesNb(){
	uint8_t val;
	RFM9X_ReadRegister(LORA_RegRxNbBytes,&val);
	return val;
}
void LORA_SetDIOMapping(LORA_DIO0_T dio0, LORA_DIO1_T dio1, LORA_DIO2_T dio2, LORA_DIO3_T dio3, LORA_DIO4_T dio4, LORA_DIO5_T dio5){
	uint8_t RegDio1=0;
	uint8_t RegDio2=0;
	RFM9X_ReadRegister(RFM9X_RegDioMapping2, &RegDio2);
	RegDio2&=0x0F;
	//
	RegDio1 |= (dio3 | dio2 << 2 | dio1 <<4 |dio0 <<6);
	RegDio2 |= (dio5 <<4 |dio4 <<6);
	//
	dio0Mode = dio0;
	dio1Mode = dio1;
	dio2Mode = dio2;
	dio3Mode = dio3;
	dio4Mode = dio4;
	dio5Mode = dio5;
	//
	RFM9X_WriteRegister(RFM9X_RegDioMapping1, RegDio1);
	RFM9X_WriteRegister(RFM9X_RegDioMapping2, RegDio2);
}

inline void LORA_FhssChangeChannelHandler(){
	LORA_ResetIrqFlag(LORA_IRQ_FHSS_CHANGE_CHANNEL);
	uint8_t channel = LORA_GetHopChannel();
	uint32_t newFreq;
	if(freqRequest)
		newFreq = freqRequest(channel);
	else
		newFreq = RFM9X_GetFrequency();
	RFM9X_SetFrequency(newFreq);
	LORA_ResetIrqFlag(LORA_IRQ_FHSS_CHANGE_CHANNEL);
}
inline void LORA_TxDoneHandler()
{
#ifdef DEBUG_LEVEL3
	DEBUGOUT("IRQ - TxDone\n");
#endif
	if(txDoneHandler) txDoneHandler();
}
inline void LORA_RxDoneHandler()
{
#ifdef DEBUG_LEVEL3
	DEBUGOUT("IRQ - RxDone\n");
#endif
	LORA_RegIrqFlags_T irq = LORA_GetIrqFlags();
	LORA_ResetIrqFlags();
	if(!irq.PayloadCrcError && irq.ValidHeader){
#ifdef DEBUG_LEVEL3
		DEBUGOUT("Data seem valid\n");
#endif
		uint8_t dataCount = LORA_GetFifoRxBytesNb();
		uint8_t offset = LORA_GetFifoRxByteAddr();
		uint8_t start = offset-dataCount;
#ifdef DEBUG_LEVEL3
		DEBUGOUT("Reading %d bytes\n", dataCount);
#endif
		LORA_ReadBuffer(start,dataCount,fifoBuffer);
		int16_t prssi = LORA_ReadPacketRssi();
//		int8_t snr;
//		RFM9X_ReadRegister(LORA_RegPktSnrValue,&snr);
//		uint8_t rssi;
//		RFM9X_ReadRegister(LORA_RegPktRssiValue,&rssi);
//		int32_t rssi_calculated = rssi;
//		rssi_calculated = -157 + rssi_calculated + snr/4;
//#ifdef DEBUG_LEVEL1
//		DEBUGOUT("rssi %d snr %d, corrected %d\n", prssi, snr, rssi_calculated);
//#endif
		if(rxDoneHandler) rxDoneHandler(fifoBuffer,dataCount,prssi, rxMode);
	}
}
inline void LORA_RxTimeoutHandler()
{
#ifdef DEBUG_LEVEL3
	DEBUGOUT("IRQ - RxTimeout\n");
#endif
	if(rxTimeoutHandler) rxTimeoutHandler();
}

inline void LORA_CadDoneHandler()
{
#ifdef DEBUG_LEVEL3
	DEBUGOUT("IRQ - CadDone\n");
#endif
	if(cadDoneHandler) cadDoneHandler();
}

inline void LORA_CadDetectedHandler()
{
#ifdef DEBUG_LEVEL3
	DEBUGOUT("IRQ - CadDetected");
#endif
	if(cadDetectedHandler) cadDetectedHandler();
}

inline void LORA_DIO0Handler(void){
	switch(dio0Mode){
	case LORA_DIO0_TxDone:
		LORA_TxDoneHandler();
		break;
	case LORA_DIO0_RxDone:
		LORA_RxDoneHandler();
		break;
	case LORA_DIO0_CadDone:
		LORA_CadDoneHandler();
		break;
	}
}
inline void LORA_DIO1Handler(void){
	switch(dio1Mode){
	case LORA_DIO1_CadDetected:
		LORA_CadDetectedHandler();
		break;
	case LORA_DIO1_RxTimeout:
		LORA_RxTimeoutHandler();
		break;
	}
}
inline void LORA_DIO2Handler(void){
	LORA_FhssChangeChannelHandler();
}
inline void LORA_DIO3Handler(void){
	switch(dio3Mode){
	case LORA_DIO3_CadDone:
		LORA_CadDoneHandler();
		break;
	case LORA_DIO3_PayloadCrcError:
		break;
	case LORA_DIO3_ValidHeader:
		break;
	}
}
inline void LORA_DIO4Handler(void){
	switch(dio4Mode){
	case LORA_DIO4_CadDetected:
		LORA_CadDetectedHandler();
		break;
	case LORA_DIO4_PllLock:
		break;
	}
}
inline void LORA_DIO5Handler(void){
	switch(dio5Mode){
	case LORA_DIO5_ClkOut:
		break;
	case LORA_DIO5_ModeReady:
		break;
	}
}

void LORA_DIOHandler(RFM9X_DIO_T dio){
	switch(dio){
	case RFM9X_DIO0:
		LORA_DIO0Handler();
		break;
	case RFM9X_DIO1:
		LORA_DIO1Handler();
		break;
	case RFM9X_DIO2:
		LORA_DIO2Handler();
		break;
	case RFM9X_DIO3:
		LORA_DIO3Handler();
		break;
	case RFM9X_DIO4:
		LORA_DIO4Handler();
		break;
	case RFM9X_DIO5:
		LORA_DIO5Handler();
		break;
	}
}

void LORA_StartTx(uint32_t freq, uint8_t *data, uint16_t length){
	RFM9X_SetFrequency(freq);
#ifdef DEBUG_LEVEL3
	DEBUGOUT("Sending 0x%02x bytes\n",length);
#endif
	LORA_ClearBuffer();
#ifdef DEBUG_LEVEL3
	DEBUGOUT("Set payload length 0x%02x\n",length);
#endif
	LORA_SetPayloadLength(length);
	uint8_t tx_base = LORA_GetTxBase();
#ifdef DEBUG_LEVEL3
	DEBUGOUT("Writing 0x%02x bytes to buffer at 0x%02x\n",length,tx_base);
#endif
	LORA_WriteBuffer(tx_base,length,data);

	LORA_ResetIrqFlags();
	LORA_EnableIrq();

	LORA_SetDIOMapping(LORA_DIO0_TxDone,
				LORA_DIO1_RxTimeout,
				LORA_DIO2_FhssChangeChannel,
				LORA_DIO3_CadDone,
				LORA_DIO4_CadDetected,
				LORA_DIO5_ModeReady);

	rxMode = LORA_RX_None;
#ifdef DEBUG_LEVEL3
	DEBUGOUT("Set mode Mode_TX\n");
#endif
	RFM9X_SetOpMode(Mode_TX);
	LORA_GetIrqFlags(); //workaround
}
void LORA_StartRx(uint32_t freq, LORA_RxMode_T mode){
	RFM9X_SetFrequency(freq);

	LORA_ResetIrqFlags();
	LORA_EnableIrq();
	LORA_ClearBuffer();

	LORA_SetDIOMapping(LORA_DIO0_RxDone,
					LORA_DIO1_RxTimeout,
					LORA_DIO2_FhssChangeChannel,
					LORA_DIO3_CadDone,
					LORA_DIO4_CadDetected,
					LORA_DIO5_ModeReady);

	rxMode = mode;
	RFM9X_RegOpMode_Mode_T opMode = Mode_RXCONTINUOUS;
	if(mode == LORA_RX_Single){
#ifdef DEBUG_LEVEL3
		DEBUGOUT("Set mode Mode_RXSINGLE\n");
#endif
		opMode = Mode_RXSINGLE;
	}
#ifdef DEBUG_LEVEL3
	else
		DEBUGOUT("Set mode Mode_RXCONTINUOUS\n");
#endif
	RFM9X_SetOpMode(opMode);
	LORA_GetIrqFlags();
}
void LORA_StartCad(uint32_t freq){
	RFM9X_SetFrequency(freq);

	LORA_ResetIrqFlags();
	LORA_EnableIrq();

	LORA_SetDIOMapping(LORA_DIO0_CadDone,
					LORA_DIO1_CadDetected,
					LORA_DIO2_FhssChangeChannel,
					LORA_DIO3_CadDone,
					LORA_DIO4_CadDetected,
					LORA_DIO5_ModeReady);

	RFM9X_RegOpMode_Mode_T opMode = Mode_CAD;
#ifdef DEBUG_LEVEL3
	DEBUGOUT("Set mode Mode_CAD\n");
#endif
	RFM9X_SetOpMode(opMode);
	LORA_GetIrqFlags();
}
