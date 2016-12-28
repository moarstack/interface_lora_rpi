//
// Created by svalov on 8/25/16.
//

#include <funcResults.h>
#include <stdint.h>
#include <moarInterfaceLoraPrivate.h>
#include <hwInterface.h>
#include <interrupts.h>
#include "loraInterface.h"
#include <interface.h>
#include <string.h>
#include <crc16.h>
#include <moarInterfaceCommand.h>
#include <moarIfaceStructs.h>
#include <moarLoraSettings.h>

// returns place in packet where iface header starts
IfaceHeader_T * Iface_startHeader(void* packet){
	return (IfaceHeader_T*)(packet + IfaceHeaderStart);
}
// returns place in packet where iface payload starts
Packet_T	Iface_startPayload(void* packet){
	return packet + IfacePayloadStart;
}
// returns place in packet where iface footer starts
IfaceFooter_T * Iface_startFooter(void* packet){
	return (IfaceFooter_T *)(Iface_startPayload( packet ) + Iface_startHeader( packet )->Size);
}

int interfaceMakeBeacon(LoraIfaceLayer_T* layer, void* payload, PayloadSize_T size){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LogWrite(layer->Log, LogLevel_DebugQuiet, "Creating beacon with payload");
	//LogWrite(layer->Log, LogLevel_Dump, "New beacon payload is %b", payload, size);
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
	LogWrite(layer->Log, LogLevel_Dump, "New beacon content is %b", layer->BeaconData, layer->BeaconDataSize);
	return  FUNC_RESULT_SUCCESS;
}
int interfaceInit(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Interface low level init");

	// init hardware
#ifdef ENABLE_IO
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Init wiring pi");
	int wpiRes = wiringPiSetup();
#endif
	// init neighbors here
	int neighborsRes = neighborsInit(layer);
	if(FUNC_RESULT_SUCCESS != neighborsRes) {
		LogErrMoar(layer->Log, LogLevel_Critical, neighborsRes, "Neighbors init failed");
		return neighborsRes;
	}
	// init interrupts
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Init interrupts");
	Init_interupts();
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Init LORA");
	bool res = Init_LORA(&(layer->Settings.LORA_Settings), layer->Log);
//	if(!res)
//		return FUNC_RESULT_FAILED;

	//randomizing address
	srand(timeGetCurrent() & 0xFFFFFFFF);
	for(int i=0; i<IFACE_ADDR_SIZE; i++)
		layer->LocalAddress.Address[i] = (uint8_t)(rand()&0xFF);
	LogWrite(layer->Log, LogLevel_Information, "Setting address %b", &(layer->LocalAddress), sizeof(IfaceAddr_T));
	layer->ListeningChannel = rand() % channelsCount;
	layer->ListeningSeed = rand() % UINT16_MAX;
	LogWrite(layer->Log, LogLevel_DebugQuiet, "Setting channel 0x%02x and seed 0x%04x", layer->ListeningChannel, layer->ListeningSeed);

	// create beacon packet here
	int beaconRes = interfaceMakeBeacon(layer, NULL, 0);
	if(FUNC_RESULT_SUCCESS != beaconRes) {
		LogErrMoar(layer->Log, LogLevel_Critical, neighborsRes, "Beacon creating failed");
		return beaconRes;
	}
	layer->ResetEnabled = false;
	layer->StartupTime = timeGetCurrent();
	layer->LastBeaconSent = timeGetCurrent();
	layer->BeaconSendInterval = layer->Settings.BeaconStartupInterval;
	layer->TransmitResetTime = INFINITY_TIME;
	layer->WaitingResponseTime = INFINITY_TIME;
	layer-> IfaceResetTimeout = layer->Settings.BeaconListenForce*10;
	layer->LastResetTime = timeGetCurrent();
	layer->LastReceivedDataTime = INFINITY_TIME;
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
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Updating neighbor %b", &(header->From), sizeof(IfaceAddr_T));
	int16_t loss = header->TxPower-rssi;
	int updateRes = FUNC_RESULT_FAILED;
	// if beacons
	if(header->Beacon) {
		LogWrite(layer->Log, LogLevel_DebugVerbose, "Updating from beacon");
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
		LogWrite(layer->Log, LogLevel_DebugVerbose, "Call beacon update");
		updateRes = neighborsUpdate(layer, &neighbor, Iface_startPayload(header), (PayloadSize_T)header->Size);
	} else{
		LogWrite(layer->Log, LogLevel_DebugVerbose, "Updating last seen");
		// update last seen
		updateRes = neighborsUpdateLastSeen(layer, &(header->From), loss);
	}
	LogErrMoar(layer->Log, LogLevel_DebugVerbose, updateRes, "Neighbor update result");
	return updateRes;
}

