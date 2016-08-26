/*
 * hwInterface.c

 *
 *  Created on: 03 февр. 2016 г.
 *      Author: svalov
 */
#include "port.h"
#include "hwInterface.h"
#include "stdfunc.h"
#include "string.h"

#define inline

#define LORA_HEADER_OVERHEAD 4
#define LORA_CRC_OVERHEAD 2
#define LORA_PREAMBLE_CONST_OVERHEAD 5
#define LORA_MESSAGE_OVERHEAD (LORA_HEADER_OVERHEAD + LORA_CRC_OVERHEAD + LORA_PREAMBLE_CONST_OVERHEAD)
#define DEBUGOUT printf
#define DEBUG_LEVEL0
#define DEBUG_LEVEL1
#define DEBUG_LEVEL2
#define DEBUG_LEVEL3

//can be replaced by on fly generation
uint8_t* channelsTable = NULL;
//physical
int16_t minSensetivity = 0;
//queue processing - simple
RxData_T recievedData;

//interrupt flags
InterfaceEvents_T events;
//interface state
InterfaceState_T interfaceState = InterfaceState_Off;
uint16_t constantMessageOverhead = 0;

uint8_t channelsCount;
uint32_t channelBandwidth;

LORA_Settings_T* loraSettings;
//init
inline void freqDivisionInit(LORA_Settings_T* settings){
	uint32_t realBandwidth = settings->MaxFrequency-settings->MinFrequency;
	channelBandwidth = realBandwidth/settings->ChannelsCount;
	channelsCount = settings->ChannelsCount;
	if(channelBandwidth<settings->MinChannelBandWidth){
		channelBandwidth = settings->MinChannelBandWidth;
		channelsCount = realBandwidth/settings->MinChannelBandWidth;
	}
	if(channelsTable)
		vPortFree(channelsTable);
	channelsTable = pvPortMalloc(channelsCount);
}
//get frequency by channel
uint32_t getFrequency(uint8_t channel){
	return (loraSettings->MinFrequency+channelBandwidth*channel);
}

void buildChannelsTable(uint8_t channel, uint16_t seed){
	unsigned short intSeed = seed;
	channelsTable[0] = channel;
	int i=1;
	for(;i<channelsCount;i++)
		channelsTable[i] = (brand(&intSeed))%channelsCount;
}

//interrupt handlers
//set flags and store data
void RxDoneHandler(uint8_t *data, uint16_t size, int16_t rssi, LORA_RxMode_T mode){
	//to avoid memory leak
	if(recievedData.Processed){
		//set sens
		if(rssi<minSensetivity)
			minSensetivity = rssi;
		//get memory and create data
		recievedData.Rssi = rssi;
		recievedData.Size = size;
		recievedData.Pointer = data;
		//set to not processed
		recievedData.Processed = false;
		//switch to standby
		LORA_ResetIrqFlags();
		LORA_SwitchToStandby();
		//set interface state
		events.RxDone = 1;
		interfaceState = InterfaceState_Busy;
	}
}
void TxDoneHandler(){
	LORA_ResetIrqFlags();
	LORA_SwitchToStandby();
	events.TxDone = 1;
	interfaceState = InterfaceState_On;
}
void RxTimeoutHandler(){
	LORA_ResetIrqFlags();
	LORA_SwitchToStandby();
	events.RxTimeout = 1;
	interfaceState = InterfaceState_On;
}
void CadDoneHandler(){

}
void CadDetectedHandler(){

}
void resetInterfaceState(){
	LORA_ResetIrqFlags();
	LORA_SwitchToStandby();
	events.RxDone = 0;
	events.TxDone = 0;
	events.RxTimeout = 0;
	interfaceState = InterfaceState_On;
}
//start rx
uint8_t startRx(uint8_t channel, uint16_t seed){
	if(interfaceState!=InterfaceState_On && interfaceState!=InterfaceState_Receive)
		return 1;
	//currentFreq = frequency;
	//currentSeed = seed;
	buildChannelsTable(channel,seed);
	LORA_StartRx(getFrequency(channel),LORA_RX_Continiuos);
	events.RxDone = 0;
	interfaceState = InterfaceState_Receive;
	return 0;
}
//send message
uint8_t startTx(uint8_t channel, uint16_t seed, uint8_t* data, uint16_t size){
	if(interfaceState!=InterfaceState_On)
		return 1;
	//currentFreq = frequency;
	//currentSeed = seed;
	buildChannelsTable(channel,seed);
	LORA_StartTx(getFrequency(channel),data,size);
	events.TxDone = 0;
	interfaceState = InterfaceState_Transmit;
	return 0;
}
//message avail
bool dataAvailable(){
	return !recievedData.Processed;
}
bool readyToSend(){
	return interfaceState==InterfaceState_On
			|| interfaceState == InterfaceState_Receive;
}

