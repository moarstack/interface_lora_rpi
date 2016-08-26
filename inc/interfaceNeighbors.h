//
// Created by svalov on 8/26/16.
//

#ifndef MOARSTACK_INTERFACENEIGHBORS_H
#define MOARSTACK_INTERFACENEIGHBORS_H

#include <moarInterfaceLoraPrivate.h>
#include <moarTime.h>
#include <moarIfaceStructs.h>
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

int neighborsInit(LoraIfaceLayer_T* layer);
int neighborsAdd(LoraIfaceLayer_T* layer, NeighborInfo_T* neighbor);
int neighborsUpdate(LoraIfaceLayer_T* layer, NeighborInfo_T* neighbor);
int neighborsGet(LoraIfaceLayer_T* layer, IfaceAddr_T* addr, NeighborInfo_T* neighbor);
int neighborsRemove(LoraIfaceLayer_T* layer, IfaceAddr_T* addr);
int neighborsUpdateSendtrys(LoraIfaceLayer_T* layer, IfaceAddr_T* addr, bool responded);
int neighborsUpdateLastSeen(LoraIfaceLayer_T* layer, IfaceAddr_T* addr);

#endif //MOARSTACK_INTERFACENEIGHBORS_H
