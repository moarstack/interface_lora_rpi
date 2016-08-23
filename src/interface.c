/*
 * interface.c
 *
 *  Created on: 16 янв. 2016 г.
 *      Author: kryvashek
 */

#include "stdint.h"
#include "stdlib.h"
#include "interface.h"
#include "task.h"
#include "hwInterface.h"
#include "crc16.h"
#include "routing.h"
#include "rnd.h"
#include "queuedec.h"

//address
IfaceAddr_T nodeAddress;
uint8_t listeningChannel;
uint16_t listeningSeed;

//neighbors

uint16_t beaconSendInterval;

//last beacon received
portTickType lastBeaconReceived = 0; // was -1
portTickType lastReset = 0; // was -1
//next beacon send
portTickType lastBeaconSent = 0;
//waiting respons
bool waitingResponse = false;
uint16_t waitingResponseTimeout;
portTickType waitingResponseResetTime = INFINITY_TIME;
bool enableReset = false;
uint16_t transmitResetTimeout = INFINITY_TIME;
portTickType transmitStartTime = 0;
//time of last rx
portTickType lastReceived = 0;
portTickType currentTime;

//transmission state
TransmissionState_T transmissionState;

//counters
uint16_t beaconCounter = 0;
uint16_t sentBeaconCounter = 0;
uint16_t totalCounter = 0;
uint16_t brokenCounter = 0;
uint16_t dataCounter = 0;
//total speed in bytes per second including preamble, header, and crc
uint16_t netSpeed = 0;

//cached message
RouteMsgDown_T currentMessage;
CRCvalue_T currentMessageCrc;
CRCvalue_T currentMessageCrcFull;
uint16_t currentMessageSize;

//rx done behavior control
bool lastIsBeacon = false;
bool sendingResponse = false;
bool startup = true;

//beacon data
uint8_t* beaconData;
RouteMsgDown_T beaconUpdate;
bool listenBeacon = false;
portTickType listenBeaconStart;
bool monitorMode = false;
bool monitorState = false;
NeighborInfo_T* neighbors;
uint8_t neighborsCount=0;
IfaceSettings_T* ifaceSettings = NULL;


// returns place in packet where iface header starts
inline IfaceHeader_T * Iface_startHeader( Packet_T packet )
{
	return packet + IfaceHeaderStart;
}

// returns place in packet where iface payload starts
inline Packet_T	Iface_startPayload( Packet_T packet )
{
	return packet + IfacePayloadStart;
}

// returns place in packet where iface footer starts
inline IfaceFooter_T * Iface_startFooter( Packet_T packet )
{
	return Iface_startPayload( packet ) + Iface_startHeader( packet )->Size;
}

void enqueueUpstreamMessage(RouteMsgDown_T message){
	IfaceMsgUp_T upMessage;
	upMessage.Pack = message.Pack;
	upMessage.Size = message.Size;
	upMessage.State = message.State;
	upMessage.Attempt = message.Attempt;
#ifdef DEBUG_LEVEL3
	if( Iface_startHeader( message.Pack )->Response)
		DEBUGOUT("RESPONSE\n");
#endif
	QueuedecWrite(QName_ifaceup,&upMessage);
}

int8_t findNeighbor(IfaceAddr_T addr){
	uint8_t i = 0;
	for(;i<neighborsCount;i++){
		if(neighbors[i].Address==addr)
			return i;
	}
	return -1;
}
void removeNeighbor(uint8_t index){
#ifdef DEBUG_LEVEL1
	DEBUGOUT("Remove neighbor %d - 0x%08x\n", index, neighbors[index].Address);
#endif
	//critical section here
	vTaskSuspendAll();
	uint8_t i;
	for(i=index;i<neighborsCount-1;i++)
		neighbors[i] = neighbors[i+1];
	neighborsCount--;
	xTaskResumeAll();
}
void updateNeighbor(bool add, uint8_t index, IfaceHeader_T* header, int16_t rssi){
	NeighborInfo_T neighbor;
	if(!add)
		neighbor = neighbors[index];
	if(add)
		neighbor.Address = header->From;
	neighbor.LastSeen = currentTime;
	neighbor.LastFailedTrys = 0;
	neighbor.SignalLoss = header->TxPower - rssi;
	if(header->Beacon){
		IfaceFooter_T * beaconFooter = Iface_startFooter( ( Packet_T )header );
		neighbor.MinSensitivity = beaconFooter->MinSensitivity;
		neighbor.Frequency = beaconFooter->FreqStart;
		neighbor.Seed = beaconFooter->FreqSeed;
	}
	neighbors[index] = neighbor;
#ifdef DEBUG_LEVEL1
	if(add)
		DEBUGOUT("Adding new neighbor %d 0x%08x\n",index, header->From);
#ifdef DEBUG_LEVEL2
	else
		DEBUGOUT("Updating neighbor %d 0x%08x\n",index, header->From);
#endif
#endif
}