//power calc
inline RFM9X_RegPaConfig_T powerCalc(int8_t power, bool boost){
	RFM9X_RegPaConfig_T val;
#ifdef LORA_PABOOST_ENABLE
	if(boost){
		//limit values
		if(power > LORA_MAXBOOSTOUTPUTPOWER)
			power = LORA_MAXBOOSTOUTPUTPOWER;
		if(power < LORA_MINBOOSTOUTPUTPOWER)
			power = LORA_MINBOOSTOUTPUTPOWER;

		val.PaSelect = 1;
		val.MaxPower = 0x07;
		val.OutputPower = power-2; // 0..0xf
	}
	else
#endif
	{
		//limit values
		if(power > LORA_MAXNOBOOSTOUTPUTPOWER)
			power = LORA_MAXNOBOOSTOUTPUTPOWER;
		if(power < LORA_MINNOBOOSTOUTPUTPOWER)
			power = LORA_MINNOBOOSTOUTPUTPOWER;

		uint8_t maxPower = 0x07;
		uint8_t maxPoweVal = 15;
		if(power<0){
			maxPower = 0x00;
			maxPoweVal = 11;  //10.8
		}
		val.PaSelect = 0;
		val.MaxPower = maxPower;
		val.OutputPower = power+(15-maxPoweVal); // 0..0xf
	}
	return val;
}
//set power and return real setted power
int8_t setPower(int8_t value, bool boost){
	RFM9X_RegPaConfig_T val = powerCalc(value, boost);
	RFM9X_SetPaConfig(val);
	if(val.PaSelect){
		return val.OutputPower + 2;
	}
	else{
		int8_t maxPower = 11;
		if(val.MaxPower==0x07)
			maxPower = 15;
		return val.OutputPower - (15-(maxPower));
	}
}

