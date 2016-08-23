/*
 * routing.h
 *
 *  Created on: 16 янв. 2016 г.
 *      Author: kryvashek
 */

#ifndef ROUTING_H_
#define ROUTING_H_

#include "packet.h"
#include "interface.h"
#include "routetable.h"



// size of routing packet header
#define SzRouteHeader			sizeof(RouteHeader_T)

#define SzRouteACKPayloadSize sizeof(RouteACKPayload_T)

// maximum size of usual routing packet
#define SzRouteMaxPacketUsual	(SzIfaceMaxPayloadUsual)

// maximum size of beacon routing packet
#define SzRouteMaxPacketBeacon	(SzIfaceMaxPayloadBeacon)

// maximum size of usual routing packet payload
#define SzRouteMaxPayloadUsual	(SzRouteMaxPacketUsual-SzRouteHeader)

// maximum size of beacon routing packet payload
#define SzRouteMaxPayloadBeacon	(SzRouteMaxPacketBeacon-SzRouteHeader)

// place where routing packet header starts
#define RouteHeaderStart		(IfacePayloadStart)

// place where routing packet payload starts
#define RoutePayloadStart		(RouteHeaderStart+SzRouteHeader)

// just very long ttl. Change if it is not long enough
#define ForeverTTL 65535

#define SzAdHocRecord sizeof(AdHocRecord_T)

#define SzRouteDataRecord sizeof(RouteDataRecord_T)

// typedefs for routing header fields
//typedef uint32_t	RouteAddr_T;	//type for routing addresses
typedef uint16_t	LifeTime_T;		//type for packet life time
typedef uint16_t	PacketId_T;		//type for packet id
typedef uint8_t		PfCount_T;		//type for (maximum) length of probe and/or finder

typedef enum
{
	RoutePackType_Data,	// current packet is data packet
	RoutePackType_ACK,	// current packet is ack packet
	RoutePackType_FinderACK, // current packet is ack for route finder
	RoutePackType_Finder,	// current packet is data packet
	RoutePackType_Probe, // current packet is probe packet, which must establish inter-cluster connections
} RoutePackType_T;

typedef enum{
	DataSource_Iface_Receive,
	DataSource_Iface_Fail,
	DataSource_Routing,
	DataSource_Service,
} RouteDataSource_T;

#pragma pack(push, 1)
// type of usual routing packet header
typedef struct
{
	PacketId_T		Id;		// Packet id
	RouteAddr_T		To,		// receiver of current packet
					From;	// sender of current packet
	LifeTime_T		TTL;	// maximum time left for current packet to exist
	RoutePackType_T Type;   // routing packet type
	SizePayload_T	Size;	// size of payload in current packet
} RouteHeader_T;

typedef struct
{
	RouteAddr_T OriginalTo;
	RouteAddr_T OriginalFrom;
} RouteACKPayload_T;

// struct to hold together packet and "bridge" (using in routing -> interface queue)
typedef struct
{
	Packet_T		Pack;	// current packet itself
	RouteAddr_T		Bridge;	// node to send current packet through (may - even should - change from one sending attempt to another for the same packet)
	PackState_T		State;	// state of packet
	SizePayload_T	Size;	// payload size
	uint8_t 		Attempt;
} RouteMsgDown_T;

typedef struct
{
	RouteAddr_T Peer;
	char P;
} AdHocRecord_T;

typedef struct
{
	Packet_T		Pack;		// current packet itself (place in memory)
	uint8_t			Retries;	// count of this packet already done re-receives ( == 0 at start )
	RouteAddr_T		HostFrom;	// route address of sender host
} RouteMsgUp_T;


typedef struct{
	Packet_T Pack;
	RouteDataSource_T Source;	// source of data, if data send failed | data recieved source is iface, if first try of processing routing | service
	//PackState_T State;			// valid only for data from interface
	uint8_t 		Attempt; 			// by default number of trys to process data
	moarTime_T	NextProcessingTime; 		// default value is zero, after first send try will be set to ROUTING_DEFAULT_DELAY
} RouteInnerData_T;

typedef struct{
	Packet_T Pack;
	uint8_t Count;

} RouteInnerPointer_T;

typedef struct {
	RouteAddr_T	* List;
	PfCount_T	Length;
} RouteAddrList_T;
typedef struct{
	//uint16_t RoutingStackSize;
	uint8_t QueueSize;
	uint8_t SentDataPointersCacheSize; //max size is equal to route down queue

	RouteTableSettings_T RouteTableSettings;
//#define ROUTING_TRYS_COUNT		10
	uint8_t RouteTrysCount;
	uint8_t RouteServiceMsgTrysCount;
//#define ROUTING_DEFAULT_DELAY	50000
	uint32_t RouteDelay;
//#define ROUTING_DEFAULT_TTL		10
	LifeTime_T DefaultTTL;
//#define ROUTING_QUERY_INTERVAL	(1000 * 60 * 2)
	//uint32_t MaintainInterval;
// size of routing probe payload
//#define SzRouteProbeSize 10
//#define QualityThreshold	10
	bool ProbesEnabled; //done

	uint32_t RoutesGCInterval; //done
	uint32_t ProbesSendInterval; //done

	bool SendBackDataNAK;

	uint8_t ProbeSize;
	uint8_t FinderSize;
} RoutingSettings_T;

#pragma pack(pop)

extern RouteDataTable_T * routeTable;
extern bool forceProbeSend;
#ifdef DEBUG_LEVEL1
extern uint8_t intakeCount;
extern uint8_t intakeMaxCount;
#endif

void Route_init(RouteAddr_T selfAddr, RoutingSettings_T* settings);
void Route_MainTaskLoop(void *pvParameters);

#endif /* ROUTING_H_ */
