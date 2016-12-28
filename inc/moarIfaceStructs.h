//
// Created by svalov on 8/25/16.
//

#ifndef MOARSTACK_MOARIFACESTRUCTS_H
#define MOARSTACK_MOARIFACESTRUCTS_H

#include <stdint.h>
#include <packet.h>

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
//typedef uint32_t	IfaceAddr_T;	// type for interface addresses
typedef int8_t		PowerTx_T;		// type for transmitting power value
typedef uint8_t		PackType_T;		// type for number with meaning of pack type
typedef uint16_t	CRCvalue_T;		// type for CRC value

// typedefs for iface footer fields (used in beacon)
typedef uint8_t		FreqNum_T;		// type for frequency number
typedef uint32_t	FreqVal_T;		// type for frequency value (used in calculatings, etc.)
typedef uint16_t	FreqSeed_T;		// type for frequency list seed

typedef enum{
	ListenChannel_None,
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

#pragma pack(pop)
#endif //MOARSTACK_MOARIFACESTRUCTS_H
