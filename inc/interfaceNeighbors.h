//
// Created by svalov on 8/26/16.
//

#ifndef MOARSTACK_INTERFACENEIGHBORS_H
#define MOARSTACK_INTERFACENEIGHBORS_H

#include <moarInterfaceLoraPrivate.h>
//neighbor struct //need critical section on operation
typedef struct{
	IfaceAddr_T Address;
	moarTime_T LastSeen;
	FreqSeed_T	Seed;
	int16_t		SignalLoss;
	FreqNum_T	Frequency;
	PowerTx_T	MinSensitivity;
	uint8_t		LastFailedTrys;
} NeighborInfo_T;

#endif //MOARSTACK_INTERFACENEIGHBORS_H
