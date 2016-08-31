//
// Created by svalov on 8/25/16.
//

#include <funcResults.h>
#include <moarInterfaceLoraPrivate.h>
#include <hwInterface.h>
#include <interrupts.h>
#include "loraInterface.h"
#include <interface.h>
#include <string.h>
#include <crc16.h>
#include <moarInterfaceCommand.h>

// returns place in packet where iface header starts
IfaceHeader_T * Iface_startHeader( Packet_T packet ){
	return (IfaceHeader_T*)(packet + IfaceHeaderStart);
}
// returns place in packet where iface payload starts
Packet_T	Iface_startPayload( Packet_T packet ){
	return packet + IfacePayloadStart;
}
// returns place in packet where iface footer starts
IfaceFooter_T * Iface_startFooter( Packet_T packet ){
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
	beaconHeader->Size = (SizePayload_T)size;
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
	layer->StartupTime = timeGetCurrent();
	layer->BeaconSendInterval = layer->Settings.BeaconStartupInterval;
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
uint16_t calcTimeout(LoraIfaceLayer_T* layer, uint16_t size){
	if(layer->NetSpeed > 0)
		return (uint16_t)(((((uint32_t)size+constantMessageOverhead)*1000) / layer->NetSpeed )*layer->Settings.TxTimeoutCoef);
	else
		return layer->Settings.TransmitTimeout;
}

int updateNeighbors(LoraIfaceLayer_T* layer, IfaceHeader_T* header, int16_t rssi){
	if(NULL == header)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	int16_t loss = header->TxPower-rssi;
	int updateRes = FUNC_RESULT_FAILED;
	// if beacons
	if(header->Beacon) {
		IfaceFooter_T* footer = Iface_startFooter(header);
		// fill beacon struct
		NeighborInfo_T neighbor = {0};
		neighbor.SignalLoss = loss;
		neighbor.Address = header->From;
		neighbor.Frequency = footer->FreqStart;
		neighbor.Seed = footer->FreqSeed;
		neighbor.LastFailedTrys = 0;
		neighbor.LastSeen = timeGetCurrent();
		neighbor.MinSensitivity = footer->MinSensitivity;
		updateRes = neighborsUpdate(layer, &neighbor, Iface_startPayload(header), header->Size);
	} else{
		// update last seen
		updateRes = neighborsUpdateLastSeen(layer, &(header->From), loss);
	}
	return updateRes;
}

IfaceListenChannel_T startListen(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return ListenChannel_None;
	printf("start listen\n");
	if(layer->MonitorMode){
		startRx(layer->Settings.MonitorChannel, layer->Settings.MonitorSeed);
		return ListenChannel_Monitor;
	}
	moarTime_T  currentTime = timeGetCurrent();
	if((!layer->WaitingResponse && 	(layer->Neighbors.Count==0 ||
			(timeCompare(timeGetDifference(currentTime, layer->LastBeaconReceived),
				 layer->Settings.BeaconListenForce) > 0)||
			timeCompare(timeGetDifference(currentTime, layer->StartupTime),
				layer->Settings.BeaconListenStartup) < 0 ||
			(layer->ListenBeacon && timeCompare(timeGetDifference(currentTime,layer->ListenBeaconStart),
										layer->Settings.BeaconListenTimeout)<0)))
			){
#ifdef DEBUG_LEVEL1
		DEBUGOUT("Listen for beacon 0x%02x\n",ifaceSettings->BeaconChannel);
#endif

//#ifdef DEBUG_LEVEL3
		if(layer->Neighbors.Count == 0 )
			printf("No neighbors\n");
		if(timeCompare(timeGetDifference(currentTime, layer->LastBeaconReceived),
				 layer->Settings.BeaconListenForce) > 0)
			printf("Time from last beacon %d > %d\n",(currentTime-layer->LastBeaconReceived),layer->Settings.BeaconListenForce);
		if(timeCompare(timeGetDifference(currentTime, layer->StartupTime),
				layer->Settings.BeaconListenStartup) < 0)
			printf("Current time %d < %d\n",currentTime-layer->StartupTime,layer->Settings.BeaconListenStartup);
		if(layer->ListenBeacon && timeCompare(timeGetDifference(currentTime,layer->ListenBeaconStart),
										layer->Settings.BeaconListenTimeout)<0)
		printf("Listen beacon and listened for %d < %d\n",(currentTime-layer->LastBeaconReceived),layer->Settings.BeaconListenTimeout);
//#endif
		if(!layer->ListenBeacon)
			layer->ListenBeaconStart = currentTime;
		layer->ListenBeacon = true;
		startRx(layer->Settings.BeaconChannel, layer->Settings.BeaconSeed);
		return ListenChannel_Beacon;
	}else{
//#ifdef DEBUG_LEVEL1
		printf("Listen for data 0x%02x\n",layer->ListeningChannel);
//#endif
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
	// send
	startTx(layer->Settings.BeaconChannel, layer->Settings.BeaconSeed, layer->BeaconData, layer->BeaconDataSize);
	// timeouts
	layer->TransmitResetTime = timeAddInterval(timeGetCurrent(),calcTimeout(layer, layer->BeaconDataSize));
	layer->LastBeaconSent = timeGetCurrent();
	layer->Busy = true;
	return FUNC_RESULT_SUCCESS;
}
int sendData(LoraIfaceLayer_T* layer, IfaceAddr_T* dest, MessageId_T* mid, bool needResponse, bool isResponse,
			 void* data, PayloadSize_T size){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == dest)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == mid)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == data || 0 == size)
		return FUNC_RESULT_FAILED_ARGUMENT;

	// find neighbor
	NeighborInfo_T neighbor;
	int neighborRes = neighborsGet(layer,dest,&neighbor);
	// if not found send error
	if(FUNC_RESULT_SUCCESS != neighborRes){
		return FUNC_RESULT_FAILED_NEIGHBORS;
	}
	// set mid
	layer->CurrentMid = *mid;
	// prepare ifac
	resetInterfaceState();
	int8_t power = setPower(layer->Settings.DataTxPower, layer->Settings.DataTxBoost);
	// prepare message
	uint8_t* fullData = malloc(SzIfaceHeader + size);
	if(NULL == fullData){
		return FUNC_RESULT_FAILED_MEM_ALLOCATION;
	}
	memcpy(Iface_startPayload(fullData), data, size);
	IfaceHeader_T* header = Iface_startHeader(fullData);

	header->Beacon = 0;
	header->Response = (PackType_T)(isResponse?1:0);
	header->NeedResponse = (PackType_T)(needResponse?1:0);
	header->From = layer->LocalAddress;
	header->To = *dest;
	header->Size = (SizePayload_T)size;
	header->TxPower = power;

	//crc calc
	layer->CurrentCRC = calcPacketCrc(header,true);
	header->CRC = layer->CurrentCRC;
	layer->CurrentFullCRC = calcPacketCrc(header,false);
	// start tx
	startTx(neighbor.Frequency, neighbor.Seed, fullData, SzIfaceHeader + size);
	// set up flags
	layer->TransmitResetTime = timeAddInterval(timeGetCurrent(),calcTimeout(layer, SzIfaceHeader + size));
	layer->WaitingResponse = needResponse;
	// free data
	free(fullData);
	layer->Busy = true;
	return FUNC_RESULT_SUCCESS;
}

