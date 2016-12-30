/*
 * settings.c
 *
 *  Created on: 05 мая 2016 г.
 *      Author: svalov
 */

#include <rfm9x.h>
#include "settings.h"
#include "loraSettings.h"
#include "ifaceSettings.h"
#include <hwConfig.h>

void Init_RFM9XSettings(RFM9X_Settings_T* settings)
{
	settings->spiSpeed = SPI_SPEED;
	settings->spiChannel = SPI_CHANNEL;
	settings->resetPin = RESET_PIN;
	settings->resetPort = RESET_PORT;
	settings->useReset = true;
}

void Init_LORASettings(LORA_Settings_T* settings){
	Init_RFM9XSettings(&(settings->RFM9X_Settings));
	settings->MinFrequency = MIN_FREQUENCY;
	settings->MaxFrequency = MAX_FREQUENCY;
	settings->MinChannelBandWidth = MIN_CHANNEL_BANDWIDTH;
	settings->ChannelsCount = CHANNELS_COUNT;
	settings->MaxPower = LORA_MAXPOWER;
	settings->OutputPower = LORA_OUTPUTPOWER;
	settings->UseBoost = LORA_PABOOST;
	//settings->MaxOutputPower = LORA_MAXOUTPUTPOWER;
	//settings->MinOutputPower = LORA_MINOUTPUTPOWER;
	settings->Bandwidth = LORA_BANDWIDTH;
	settings->CoddingRate = LORA_CODDINGRATE;
	settings->UseImplicitHeader = LORA_IMPLICITHEADER;
	settings->Preamble = LORA_PREAMBLE;
	//settings->LoraMessageOverhead = LORA_MESSAGE_OVERHEAD;
	settings->Timeout = LORA_TIMEOUT;
	settings->HopPeriod = LORA_HOPPERIOD;
	settings->SpreadingFactor = LORA_SPREADINGFACTOR;
	settings->RxPayloadCRC = LORA_RXPAYLOADCRC;
}

void Init_IfaceSettings(IfaceSettings_T* settings){
	Init_LORASettings(&(settings->LORA_Settings));

	settings->DataTxPower = LORA_OUTPUTPOWER;
	settings->DataTxBoost = LORA_PABOOST;
	settings->BeaconTxBoost = LORA_PABOOST;
	settings->BeaconTxPower = BEACON_POWER;
	settings->NeighborsCount = STORED_NEIGHBORS;
	settings->DataNeedResponse = DATA_NEED_RESPONSE;
	settings->WaitingResponseTimeout = WAITING_RESPONSE_TIMEOUT;
	settings->TransmitTimeout = TRANSMIT_TIMEOUT;
	settings->BeaconRssiMinThreshold = BEACON_RSSI_MIN_THRESHOLD;
	settings->BeaconRssiMaxThreshold = BEACON_RSSI_MAX_THRESHOLD;
	settings->BeaconChannel = BEACON_CHANNEL;
	settings->BeaconSeed = BEACON_SEED;
	settings->MonitorChannel = BEACON_CHANNEL;
	settings->MonitorSeed = BEACON_SEED;
	settings->BeaconDefaultPayloadSize = BEACON_DEFAULT_PAYLOAD_SIZE;
	settings->BeaconSendInterval = BEACON_SEND_INTERVAL;
	settings->BeaconSendDeviation = BEACON_SEND_DEVIATION;
	settings->BeaconListenTimeout = BEACON_LISTEN_TIMEOUT;
	settings->BeaconStartupInterval = BEACON_SEND_STARTUP_INTERVAL;
	settings->BeaconStartupDuration = BEACON_SEND_STARTUP_DURATION;
	settings->BeaconListenStartup = BEACON_LISTEN_STARTUP;
	settings->BeaconListenForce = BEACON_LISTEN_FORCE;
//	settings->InterfaceStackSize = INTERFACE_STACK_SIZE;
	settings->MaxNeighborSendTrys = MAX_LAST_SEND_TRYS;
	settings->TxTimeoutCoef = TX_TIMEOUT_COEF;
	settings->ResetInterfaceTimeout = LORA_RESTART_TIMEOUT;
}
