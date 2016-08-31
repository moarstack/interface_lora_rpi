//
// Created by svalov on 8/25/16.
//

#ifndef MOARSTACK_MOARIFACESETTINGS_H
#define MOARSTACK_MOARIFACESETTINGS_H

#include <stdint.h>
#include <lora.h>

typedef struct{
	LORA_Settings_T LORA_Settings;
	//INTERFACE_STACK_SIZE configMINIMAL_STACK_SIZE*2
	//uint16_t InterfaceStackSize;
	//STORED_NEIGHBORS				10
	uint8_t NeighborsCount;

	IfaceAddr_T BeaconAddress;
//BEACON_DEFAULT_PAYLOAD_SIZE		0
	uint8_t BeaconDefaultPayloadSize;
	//data tx power
	int8_t DataTxPower;
	//data boost
	bool DataTxBoost;
	//beacon boost
	bool BeaconTxBoost;
	//beacon power
	//BEACON_POWER					LORA_MAXOUTPUTPOWER
	int8_t BeaconTxPower;
//DATA_NEED_RESPONSE 				1
	bool DataNeedResponse;
//WAITING_RESPONSE_TIMEOUT 		1000
	uint16_t WaitingResponseTimeout;
//TRANSMIT_TIMEOUT				WAITING_RESPONSE_TIMEOUT*10;
	uint16_t TransmitTimeout;
//BEACON_RSSI_THRESHOLD			-90
	int8_t BeaconRssiMinThreshold;
	int8_t BeaconRssiMaxThreshold;
//BEACON_CHANNEL 					0
	uint8_t BeaconChannel;
//BEACON_SEED						0xa3fc
	uint16_t BeaconSeed;
	uint8_t MonitorChannel;
	uint16_t MonitorSeed;
//BEACON_DEFAULT_ADDRESS			0x00000000
//BEACON_SEND_INTERVAL			5000
	uint16_t BeaconSendInterval;
//BEACON_SEND_DEVIATION			500
	uint16_t BeaconSendDeviation;
//BEACON_LISTEN_TIMEOUT			BEACON_SEND_INTERVAL
	uint16_t BeaconListenTimeout;
//BEACON_SEND_STARTUP_INTERVAL	1000
	uint16_t BeaconStartupInterval;
//BEACON_SEND_STARTUP_DURATION	(BEACON_SEND_STARTUP_INTERVAL*10)
	uint16_t BeaconStartupDuration;
//BEACON_LISTEN_STARTUP			(BEACON_SEND_STARTUP_DURATION)
	uint16_t BeaconListenStartup;
//BEACON_LISTEN_FORCE				(BEACON_SEND_INTERVAL*10)
	uint16_t BeaconListenForce;
	//remove neighbor after MAX_LAST_SEND_TRYS send without response
	//#define MAX_LAST_SEND_TRYS				3
	uint8_t MaxNeighborSendTrys;
	//#define TX_TIMEOUT_COEF					2
	uint8_t TxTimeoutCoef;
	uint32_t ResetInterfaceTimeout;
} IfaceSettings_T;

#endif //MOARSTACK_MOARIFACESETTINGS_H
