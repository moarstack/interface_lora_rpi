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
#include <crc16.h>
#include <hashTable.h>

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
	layer->BeaconDataSize = (PayloadSize_T)fullSize;
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

//calculate crc, use override if need crc only for data+header
CRCvalue_T calcPacketCrc(IfaceHeader_T* packet, bool override){
	uint8_t* data = (uint8_t*)((void*)packet);
	uint16_t length = SzIfaceHeader;
	CRCvalue_T oldVal = packet->CRC;
	//override old value
	if(override)
		packet->CRC = 0;
	//data
	length+=packet->Size;
	//footer
	if(packet->Beacon)
		length+=SzIfaceFooter;
	//limit max size
	if(length>SzIfaceMaxPacket)
		length = SzIfaceMaxPacket;
	//calc
	uint16_t value = crc16(data,length);
	//return back
	if(override)
		packet->CRC = oldVal;
	return value;
}

uint16_t calcTimeout(LoraIfaceLayer_T* layer, uint16_t size)
{
	if(layer->NetSpeed > 0)
		return ((((uint32_t)size+constantMessageOverhead)*1000) / layer->NetSpeed )*layer->Settings.TxTimeoutCoef;
	else
		return layer->Settings.TransmitTimeout;
}

IfaceListenChannel_T startListen(LoraIfaceLayer_T* layer){

	if(layer->MonitorMode){
		startRx(layer->Settings.MonitorChannel, layer->Settings.MonitorSeed);
		return ListenChannel_Monitor;
	}
	moarTime_T  currentTime = timeGetCurrent();
	if((!layer->WaitingResponse && 	(layer->Neighbors.Count==0 ||
			(timeCompare(timeGetDifference(currentTime, layer->LastBeaconReceived),
				 layer->Settings.BeaconListenForce) > 0)||
			timeCompare(timeGetDifference(currentTime, layer->StartupTime),
				layer->Settings.BeaconListenStartup) ||
			(layer->ListenBeacon && timeCompare(timeGetDifference(currentTime,layer->ListenBeaconStart),
										layer->Settings.BeaconListenTimeout)<0)))
			){
#ifdef DEBUG_LEVEL1
		DEBUGOUT("Listen for beacon 0x%02x\n",ifaceSettings->BeaconChannel);
#endif

#ifdef DEBUG_LEVEL3
		if(neighborsCount==0)
			DEBUGOUT("No neighbors\n");
		if(currentTime-lastBeaconReceived>ifaceSettings->BeaconListenForce)
			DEBUGOUT("Time from last beacon %d > %d\n",(uint32_t)(currentTime-lastBeaconReceived),ifaceSettings->BeaconListenForce);
		if(currentTime<ifaceSettings->BeaconListenStartup)
			DEBUGOUT("Current time %d < %d\n",(uint32_t)currentTime,ifaceSettings->BeaconListenStartup);
		if(listenBeacon && (currentTime-listenBeaconStart<ifaceSettings->BeaconListenTimeout))
			DEBUGOUT("Listen beacon and listened for %d < %d\n",(uint32_t)(currentTime-listenBeaconStart),ifaceSettings->BeaconListenTimeout);
#endif
		if(!layer->ListenBeacon)
			layer->ListenBeaconStart = currentTime;
		layer->ListenBeacon = true;
		startRx(layer->Settings.BeaconChannel, layer->Settings.BeaconSeed);
		return ListenChannel_Beacon;
	}else{
#ifdef DEBUG_LEVEL1
		DEBUGOUT("Listen for data 0x%02x\n",listeningChannel);
#endif
		layer->Startup = false;
		layer->ListenBeaconStart = INFINITY_TIME;
		layer->ListenBeacon = false;
		startRx(layer->ListeningChannel,layer->ListeningSeed);
		return ListenChannel_Data;
	}
}
int sendBeacon(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	//prepare
	//calculate power
	resetInterfaceState();
	//set power
	int8_t power = setPower(layer->Settings.BeaconTxPower, layer->Settings.BeaconTxBoost);
	//set in beacon
	IfaceHeader_T* beaconHeader = Iface_startHeader( layer->BeaconData );
	beaconHeader->TxPower = power;
	//set min sensetivity
	IfaceFooter_T* beaconFooter = Iface_startFooter( layer->BeaconData );
	beaconFooter->MinSensitivity = minSensetivity; //TODO fix
	//crc
	CRCvalue_T val = calcPacketCrc(beaconHeader,true);
	beaconHeader->CRC = val;
#ifdef DEBUG_LEVEL1
	DEBUGOUT("Sending beacon %d\n",sentBeaconCounter++);
#endif

	layer->TransmitResetTimeout = calcTimeout(layer, layer->BeaconDataSize);

#ifdef DEBUG_LEVEL3
	DEBUGOUT("Setting timeout to %d\n",transmitResetTimeout);
#endif
	startTx(layer->Settings.BeaconChannel, layer->Settings.BeaconSeed, layer->BeaconData, layer->BeaconDataSize);
	layer->TransmitStartTime = timeGetCurrent();
	layer->LastBeaconSent = timeGetCurrent();
	layer->Busy = true;
	return FUNC_RESULT_SUCCESS;
}
int sendData(LoraIfaceLayer_T* layer, IfaceAddr_T* dest, MessageId_T* mid, bool needResponse, void* data, PayloadSize_T size){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == dest)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == mid)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == data || 0 == size)
		return FUNC_RESULT_FAILED_ARGUMENT;

	// find neighbor
	// if not found send error
	// prepare message
	// start tx
	// set up flags

	return FUNC_RESULT_SUCCESS;
}

// process message function here
int processReceivedMessage(LoraIfaceLayer_T* layer, RxData_T* data){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == data)
		return FUNC_RESULT_FAILED_ARGUMENT;

	return FUNC_RESULT_SUCCESS;
}

int interfaceStateProcessing(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;

	//if timeouted

	if(events.RxDone){
		resetInterfaceState();
		processReceivedMessage(layer,&recievedData);
		// if no need to transmit response
		// reset busy flag
		// try to listen
	}
	if(events.TxDone){
		// if waiting response
		// set timeout value
	}
	// if interface is still free
	if()
	//send beacons

	return FUNC_RESULT_SUCCESS;
}