IfaceListenChannel_T startListen(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return ListenChannel_None;
	if(layer->MonitorMode){
		startRx(layer->Settings.MonitorChannel, layer->Settings.MonitorSeed);
		LogWrite(layer->Log, LogLevel_DebugVerbose, "Start listen monitor channel 0x%02x seed 0x%04x", layer->Settings.MonitorChannel, layer->Settings.MonitorSeed);
		return ListenChannel_Monitor;
	}
	moarTime_T  currentTime = timeGetCurrent();
	//LogWrite(layer->Log, LogLevel_Dump, "Listen beacon %d and listened for %lld timeout %d",layer->ListenBeacon, (currentTime-layer->ListenBeaconStart), layer->Settings.BeaconListenTimeout);
	if((!layer->WaitingResponse && 	(layer->Neighbors.Count==0 || // not waiting response no neighbors
			(timeCompare(timeGetDifference(currentTime, layer->LastBeaconReceived),
				 layer->Settings.BeaconListenForce) > 0)||  // last beacon was to long ago
			timeCompare(timeGetDifference(currentTime, layer->StartupTime),
				layer->Settings.BeaconListenStartup) < 0 || // startup
			(layer->ListenBeacon && timeCompare(timeGetDifference(currentTime,layer->ListenBeaconStart),
										layer->Settings.BeaconListenTimeout)<0))) //listen for specific time
			){
		if(layer->Neighbors.Count == 0 )
			LogWrite(layer->Log, LogLevel_Dump, "No neighbors\n");
		if(timeCompare(timeGetDifference(currentTime, layer->LastBeaconReceived),
				 layer->Settings.BeaconListenForce) > 0)
			LogWrite(layer->Log, LogLevel_Dump, "Time from last beacon %lld > %d\n",(currentTime-layer->LastBeaconReceived),layer->Settings.BeaconListenForce);
		if(timeCompare(timeGetDifference(currentTime, layer->StartupTime),
				layer->Settings.BeaconListenStartup) < 0)
			LogWrite(layer->Log, LogLevel_Dump, "Current time %lld < %d\n",currentTime-layer->StartupTime,layer->Settings.BeaconListenStartup);
		if(layer->ListenBeacon && timeCompare(timeGetDifference(currentTime,layer->ListenBeaconStart),
										layer->Settings.BeaconListenTimeout)<0)
			LogWrite(layer->Log, LogLevel_Dump, "Listen beacon and listened for %lld < %d\n",(currentTime-layer->ListenBeaconStart),layer->Settings.BeaconListenTimeout);
		if(!layer->ListenBeacon)
			layer->ListenBeaconStart = currentTime;
		layer->ListenBeacon = true;
		startRx(layer->Settings.BeaconChannel, layer->Settings.BeaconSeed);
		LogWrite(layer->Log, LogLevel_DebugVerbose, "Start listen beacon channel 0x%02x seed 0x%04x", layer->Settings.BeaconChannel, layer->Settings.BeaconSeed);
		return ListenChannel_Beacon;
	}else{
		layer->Startup = false;
		layer->ListenBeaconStart = INFINITY_TIME;
		layer->ListenBeacon = false;
		startRx(layer->ListeningChannel,layer->ListeningSeed);
		LogWrite(layer->Log, LogLevel_DebugVerbose, "Start listen data channel 0x%02x seed 0x%04x", layer->ListeningChannel,layer->ListeningSeed);
		return ListenChannel_Data;
	}
}
int sendBeacon(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	layer->SentBeaconCounter++;
	LogWrite(layer->Log, LogLevel_Information, "Sending beacon %d", layer->SentBeaconCounter);
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
	layer->TransmitStartTime = timeGetCurrent();
	layer->CurrentSize = layer->BeaconDataSize;
	layer->Busy = true;
	layer->LastIsBeacon = true;
	return FUNC_RESULT_SUCCESS;
}
int sendData(LoraIfaceLayer_T* layer, IfaceAddr_T* dest, bool needResponse, bool isResponse,
			 void* data, PayloadSize_T size){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == dest)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == data || 0 == size)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LogWrite(layer->Log, LogLevel_Information, "Sending data to %b", dest, sizeof(IfaceAddr_T));

	// find neighbor
	NeighborInfo_T neighbor;
	int neighborRes = neighborsGet(layer,dest,&neighbor);
	// if not found send error
	if(FUNC_RESULT_SUCCESS != neighborRes){
		LogWrite(layer->Log, LogLevel_Warning, "Neighbor not found %b", dest, sizeof(IfaceAddr_T));
		return FUNC_RESULT_FAILED_NEIGHBORS;
	}
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
	LogWrite(layer->Log, LogLevel_Dump, "Data is %b", fullData, SzIfaceHeader + size);
	// start tx
	startTx(neighbor.Frequency, neighbor.Seed, fullData, SzIfaceHeader + size);
	// set up flags
	layer->TransmitStartTime = timeGetCurrent();
	layer->TransmitResetTime = timeAddInterval(timeGetCurrent(),calcTimeout(layer, SzIfaceHeader + size));
	layer->WaitingResponse = needResponse;
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Waiting response %d", layer->WaitingResponse);
	layer->LastIsBeacon = false;
	layer->CurrentSize = SzIfaceHeader + size;
	layer->CurrentMsgAddr = *dest;
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
		LogWrite(layer->Log, LogLevel_Information, "Received data from %b, size %hu", &(header->From), sizeof(IfaceAddr_T), data->Size);
		LogWrite(layer->Log, LogLevel_DebugQuiet, "TxPow %d Rssi %d Loss %d",header->TxPower,data->Rssi, header->TxPower-data->Rssi);
		LogWrite(layer->Log, LogLevel_Dump, "Data is %b", data->Pointer, data->Size);
		// is response
		//process response state
		//check for valid
		IfaceResponsePayload_T* payload = (IfaceResponsePayload_T *)Iface_startPayload( data->Pointer );
		//if valid
		if(header->Response && layer->CurrentCRC == payload->NormalMessageCrc && layer->CurrentFullCRC == payload->FullMessageCrc)
		{
			LogWrite(layer->Log, LogLevel_DebugQuiet, "Valid Response for message %b",&(layer->CurrentMid), sizeof(MessageId_T));
			//reset waiting
			layer->WaitingResponse = false;
			layer->WaitingResponseTime = INFINITY_TIME;
			int updateRes = neighborsUpdateSendtrys(layer, &(layer->CurrentMsgAddr), true);
			int notifyRes = processIfaceMsgState(layer, &(layer->CurrentMid), IfacePackState_Responsed);
			layer->Busy = false;
		}
		// is beacons
		if(header->Beacon){
			layer->BeaconCounter++;
			LogWrite(layer->Log, LogLevel_DebugQuiet, "Beacon %d",layer->BeaconCounter);
			layer->LastBeaconReceived = timeGetCurrent();
		}
		// is data
		if(header->Size!=0 && !header->Response && !header->Beacon) {
			LogWrite(layer->Log, LogLevel_DebugQuiet, "Data %d bytes", header->Size);
			layer->LastReceivedDataTime = timeGetCurrent();
			// process data
			int notifyRes = processIfaceReceived(layer, &(header->From), Iface_startPayload(header), header->Size);
		}
		// if need response
		if(header->NeedResponse){
			LogWrite(layer->Log, LogLevel_DebugVerbose, "Message need response");
			IfaceResponsePayload_T payload = {0};
			payload.NormalMessageCrc = val;
			payload.FullMessageCrc = calcPacketCrc(header,false);
			LogWrite(layer->Log, LogLevel_Information, "Sending response");
			int responseRes = sendData(layer, &(header->From), false, true, &payload, sizeof(payload));
			LogErrMoar(layer->Log, LogLevel_DebugQuiet, responseRes, "Sending response result");
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
		LogWrite(layer->Log, LogLevel_Warning, "No response in timeout");
		// logic here
		int updateRes = neighborsUpdateSendtrys(layer, &(layer->CurrentMsgAddr), false);
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
	if((layer->Busy && timeCompare(currentTime, layer->TransmitResetTime)>0) ||
			(// last reset was long long time ago
			(timeCompare(currentTime, timeAddInterval(layer->LastResetTime, layer->IfaceResetTimeout))>0) && (
			// data not received to long
			//(timeCompare(currentTime, timeAddInterval(layer->LastReceivedDataTime, layer->IfaceResetTimeout))>0) ||
			// beacons not received
			(timeCompare(currentTime, timeAddInterval(layer->LastBeaconReceived, layer->IfaceResetTimeout))>0))
			))
	{
		LogWrite(layer->Log, LogLevel_Warning, "Reseting lora interface");
		LogWrite(layer->Log, LogLevel_Dump, "Layer busy %d and tx timeout %d", layer->Busy, timeCompare(currentTime, layer->TransmitResetTime));
		LogWrite(layer->Log, LogLevel_Dump, "Inter reset interval %d", timeCompare(currentTime, timeAddInterval(layer->LastResetTime, layer->IfaceResetTimeout)));
		LogWrite(layer->Log, LogLevel_Dump, "No data to long %d", timeCompare(currentTime, timeAddInterval(layer->LastReceivedDataTime, layer->IfaceResetTimeout)));
		LogWrite(layer->Log, LogLevel_Dump, "No beacons to long %d", timeCompare(currentTime, timeAddInterval(layer->LastBeaconReceived, layer->IfaceResetTimeout)));
		// reset lora
		Init_LORA(&(layer->Settings.LORA_Settings), layer->Log);
		resetInterfaceState();
		layer->LastResetTime = currentTime;
		// reset busy
		layer->Busy = false;
		// reset time
		layer->TransmitResetTime = INFINITY_TIME;
	}

	if(events.RxDone){
		resetInterfaceState();
		processReceivedMessage(layer,&recievedData);
		// try to listen
		if(!layer->Busy){
			startListen(layer);
		}
	}

	if(events.TxDone){
		layer->TransmitResetTime = INFINITY_TIME;
		//calculate net speed
		if(layer->NetSpeed == 0){
			moarTime_T transmitTime = timeGetDifference(currentTime, layer->TransmitStartTime);
			if(transmitTime != 0) {
				LogWrite(layer->Log, LogLevel_Dump, "Transmission time %lld", transmitTime);
				uint32_t messageSize = (uint32_t) ((layer->CurrentSize + constantMessageOverhead) * 1000);
				layer->NetSpeed = messageSize / transmitTime;
				LogWrite(layer->Log, LogLevel_Dump, "Transmission speed %d bytes/sec", layer->NetSpeed);
			}
		}
		if(layer->WaitingResponse) {
			layer->WaitingResponseTime = timeAddInterval(currentTime, layer->Settings.WaitingResponseTimeout);
			LogWrite(layer->Log, LogLevel_DebugVerbose, "Setting wait response time");
		}
		else {
			LogWrite(layer->Log, LogLevel_DebugVerbose, "Sending done");
			layer->Busy = false;
			if(!layer->LastIsBeacon)
				processIfaceMsgState(layer, &(layer->CurrentMid), IfacePackState_Sent);
		}
		resetInterfaceState();
		startListen(layer);

	}
	// if interface is still free and time to send beacons
	if(rts &&
			timeCompare(timeGetDifference(currentTime, layer->LastBeaconSent),layer->BeaconSendInterval)>0){
		//send beacons
		sendBeacon(layer);

		if(timeCompare(timeGetDifference(currentTime, layer->StartupTime),
					   layer->Settings.BeaconStartupDuration)>0) {
			layer->BeaconSendInterval = layer->Settings.BeaconSendInterval +
										((rand() % layer->Settings.BeaconSendDeviation) -
										 layer->Settings.BeaconSendDeviation / 2);
			LogWrite(layer->Log, LogLevel_Dump, "Changing beacon send interval to %lld", layer->BeaconSendInterval);
		}
	}

	moarTime_T min = EPOLL_TIMEOUT;
	moarTime_T toWaiting = timeGetDifference(layer->WaitingResponseTime, currentTime);
	min = (timeCompare(min, toWaiting) < 0) ? min : toWaiting;
	moarTime_T toTransmit = timeGetDifference(layer->TransmitResetTime, currentTime);
	min = (timeCompare(min, toTransmit) < 0) ? min : toTransmit;
	moarTime_T toBeacon = timeGetDifference(timeAddInterval(layer->LastBeaconSent,layer->BeaconSendInterval), currentTime);
	min = (timeCompare(min, toBeacon) < 0) ? min : toBeacon;
	min = min>0 ? min: EPOLL_TIMEOUT;
	layer->EpollTimeout = min;

	LogWrite(layer->Log, LogLevel_Dump, "Setting next epoll timeout %d", layer->EpollTimeout);

	return FUNC_RESULT_SUCCESS;
}
