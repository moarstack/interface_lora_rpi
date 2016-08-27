/*
 * interface.h
 *
 *  Created on: 16 янв. 2016 г.
 *      Author: kryvashek
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "packet.h"
#include "lora.h"
#include <moarTime.h>
#include <moarInterfaceLoraPrivate.h>
#include <moarIfaceStructs.h>
#include <moarILoraSettings.h>



#define INFINITY_TIME					UINT64_MAX
// maximum size of usual iface packet payload
#define SzIfaceMaxPacket		256

// size of iface packet header
#define SzIfaceHeader			sizeof(IfaceHeader_T)

// size of iface packet footer
#define SzIfaceFooter			sizeof(IfaceFooter_T)

// maximum size of usual iface packet payload
#define SzIfaceMaxPayloadUsual	(SzIfaceMaxPacket-SzIfaceHeader)

// maximum size of iface beacon packet payload
#define SzIfaceMaxPayloadBeacon	(SzIfaceMaxPayloadUsual-SzIfaceFooter)

// place where iface packet header starts
#define IfaceHeaderStart		(0)

// place where iface packet payload starts
#define IfacePayloadStart		(SzIfaceHeader)

typedef struct{

} IfaceState_T;

//max send trys

//cross layer data struct
//pointer
//status

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

#pragma pack(pop)
//states
typedef enum{
	TransmissionState_Monitor,
	TransmissionState_Listening,
	TransmissionState_Receiving,
	TransmissionState_Transmiting,
	TransmissionState_WaitingResponse,
}TransmissionState_T;

extern uint8_t neighborsCount;
extern NeighborInfo_T* neighbors;
extern bool monitorMode;
extern uint8_t monitorChannel;
extern uint16_t monitorSeed;
void Interface_PreStart();
void Interface_MainTaskLoop(void *pvParameters);
void Init_Interface(IfaceAddr_T address, IfaceSettings_T* settings);


#endif /* INTERFACE_H_ */
