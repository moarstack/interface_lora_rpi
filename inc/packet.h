/*
 * packet.h
 *
 *  Created on: 06 фев. 2016 г.
 *      Author: kryvashek
 */

#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>

// whole-stack typedef for packet sizes
typedef uint16_t	SizePacket_T;	//type for calculating payload, header and whole packet sizes
typedef uint8_t		SizePayload_T;	//type for specifying payload size in the packet actually
typedef uint8_t*	Packet_T;	// place in memory containing ALL the data of current packet (including headers)

// any possible state or result of current layer work under current packet
typedef enum
	{
	PackState_none, 		//not defined state of enum
	PackState_beaconupdate,  //update beacon payload
	PackState_beaconupdated, //beacon payload was updated
	PackState_unprepared,	// current packet got from upper layer and is NOT ready for sending (i.e. has no header for current layer)
	PackState_prepared,		// current packet got from upper layer and is ready for sending (has good header for current layer)
	PackState_sending,		// current packet is sending now
	PackState_sent,			// current packet is sent and has no need to get response
	PackState_waiting,		// current packet is sent and now is waiting for response
	PackState_responsed,	// current packet was sent and got response in time
	PackState_timeouted,	// current packet was sent and got NO response in time
	PackState_unknownDest,	// current packet was not sent due to no neighbor info found
	PackState_received		// current packet was received
	} PackState_T;


#endif /* PACKET_H_ */
