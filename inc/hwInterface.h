/*
 * hwInterface.h
 *
 *  Created on: 03 февр. 2016 г.
 *      Author: svalov
 */

#ifndef HWINTERFACE_H_
#define HWINTERFACE_H_

#include "stdint.h"
#include "board.h"
#include "lora.h"

typedef struct {
	uint8_t RxDone:1;
	uint8_t TxDone:1;
	uint8_t RxTimeout:1;
} InterfaceEvents_T;

typedef enum{
	InterfaceState_On,
	InterfaceState_Receive,
	InterfaceState_Busy,
	InterfaceState_Transmit,
	InterfaceState_Off,
} InterfaceState_T;

typedef struct{
	uint8_t* Pointer;
	uint16_t Size;
	int16_t Rssi;
	bool Processed;
} RxData_T;
//variables
extern int16_t minSensetivity;
extern InterfaceEvents_T events;
extern InterfaceState_T interfaceState;
extern RxData_T recievedData;
extern uint8_t channelsCount;
extern uint16_t constantMessageOverhead;
//handlers

//void RxDoneHandler(uint8_t *data, uint16_t size, int16_t rssi, LORA_RxMode_T mode);
//void TxDoneHandler();
//void RxTimeoutHandler();
//void CadDoneHandler();
//void CadDetectedHandler();
int8_t setPower(int8_t value, bool boost);
bool Init_LORA(LORA_Settings_T* settings);
void resetInterfaceState();
//start rx
uint8_t startRx(uint8_t channel, uint16_t seed);
//send message
uint8_t startTx(uint8_t channel, uint16_t seed, uint8_t* data, uint16_t size);
//message avail
bool dataAvailable();
bool readyToSend();

#endif /* HWINTERFACE_H_ */
