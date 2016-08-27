//
// Created by svalov on 8/25/16.
//

#include <funcResults.h>
#include <moarInterfaceLoraPrivate.h>
#include <moarLoraSettings.h>
#include <hwInterface.h>
#include <interrupts.h>
#include <wiringPi.h>
#include "loraInterface.h"
#include <hwConfig.h>
#include <interface.h>
#include <stdlib.h>
#include <string.h>

// returns place in packet where iface header starts
IfaceHeader_T * Iface_startHeader( Packet_T packet )
{
	return (IfaceHeader_T*)(packet + IfaceHeaderStart);
}

// returns place in packet where iface payload starts
Packet_T	Iface_startPayload( Packet_T packet )
{
	return packet + IfacePayloadStart;
}

// returns place in packet where iface footer starts
IfaceFooter_T * Iface_startFooter( Packet_T packet )
{
	return (IfaceFooter_T *)(Iface_startPayload( packet ) + Iface_startHeader( packet )->Size);
}

int interfaceMakeBeacon(LoraIfaceLayer_T* layer, void* payload, PayloadSize_T size){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;

	size_t fullSize = SzIfaceHeader+size+SzIfaceFooter;
	void* newBeaconData = malloc(fullSize);
	if(NULL == newBeaconData)
		return FUNC_RESULT_FAILED_MEM_ALLOCATION;

	IfaceHeader_T* beaconHeader = Iface_startHeader( newBeaconData );
	beaconHeader->Beacon = 1;
	beaconHeader->Response = 0;
	beaconHeader->NeedResponse = 0;
	//beaconHeader->Routing = 0;
	beaconHeader->From = layer->LocalAddress;
	beaconHeader->Size = size;
	beaconHeader->To = layer->Settings.BeaconAddress;
	if(size !=0 && payload != NULL) {
		memcpy(Iface_startPayload(newBeaconData), payload, size);
	}
	IfaceFooter_T* beaconFooter = Iface_startFooter( newBeaconData );
	beaconFooter->FreqSeed = layer->ListeningSeed;
	beaconFooter->FreqStart = layer->ListeningChannel;

	void* oldPointer = layer->BeaconData;
	layer->BeaconData = newBeaconData;
	if(oldPointer!=NULL){
		free(oldPointer);
	}
	return  FUNC_RESULT_SUCCESS;
}

int interfaceInit(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	// init hardware
#ifdef ENABLE_IO
	int wpiRes = wiringPiSetup();
#endif
	// init interrupts
	Init_interupts();
	bool res = Init_LORA(&(layer->Settings.LORA_Settings));
//	if(!res)
//		return FUNC_RESULT_FAILED;
	// init neighbors here
	int neighborsRes = neighborsInit(layer);
	// create beacon packet here
	int beaconRes = interfaceMakeBeacon(layer, NULL, 0);

	// reset interface state here
	return FUNC_RESULT_SUCCESS;
}

// process message function here


int interfaceStateProcessing(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;

	return FUNC_RESULT_SUCCESS;
}
