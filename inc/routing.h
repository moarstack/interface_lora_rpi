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
typedef int RouteAddr_T;

typedef struct
{
	Packet_T		Pack;	// current packet itself
	RouteAddr_T		Bridge;	// node to send current packet through (may - even should - change from one sending attempt to another for the same packet)
	PackState_T		State;	// state of packet
	SizePayload_T	Size;	// payload size
	uint8_t 		Attempt;
} RouteMsgDown_T;

#endif /* ROUTING_H_ */
