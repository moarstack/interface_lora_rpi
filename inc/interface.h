/*
 * interface.h
 *
 *  Created on: 16 янв. 2016 г.
 *      Author: kryvashek
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "packet.h"
#include "lora.h"
#include "FreeRTOS.h"

#define INFINITY_TIME					UINT64_MAX
// maximum size of usual iface packet payload
#define SzIfaceMaxPacket		256

// size of iface packet header
#define SzIfaceHeader			sizeof(IfaceHeader_T)

// size of iface packet footer
#define SzIfaceFooter			sizeof(IfaceFooter_T)

// maximum size of usual iface packet payload
#define SzIfaceMaxPayloadUsual	(SzIfaceMaxPacket-SzIfaceHeader)

// maximum size of iface beacon packet payload
#define SzIfaceMaxPayloadBeacon	(SzIfaceMaxPayloadUsual-SzIfaceFooter)

// place where iface packet header starts
#define IfaceHeaderStart		(0)

// place where iface packet payload starts
#define IfacePayloadStart		(SzIfaceHeader)

// typedefs for iface header fields
typedef uint32_t	IfaceAddr_T;	// type for interface addresses
typedef int8_t		PowerTx_T;		// type for transmitting power value
typedef uint8_t		PackType_T;		// type for number with meaning of pack type
typedef uint16_t	CRCvalue_T;		// type for CRC value

// typedefs for iface footer fields (used in beacon)
typedef uint8_t		FreqNum_T;		// type for frequency number
typedef uint32_t	FreqVal_T;		// type for frequency value (used in calculatings, etc.)
typedef uint16_t	FreqSeed_T;		// type for frequency list seed

typedef enum{
	ListenChannel_Data,
	ListenChannel_Beacon,
	ListenChannel_Monitor,
} IfaceListenChannel_T;
#pragma pack(push, 1)

// struct to hold together packet and his status (using in all upgoing queues)
typedef struct
	{
	Packet_T		Pack;	// current packet itself (place in memory)
	SizePayload_T	Size;	// payload size of current packet
	PackState_T		State;	// status or result of sending current packet
	uint8_t 		Attempt;
	} IfaceMsgUp_T;

// type for usual iface header
typedef struct{
	IfaceAddr_T		To,			// receiver of packet
					From;		// sender of packet
	CRCvalue_T		CRC;		// CRC summ of current packet (assuming part of From field instead of CRC bytes while calculating them)
	PowerTx_T		TxPower;		// power level of packet transmitting
	PackType_T		Beacon:1;	// type of the packet: normal (0) / beacon (1)
	//PackType_T		Routing:1;	// type of the packet: not routing hello (0) / routing hello (1)
	PackType_T		NeedResponse:1;	// packet attribute - need response
	PackType_T		Response:1;	// packet type: packet is response
	SizePayload_T	Size;		// size of payload in current packet
} IfaceHeader_T;

// type for iface beacon footer
typedef struct{
	FreqSeed_T	FreqSeed;	// seed to generate frequencies list
	FreqNum_T	FreqStart;	// frequency to start with while transmitting
	PowerTx_T	MinSensitivity; // minimal sensitivity of interface
} IfaceFooter_T;

//response payload struct
typedef struct{
	CRCvalue_T 	FullMessageCrc,
				NormalMessageCrc;
} IfaceResponsePayload_T;

typedef struct{
	LORA_Settings_T LORA_Settings;
	//INTERFACE_STACK_SIZE configMINIMAL_STACK_SIZE*2
	//uint16_t InterfaceStackSize;
	//STORED_NEIGHBORS				10
	uint8_t NeighborsCount;

	uint32_t BeaconAddress;
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
	int16_t BeaconSeed;
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

typedef struct{

} IfaceState_T;

//max send trys

//cross layer data struct
//pointer
//status

//neighbor struct //need critical section on operation
typedef struct{
	IfaceAddr_T Address;
	portTickType LastSeen;
	FreqSeed_T	Seed;
	int16_t		SignalLoss;
	FreqNum_T	Frequency;
	PowerTx_T	MinSensitivity;
	uint8_t		LastFailedTrys;
} NeighborInfo_T;

#pragma pack(pop)
//states
typedef enum{
	TransmissionState_Monitor,
	TransmissionState_Listening,
	TransmissionState_Receiving,
	TransmissionState_Transmiting,
	TransmissionState_WaitingResponse,
}TransmissionState_T;

extern uint8_t neighborsCount;
extern NeighborInfo_T* neighbors;
extern bool monitorMode;
extern uint8_t monitorChannel;
extern uint16_t monitorSeed;
void Interface_PreStart();
void Interface_MainTaskLoop(void *pvParameters);
void Init_Interface(IfaceAddr_T address, IfaceSettings_T* settings);


#endif /* INTERFACE_H_ */