// process message function here
int processReceivedMessage(LoraIfaceLayer_T* layer, RxData_T* data){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == data)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == data->Pointer || 0 == data->Size)
		return FUNC_RESULT_FAILED_ARGUMENT;

	IfaceHeader_T* header = Iface_startHeader( data->Pointer );
	layer->TotalPacketsCounter++;
	//check crc
	CRCvalue_T packetValue = header->CRC;
	CRCvalue_T val = calcPacketCrc(header,true);
	if(packetValue==val) {
		printf("Valid CRC\n");
		printf("TxPow %d Rssi %d Loss %d - ",header->TxPower,data->Rssi, header->TxPower-data->Rssi);
		// is response
		//process response state
		//check for valid
		IfaceResponsePayload_T* payload = (IfaceResponsePayload_T *)Iface_startPayload( data->Pointer );
		//if valid
		if(layer->CurrentCRC == payload->NormalMessageCrc && layer->CurrentFullCRC == payload->FullMessageCrc)
		{
			//reset waiting
			layer->WaitingResponse = false;
			layer->WaitingResponseTime = INFINITY_TIME;
			int notifyRes = processIfaceMsgState(layer, &(layer->CurrentMid), IfacePackState_Responsed);
			layer->Busy = false;
		}
		// is beacons
		if(header->Beacon){
			printf("Beacon %d from 0x%08x\n",layer->BeaconCounter, header->From);
			layer->BeaconCounter++;
			layer->LastBeaconReceived = timeGetCurrent();
		}
		// is data
		if(header->Size!=0 && !header->Response && !header->Beacon) {
			// process data
			int notifyRes = processIfaceReceived(layer, &(header->From), Iface_startPayload(header), header->Size);
		}
		// if need response
		if(header->NeedResponse){
			IfaceResponsePayload_T payload = {0};
			payload.NormalMessageCrc = val;
			payload.FullMessageCrc = calcPacketCrc(header,false);
			int responseRes = sendData(layer, &(header->From), NULL, false, true, &payload, sizeof(payload));
		}
		// update neighbors
		if(!layer->MonitorMode)
			updateNeighbors(layer, header,data->Rssi);
	}
	else{
		// broken
		layer->BrokenCounter++;
	}
	data->Processed = true;
	return FUNC_RESULT_SUCCESS;
}
int interfaceStateProcessing(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	moarTime_T  currentTime = timeGetCurrent();
	bool rts = readyToSend() && !layer->WaitingResponse && !layer->MonitorMode && !layer->Busy;
	// waiting response timeout
	if(layer->WaitingResponse && timeCompare(currentTime, layer->WaitingResponseTime)>0){
		// logic here
		// reset busy
		layer->Busy = false;
		// reset time
		layer->WaitingResponseTime = INFINITY_TIME;
		// reset flag
		layer->WaitingResponse = false;
		// notify
		processIfaceMsgState(layer, &(layer->CurrentMid), IfacePackState_Timeouted);
	}
	// send timeout
	if(timeCompare(currentTime, layer->TransmitResetTime)>0)
	{
		// reset lora
		Init_LORA(&(layer->Settings.LORA_Settings));
		resetInterfaceState();
		// reset busy
		layer->Busy = false;
		// reset time
		layer->TransmitResetTime = INFINITY_TIME;

	}

	if(events.RxDone){
		printf("rx done\n");
		resetInterfaceState();
		processReceivedMessage(layer,&recievedData);
		// try to listen
		if(!layer->Busy){
			startListen(layer);
		}
	}

	if(events.TxDone){
		printf("tx done\n");
		layer->TransmitResetTime = INFINITY_TIME;
		if(layer->WaitingResponse)
			layer->WaitingResponseTime = timeAddInterval(currentTime, layer->Settings.WaitingResponseTimeout);
		else {
			layer->Busy = false;
			processIfaceMsgState(layer, &(layer->CurrentMid), IfacePackState_Sent);
		}
		resetInterfaceState();
		startListen(layer);

	}
	// if interface is still free and time to send beacons
	if(rts &&
			timeCompare(timeGetDifference(timeGetCurrent(), layer->LastBeaconSent),layer->BeaconSendInterval)>0){
		printf("Sending beacon %d\n", timeGetDifference(timeGetCurrent(), layer->LastBeaconSent));
		//send beacons
		sendBeacon(layer);

		if(timeCompare(timeGetDifference(timeGetCurrent(), layer->StartupTime),
					   layer->Settings.BeaconStartupDuration)>0)
			layer->BeaconSendInterval = layer->Settings.BeaconSendInterval+
									((rand()%layer->Settings.BeaconSendDeviation)-layer->Settings.BeaconSendDeviation/2);
		// TODO next event time calculation here
	}


	return FUNC_RESULT_SUCCESS;
}