//fhss freq calculator
uint32_t fhssCalc(uint8_t channel){
	//uint32_t newFreq = currentFreq+channel*currentSeed;
	uint32_t newFreq = getFrequency(channelsTable[channel%channelsCount]);
	return newFreq;
}
inline bool Init_LORA_handlers(LORA_Settings_T* settings) {
	LORA_SetRxDoneHandler(RxDoneHandler);
	LORA_SetRxTimeoutHandler(RxTimeoutHandler);
	LORA_SetTxDoneHandler(TxDoneHandler);
	LORA_SetCadDoneHandler(CadDoneHandler);
	LORA_SetCadDetectedHandler(CadDetectedHandler);
	return true;
}
inline bool Init_LORA_Timeout(LORA_Settings_T* settings) {
	uint16_t timeout = settings->Timeout;
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Setup timeout 0x%02x - ", timeout);
#endif
	LORA_SetSymbTimeout(timeout);
	uint16_t timeout_read = LORA_GetSymbTimeout();
	if (timeout_read == timeout) {
#ifdef DEBUG_LEVEL0
		DEBUGOUT("OK\n");
#endif
	} else {
#ifdef DEBUG_LEVEL0
		DEBUGOUT("FAILED\n");
#endif
		return false;
	}
	return true;
}
inline bool Init_LORA_HopPeriod(LORA_Settings_T* settings) {
	uint8_t hopPeriod = settings->HopPeriod;
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Setup hop period 0x%02x - ", hopPeriod);
#endif
	//RFM9X_WriteRegister(LORA_RegHopPeriod,hopPeriod);
	LORA_SetHopPeriod(fhssCalc, hopPeriod);
	uint8_t hopPeriod_read = LORA_GetHopPeriod();
	if (hopPeriod_read == hopPeriod) {
#ifdef DEBUG_LEVEL0
		DEBUGOUT("OK\n");
#endif
	} else {
#ifdef DEBUG_LEVEL0
		DEBUGOUT("FAILED\n");
#endif
		return false;
	}
	return true;
}
inline bool Init_LORA_Preamble(LORA_Settings_T* settings){
	uint16_t preamble = settings->Preamble;
	constantMessageOverhead += (preamble + LORA_PREAMBLE_CONST_OVERHEAD);
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Setup preamble 0x%02x - ",preamble);
#endif
	LORA_SetPreamble(preamble);
	uint16_t preamble_read = LORA_GetPreamble();
	if(preamble_read == preamble){
#ifdef DEBUG_LEVEL0
		DEBUGOUT("OK\n");
#endif
	}
	else {
#ifdef DEBUG_LEVEL0
		DEBUGOUT("FAILED\n");
#endif
		return false;
	}
	return true;
}
inline bool Init_LORA_ModemConfig2(LORA_Settings_T* settings){
	LORA_RegModemConfig2_T config2 = LORA_GetModemConfig2();
	config2.SpreadingFactor = settings->SpreadingFactor;
	config2.RxPayloadCrcOn = settings->RxPayloadCRC;
	//if(config2.RxPayloadCrcOn) // not fully correct, but not found in docs see page 30 of semtech datasheet
	constantMessageOverhead += LORA_CRC_OVERHEAD;
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Setup modem config2 0x%02x - ", config2);
#endif
	LORA_SetModemConfig2(config2);
	LORA_RegModemConfig2_T config2_read = LORA_GetModemConfig2();
	if(!memcmp(&config2_read,&config2,sizeof(LORA_RegModemConfig2_T)))	{
#ifdef DEBUG_LEVEL0
		DEBUGOUT("OK\n");
#endif
	}
	else {
#ifdef DEBUG_LEVEL0
		DEBUGOUT("FAILED\n");
#endif
		return false;
	}
	return true;
}
inline bool Init_LORA_ModemConfig1(LORA_Settings_T* settings){
	LORA_RegModemConfig1_T config = LORA_GetModemConfig1();
	config.Bw = settings->Bandwidth;
	config.CodingRate = settings->CoddingRate;
	config.ImplicitHeaderModeOn = settings->UseImplicitHeader;
	if(!config.ImplicitHeaderModeOn)
		constantMessageOverhead+= LORA_HEADER_OVERHEAD;
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Setup modem config 0x%02x - ", config);
#endif
	LORA_SetModemConfig1(config);

	LORA_RegModemConfig1_T config_read = LORA_GetModemConfig1();
	if(!memcmp(&config_read,&config,sizeof(LORA_RegModemConfig1_T)))	{
#ifdef DEBUG_LEVEL0
		DEBUGOUT("OK\n");
#endif
	}
	else {
#ifdef DEBUG_LEVEL0
		DEBUGOUT("FAILED\n");
#endif
		return false;
	}
	return true;
}
inline bool Init_LORA_PaConfig(LORA_Settings_T* settings){
	RFM9X_RegPaConfig_T conf = RFM9X_GetPaConfig();
	conf.PaSelect = settings->UseBoost; //no boost 0x01 - boost
	conf.MaxPower = settings->MaxPower; //10.8dbm
	conf.OutputPower = settings->OutputPower; //pout equal max power // -4.2dbm 0x08 - 10dbm
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Setup power amplifier config 0x%02x - ",conf);
#endif
	RFM9X_SetPaConfig(conf);

	RFM9X_RegPaConfig_T conf_read = RFM9X_GetPaConfig();
	if(!memcmp(&conf_read,&conf,sizeof(RFM9X_RegPaConfig_T)))	{
#ifdef DEBUG_LEVEL0
		DEBUGOUT("OK\n");
#endif
	}
	else{
#ifdef DEBUG_LEVEL0
		DEBUGOUT("FAILED\n");
#endif
		return false;
	}
	return true;
}
inline bool Init_LORA_Mode(LORA_Settings_T* settings){
	bool res = true;
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Switch to long range - ");
#endif
	RFM9X_SwitchLongRangeMode(LongRangeMode_LORA);

	RFM9X_RegOpMode_Mode_T after = RFM9X_GetOpMode();
	RFM9X_RegOpMode_LongRangeMode_T mode = RFM9X_GetLongRangeMode();

	if(mode == LongRangeMode_LORA)
	{
#ifdef DEBUG_LEVEL0
		DEBUGOUT("OK\n");
#endif
	}
	else{
#ifdef DEBUG_LEVEL0
		DEBUGOUT("FAILED\n");
#endif
		res = false;
	}
	if(after == Mode_STANDBY){
#ifdef DEBUG_LEVEL0
		DEBUGOUT("Device ready\n");
#endif
	}
	else {
#ifdef DEBUG_LEVEL0
		DEBUGOUT("Device wakeup FAILED\n");
#endif
		res = false;
	}
	return res;
}
inline bool Init_Frequency_Division(LORA_Settings_T* settings){
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Calculate Frequency bands\n");
#endif
	freqDivisionInit(settings);
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Low Frequency 0x%08x\n",settings->MinFrequency);
	DEBUGOUT("High Frequency 0x%08x\n",settings->MaxFrequency);
	DEBUGOUT("Single channel band 0x%08x\n",channelBandwidth);
	DEBUGOUT("Channels count 0x%02x\n",channelsCount);
#endif
	return true;
}
//lora init
bool Init_LORA(LORA_Settings_T* settings){
	bool res = true;
	constantMessageOverhead = 0;
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Init SPI\n");
#endif
	loraSettings = settings;
	RFM9X_Init(&(settings->RFM9X_Settings));
	DEBUGOUT("LORA RESET\n");
	RFM9X_Reset();
	res &= Init_LORA_Mode(settings);
	res &= Init_LORA_PaConfig(settings);
	res &= Init_LORA_ModemConfig1(settings);
	res	&= Init_Frequency_Division(settings);
	res &= Init_LORA_ModemConfig2(settings);
	res &= Init_LORA_Preamble(settings);
	res &= Init_LORA_HopPeriod(settings);
	res &= Init_LORA_Timeout(settings);
	res &= Init_LORA_handlers(settings);
	interfaceState = InterfaceState_On;
	recievedData.Processed = 1;
	return res;
}