void updateNeighbors(IfaceHeader_T* header, int16_t rssi){
	if(neighborsCount<ifaceSettings->NeighborsCount){
		//search
		int index = findNeighbor(header->From);
		//if found
		if(index!=-1)
			updateNeighbor(false, index,header,rssi);
		//if not found and beacon
		if(index==-1 && header->Beacon &&
				(rssi > ifaceSettings->BeaconRssiMinThreshold && rssi < ifaceSettings->BeaconRssiMaxThreshold)){
			updateNeighbor(true, neighborsCount,header,rssi);
			neighborsCount++;
		}
	}
	//else find and replace
}

void updateLastTrys(IfaceAddr_T addr, bool responded){
	int8_t index = findNeighbor(addr);
	if(index>=0){
		if(responded){
			neighbors[index].LastFailedTrys = 0;
		}
		else if(++(neighbors[index].LastFailedTrys)>ifaceSettings->MaxNeighborSendTrys){
			//remove neighbor
			removeNeighbor(index);
		}
	}
}

void reallocateData(RxData_T* recievedData){
	uint8_t* staticAllocated = recievedData->Pointer;
	void* dynamicAllocated = pvPortMalloc( recievedData->Size );
	if( dynamicAllocated )
		memcpy( dynamicAllocated, staticAllocated, recievedData->Size );
	recievedData->Pointer = dynamicAllocated;
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

void makeBeacon(uint8_t payloadSize, void * payload){
	uint16_t size = SzIfaceHeader+payloadSize+SzIfaceFooter;
	uint8_t* beaconDataTemp = (uint8_t*)pvPortMalloc(size);
	if(beaconDataTemp!=NULL){
		IfaceHeader_T* beaconHeader = Iface_startHeader( beaconDataTemp );
		beaconHeader->Beacon = 1;
		beaconHeader->Response = 0;
		beaconHeader->NeedResponse = 0;
		//beaconHeader->Routing = 0;
		beaconHeader->From = nodeAddress;
		beaconHeader->Size = payloadSize;
		if(payloadSize !=0 && payload != NULL){
			memcpy( Iface_startPayload( beaconDataTemp ) ,payload,payloadSize);
		}
		beaconHeader->To = ifaceSettings->BeaconAddress;
		IfaceFooter_T* beaconFooter = Iface_startFooter( beaconDataTemp );
		beaconFooter->FreqSeed = listeningSeed;
		beaconFooter->FreqStart = listeningChannel;
		uint8_t* oldPointer = beaconData;
		beaconData = beaconDataTemp;
		if(oldPointer!=NULL){
			vPortFree(oldPointer);
		}
	}
}

void enqueueResponse(IfaceHeader_T* header, CRCvalue_T crc){
	//calc second crc
	CRCvalue_T crc_full = calcPacketCrc(header,false);
	//build new message
	uint8_t* data = (uint8_t*)pvPortMalloc(SzIfaceHeader+sizeof(IfaceResponsePayload_T));
	//header
	IfaceHeader_T* respHeader = Iface_startHeader( data );
	respHeader->From = nodeAddress;
	respHeader->To = header->From;
	respHeader->Beacon = 0;
	//respHeader->Routing = 0;
	respHeader->Response = 1;
	respHeader->NeedResponse = 0;
	respHeader->Size = sizeof(IfaceResponsePayload_T);
	//payload
	IfaceResponsePayload_T* response = (IfaceResponsePayload_T*)Iface_startPayload( data );
	response->FullMessageCrc = crc_full;
	response->NormalMessageCrc = crc;
	//add to queue
	RouteMsgDown_T message;
	message.Pack = data;
	message.Size = respHeader->Size;
	message.State = PackState_prepared;
	message.Bridge = header->From;
	//add to queue
	QueuedecWriteToFront(QName_routedown,&message);
}
//queue in, out, spi port, address
void Init_Interface(IfaceAddr_T address, IfaceSettings_T* settings){
	nodeAddress = address;
	//srand(address);
	//pseudorandom send start
	lastBeaconSent = random()%beaconSendInterval;
	Init_LORA(&(settings->LORA_Settings));
	ifaceSettings = settings;
	listeningChannel = random()%channelsCount;
	listeningSeed = random()&UINT16_MAX;

	beaconSendInterval = ifaceSettings->BeaconSendInterval;
	waitingResponseTimeout = ifaceSettings->WaitingResponseTimeout;
	transmitResetTimeout = ifaceSettings->TransmitTimeout;
	lastBeaconSent = 0;

	if(neighbors)
		vPortFree(neighbors);
	neighbors = pvPortMalloc(ifaceSettings->NeighborsCount*sizeof(NeighborInfo_T));
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Listen channel 0x%02x\n",listeningChannel);
	DEBUGOUT("Channel seed 0x%04x\n",listeningSeed);
#endif
	//some other stuff
	makeBeacon(settings->BeaconDefaultPayloadSize, NULL);
	resetInterfaceState();
}

//process message
void processMessage(RxData_T* data){
#ifdef DEBUG_LEVEL2
	DEBUGOUT("Processing data %d\n",data->Size);
#endif
	//get from processing queue
	IfaceHeader_T* header = Iface_startHeader( data->Pointer );
	//check crc
	CRCvalue_T packetValue = header->CRC;
	CRCvalue_T val = calcPacketCrc(header,true);
	totalCounter++;
	lastReceived = currentTime;
	if(packetValue==val){
#ifdef DEBUG_LEVEL2
		DEBUGOUT("Valid CRC\n");
#endif
#ifdef DEBUG_LEVEL1
		DEBUGOUT("TxPow %d Rssi %d Loss %d - ",header->TxPower,data->Rssi, header->TxPower-data->Rssi);
#endif
		//is response
		if(header->Response && waitingResponse){
#ifdef DEBUG_LEVEL1
			DEBUGOUT("Is response\n");
#endif
			//process response state
			//check for valid
			IfaceResponsePayload_T* payload = (IfaceResponsePayload_T *)Iface_startPayload( data->Pointer );
			//if valid
			if(currentMessageCrc == payload->NormalMessageCrc && currentMessageCrcFull == payload->FullMessageCrc)
			{
				//reset waiting
				waitingResponse = false;
				waitingResponseResetTime = INFINITY_TIME;
				//send ok to routing
				currentMessage.State = PackState_responsed;
				updateLastTrys(currentMessage.Bridge, true);
				enqueueUpstreamMessage(currentMessage);
			}
		}
		//is beacon
		if(header->Beacon){
#ifdef DEBUG_LEVEL1
			DEBUGOUT("Beacon %d from 0x%08x\n",beaconCounter, header->From);
#endif
			beaconCounter++;
			lastBeaconReceived = currentTime;
		}
		//is data
		if(header->Size!=0 && !header->Response){
#ifdef DEBUG_LEVEL1
			DEBUGOUT("Contains %d bytes payload data\n",header->Size);
#endif
			dataCounter++;
#ifdef DEBUG_LEVEL3
			DEBUGOUT("Assert data pointer 0x%08x\n",(uint32_t)data->Pointer);
			DEBUGOUT("Assert header pointer 0x%08x\n",(uint32_t)header);
			DEBUGOUT("Assert header size 0x%02x\n",SzIfaceHeader);
			DEBUGOUT("Assert payload size 0x%02x\n",header->Size);
			DEBUGOUT("DATA\n");
			int i=0, j=0;
			for(i=0;i<header->Size;i++){
				DEBUGOUT("0x%02x ",data->Pointer[i+SzIfaceHeader]);
				if(++j==16)	{j=0; DEBUGOUT("\n");	}
			} DEBUGOUT("\n");
#endif
			//check address or beacon
			if(header->Beacon || (header->To == nodeAddress) && !monitorMode){
				reallocateData(data);
				if(data->Pointer){
					IfaceMsgUp_T message;
					message.Pack = data->Pointer;
					message.Size = header->Size;
					message.State = PackState_received;
#ifdef DEBUG_LEVEL3
					if(header->Response)
						DEBUGOUT("RESPONSE\n");
#endif
					if(!QueuedecWrite(QName_ifaceup,&message))
						vPortFree(data->Pointer); //free if failed
				}//if can allocate new
			}//if beacon || correct address
		}//have size and not response
		//need response
		if(header->NeedResponse){
#ifdef DEBUG_LEVEL1
			DEBUGOUT("Need response\n");
#endif
			enqueueResponse(header, val);
		}
		if(!monitorMode)
			updateNeighbors(header,data->Rssi);
	}

	else{
#ifdef DEBUG_LEVEL2
		DEBUGOUT("Invalid CRC 0x%04x instead 0x%04x\n",val,packetValue);
#endif
		brokenCounter++;
	}
	data->Processed = 1;
}


IfaceListenChannel_T startListen(){

	if(monitorMode){
		startRx(ifaceSettings->MonitorChannel, ifaceSettings->MonitorSeed);
		return ListenChannel_Monitor;
	}

	if((!waitingResponse && 	(neighborsCount==0 ||
			(currentTime-lastBeaconReceived> ifaceSettings->BeaconListenForce) ||
			(currentTime< ifaceSettings->BeaconListenStartup) ||
			(listenBeacon && (currentTime-listenBeaconStart < ifaceSettings->BeaconListenTimeout))))
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
		if(!listenBeacon)
			listenBeaconStart = currentTime;
		listenBeacon = true;
		startRx(ifaceSettings->BeaconChannel, ifaceSettings->BeaconSeed);
		return ListenChannel_Beacon;
	}else{
#ifdef DEBUG_LEVEL1
		DEBUGOUT("Listen for data 0x%02x\n",listeningChannel);
#endif
		startup = false;
		listenBeaconStart = INFINITY_TIME;
		listenBeacon = false;
		startRx(listeningChannel,listeningSeed);
		return ListenChannel_Data;
	}
}

uint16_t calcTimeout(uint16_t size)
{
	if(netSpeed>0)
		return ((((uint32_t)size+constantMessageOverhead)*1000) / netSpeed)*ifaceSettings->TxTimeoutCoef;
	else
		return ifaceSettings->TransmitTimeout;
}

void sendMessageFromQueue(){
	//prepare
	QueuedecRead(QName_routedown,&currentMessage);
	int8_t index = findNeighbor(currentMessage.Bridge);
	IfaceHeader_T* header = Iface_startHeader( currentMessage.Pack );
	//if no neighbor with same address
	if(index ==-1){
		//not found
		//drop if we try to response to unknown neighbor, push up otherwise
		if(currentMessage.State==PackState_prepared && header->Response)
			vPortFree(currentMessage.Pack);
		else{
			currentMessage.State = PackState_unknownDest;
			enqueueUpstreamMessage(currentMessage);
		}
		return;
	}
	resetInterfaceState();
	//set power
	int8_t power = setPower(ifaceSettings->DataTxPower, ifaceSettings->DataTxBoost);
	//set in packet
	if(currentMessage.State==PackState_unprepared){
		header->Beacon = 0;

		//header->Routing = 0;
		header->Response = 0;
		header->NeedResponse = ifaceSettings->DataNeedResponse;
		header->From = nodeAddress;
		header->To = currentMessage.Bridge;
		header->Size = currentMessage.Size;
		currentMessage.State = PackState_prepared;
	}
	header->TxPower = power;
	//crc calc
	currentMessageCrc = calcPacketCrc(header,true);
	header->CRC = currentMessageCrc;
	currentMessageCrcFull = calcPacketCrc(header,false);
#ifdef DEBUG_LEVEL1
	if(header->Response)
		DEBUGOUT("Sending response\n");
	else
		DEBUGOUT("Sending data\n");
#ifdef DEBUG_LEVEL3
	RouteHeader_T	* rh = Route_startHeader( currentMessage.Pack );
	DEBUGOUT( "Pack: %p\nFrom: 0x%08x\nTo: 0x%08x\nSize: %d\nType: ", currentMessage.Pack, rh->From, rh->To, rh->Size );
	if(header->Response)
		DEBUGOUT("Pack is response\n");
	else{
		switch( rh->Type ) {
		case RoutePackType_RouteProbe: DEBUGOUT( "RoutePackType_RouteProbe\n" ); break;
		case RoutePackType_RouteFinder: DEBUGOUT( "RoutePackType_RouteFinder\n" ); break;
		case RoutePackType_FinderACK: DEBUGOUT( "RoutePackType_FinderACK\n" ); break;
		case RoutePackType_ACK: DEBUGOUT( "RoutePackType_ACK\n" ); break;
		case RoutePackType_Data: DEBUGOUT( "RoutePackType_Data\n" ); break;
		default: DEBUGOUT( "unknown or wrong\n" );
		}
	}
#endif
#endif
	waitingResponse = header->NeedResponse;
	currentMessage.State = PackState_sending;

	currentMessageSize = currentMessage.Size+SzIfaceHeader;

	transmitResetTimeout = calcTimeout(currentMessageSize);

#ifdef DEBUG_LEVEL3
	DEBUGOUT("Setting timeout to %d\n",transmitResetTimeout);
#endif
	startTx(neighbors[index].Frequency,neighbors[index].Seed,currentMessage.Pack,currentMessageSize);
	enableReset = true;
	transmitStartTime = currentTime;
	if(header->Response){
		vPortFree(currentMessage.Pack);
		sendingResponse = true;
	}
	lastIsBeacon = false;
}

void sendBeacon(){
	//prepare
	//calculate power
	resetInterfaceState();
	//set power
	int8_t power = setPower(ifaceSettings->BeaconTxPower, ifaceSettings->BeaconTxBoost);
	//set in beacon
	IfaceHeader_T* beaconHeader = Iface_startHeader( beaconData );
	beaconHeader->TxPower = power;
	//set min sensetivity
	IfaceFooter_T* beaconFooter = Iface_startFooter( beaconData );
	beaconFooter->MinSensitivity = MAX(INT8_MIN,minSensetivity);
	//crc
	CRCvalue_T val = calcPacketCrc(beaconHeader,true);
	beaconHeader->CRC = val;
#ifdef DEBUG_LEVEL1
	DEBUGOUT("Sending beacon %d\n",sentBeaconCounter++);
#endif

	currentMessageSize = SzIfaceHeader+beaconHeader->Size+SzIfaceFooter;

	transmitResetTimeout = calcTimeout(currentMessageSize);

#ifdef DEBUG_LEVEL3
	DEBUGOUT("Setting timeout to %d\n",transmitResetTimeout);
#endif
	startTx(ifaceSettings->BeaconChannel, ifaceSettings->BeaconSeed, beaconData,currentMessageSize);
	enableReset = true;
	transmitStartTime = currentTime;
	lastBeaconSent = currentTime;
	lastIsBeacon = true;
}
void Interface_PreStart(){
	resetInterfaceState();
	startListen(0);
}
void Interface_MainTaskLoop(void *pvParameters){
//#ifdef DEBUG_LEVEL0
//	DEBUGOUT("Starting interface\n");
//#endif
	//start rx
	//resetInterfaceState();
	//startListen(0);
	//infinity loop

	//while(1){
		currentTime = xTaskGetTickCount();
		bool rts = readyToSend() && !waitingResponse && !monitorMode;

		if(monitorMode != monitorState)
		{
			monitorState = monitorMode;
			startListen();
		}
		//checking for transmit/receive timeout
		if((enableReset && (currentTime > (transmitStartTime+transmitResetTimeout))) ||
			(		// last reset was long time ago
				(currentTime - lastReset > ifaceSettings->ResetInterfaceTimeout) &&
					// data not received to long
				((currentTime - lastReceived > ifaceSettings->ResetInterfaceTimeout) ||
					// beacons not received
				(currentTime - lastBeaconReceived > ifaceSettings->ResetInterfaceTimeout))
			))
		{
			//in critical section
			vTaskSuspendAll();
			//oops, no response from iface
#ifdef DEBUG_LEVEL0
			DEBUGOUT("IFACE TIMEOUT - RESET LORA\n");
#endif
			Init_LORA(&(ifaceSettings->LORA_Settings));
			lastReset = currentTime;
			enableReset = false;
			netSpeed = 0;
			resetInterfaceState();
			xTaskResumeAll();
		}
		//waiting timeout
		if(waitingResponse && currentTime>waitingResponseResetTime){
			//no data was received
			//reset waiting
			waitingResponse = false;

			waitingResponseResetTime = INFINITY_TIME;
			//
			//change neighbor info
			updateLastTrys(currentMessage.Bridge, false);
			//if no trys left
			currentMessage.State = PackState_timeouted;
			enqueueUpstreamMessage(currentMessage);
		}
		if(!monitorMode &&
				!startup &&
				rts &&
				listenBeacon &&
				(currentTime - lastBeaconReceived <= ifaceSettings->BeaconListenForce) &&
				(currentTime - listenBeaconStart >  ifaceSettings->BeaconSendInterval + ifaceSettings->BeaconSendDeviation) &&
				(neighborsCount!=0)){
#ifdef DEBUG_LEVEL1
			DEBUGOUT("Reseting beacon listen, should switch to data listen\n");
#ifdef DEBUG_LEVEL3
			DEBUGOUT("Tx %d Rx %d Timeout %d\n",events.TxDone, events.RxDone,events.RxTimeout);
			if(interfaceState == InterfaceState_Busy)
				DEBUGOUT("Current interface state is InterfaceState_Busy\n");
			if(interfaceState == InterfaceState_Busy)
				DEBUGOUT("Current interface state is InterfaceState_Busy\n");
			if(interfaceState == InterfaceState_Off)
				DEBUGOUT("Current interface state is InterfaceState_Off\n");
			if(interfaceState == InterfaceState_On)
				DEBUGOUT("Current interface state is InterfaceState_On\n");
			if(interfaceState == InterfaceState_Receive)
				DEBUGOUT("Current interface state is InterfaceState_Receive\n");
			if(interfaceState == InterfaceState_Transmit)
				DEBUGOUT("Current interface state is InterfaceState_Transmit\n");
#endif
#endif
			resetInterfaceState();
			if(startListen() == ListenChannel_Beacon){
#ifdef DEBUG_LEVEL0
				DEBUGOUT("SWITCHING TO DATA CHANNEL FAILED\n");
#endif
			}
		}
		//handlers
		if(events.RxDone){

			processMessage(&recievedData);
			resetInterfaceState();

			startListen();
		}
		if(events.TxDone ){
			//calc time here
			if(netSpeed<=0){
				uint32_t transmissionTime = currentTime - transmitStartTime;
				uint32_t messagesize = (uint32_t)((currentMessageSize+constantMessageOverhead)*1000);
				netSpeed = (messagesize/transmissionTime);
#ifdef DEBUG_LEVEL1
				DEBUGOUT("Transmission speed %d bytes/sec\n",netSpeed);
#ifdef DEBUG_LEVEL3
				uint16_t speedDiff = (constantMessageOverhead*1000)/transmissionTime;
				DEBUGOUT("Net speed %d bytes/sec\n",netSpeed-speedDiff);

				DEBUGOUT("On air time %d ms\n", transmissionTime);
				DEBUGOUT("Size = %d + %d = %d bytes\n", currentMessageSize,
						constantMessageOverhead,
						currentMessageSize+constantMessageOverhead);
#endif
#endif
			}
			enableReset = false;
			currentMessage.State = PackState_sent;
			if(!lastIsBeacon && !waitingResponse && !sendingResponse){
				//add only if message correctly sent and not response needed
				enqueueUpstreamMessage(currentMessage);
			}
			if(waitingResponse){
				currentMessage.State = PackState_waiting;
				waitingResponseResetTime = currentTime+waitingResponseTimeout;
			}
			sendingResponse = false;
			resetInterfaceState();
			startListen();
		}
		if(!QueuedecEmpty(QName_routedown)){
			//peek message
			QueuedecPeek(QName_routedown,&beaconUpdate);
			//check
			if(beaconUpdate.State == PackState_beaconupdate){
				//read message
				QueuedecRead(QName_routedown,&beaconUpdate);
				//process
				makeBeacon(beaconUpdate.Size,beaconUpdate.Pack);
				beaconUpdate.State = PackState_beaconupdated;
				//push message up
				enqueueUpstreamMessage(beaconUpdate);
			}
		}
		//check beacon event
		if(rts && currentTime>lastBeaconSent+beaconSendInterval)
		{
			sendBeacon();
			if(currentTime> ifaceSettings->BeaconStartupDuration)
				beaconSendInterval = ifaceSettings->BeaconSendInterval + ((rand()%ifaceSettings->BeaconSendDeviation)-ifaceSettings->BeaconSendDeviation/2);
			//continue;
			return;
		}
		if(rts && !QueuedecEmpty(QName_routedown)){
			sendMessageFromQueue();
		}

		//taskYIELD();
	//}
}
