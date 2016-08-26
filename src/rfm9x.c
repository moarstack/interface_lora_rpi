/*
 * rfm9x.c
 *
 *  Created on: 03 дек. 2015 г.
 *      Author: svalov
 */

#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <rfm9x.h>
#include <wiringPi.h>
#include "rfm9x.h"
#include "spi.h"
#include <hwConfig.h>
#include <printf.h>
#include <stdio.h>


int spiSpeed = 1000000;
int spiChannel = -1;
static bool resetInited = true;
int8_t reset_pin = -1;
int8_t reset_port = -1;
bool useReset = false;
//init
uint8_t RFM9X_Init(RFM9X_Settings_T* settings){
	//check args
	spiChannel = settings->spiChannel;
	spiSpeed = settings->spiSpeed;
	SPI_Init(settings->spiChannel, settings->spiSpeed);
//	if(settings->altChipSelect)
//		SPI_AltCsInit(settings->gpioPort,settings->csPort,settings->csPin);
	useReset = settings->useReset;
	reset_port = settings->resetPort;
	reset_pin  = settings->resetPin;

	return 0;
}
//deinit
uint8_t RFM9X_DeInit(){
	SPI_DeInit(spiChannel);
	spiChannel = -1;
	return 0;
}

uint8_t RFM9X_Reset(){
	if(useReset){
		if(reset_pin<0 && reset_port<0) {
			return RESULT_ERROR_NORESET;
		}
#ifdef ENABLE_IO
		if(resetInited){
			//init
			pinMode(reset_pin, OUTPUT);
			pullUpDnControl(reset_pin, PUD_UP);
		}
		//pull down
		digitalWrite(reset_pin,0);
		//delay 10ms
		delay(10);
		//push up
		digitalWrite(reset_pin,1);
		//delay for wakeup 50ms
		delay(50);
#endif
		resetInited = false;
		return RESULT_OK;
	}
	return RESULT_OK;
}

//reset to return from sleep mode
//read register
uint8_t RFM9X_ReadRegister(uint8_t reg, uint8_t *val){

	//build
	uint8_t tx[SINGLE_REG_BUFFER_SIZE] = {0};
	uint8_t rx[SINGLE_REG_BUFFER_SIZE] = {0};
	//set
	tx[0] = reg & READ_MASK;
	//write
	SPI_RW_Data(spiChannel,tx,rx,SINGLE_REG_BUFFER_SIZE);
	//get
	*val = rx[1];
	return 0;
}
//write register
uint8_t RFM9X_WriteRegister(uint8_t reg, uint8_t val){

	//build
	uint8_t tx[SINGLE_REG_BUFFER_SIZE] = {0};
	uint8_t rx[SINGLE_REG_BUFFER_SIZE] = {0};
	//set
	tx[0] = reg | WRITE_MASK;
	tx[1] = val;
	//write
	SPI_RW_Data(spiChannel,tx,rx,SINGLE_REG_BUFFER_SIZE);
	return 0;
}
//read variable
uint8_t RFM9X_ReadVariable(uint8_t reg, uint8_t mask, uint8_t *val){
	uint8_t value;
	//read
	uint8_t error = RFM9X_ReadRegister(reg,&value);
	//check
	if(error!=RESULT_OK)
		return error;
	//return
	*val = value & mask;
	return RESULT_OK;
}
//write variable
uint8_t RFM9X_WriteVariable(uint8_t reg, uint8_t mask, uint8_t val){
	uint8_t value;
	//read
	uint8_t error = RFM9X_ReadRegister(reg, &value);
	//check
	if(error!=RESULT_OK)
		return error;
	//set, clean bits and set new
	value = (value & ~mask)|(val & mask);
	//write
	error = RFM9X_WriteRegister(reg,value);
	return error;
}
uint8_t RFM9X_BurstRead(uint8_t baseReg, uint8_t *rx, uint8_t count){
	uint8_t tx[count];
	memset(tx,0x00,count);
	tx[0] = baseReg;
	SPI_RW_Data(spiChannel,tx,rx,count);
	return RESULT_OK;
}

uint8_t RFM9X_BurstWrite(uint8_t baseReg, uint8_t *txd, uint8_t count){
	uint8_t tx[count+1];
	uint8_t rx[count+1];
	memcpy( tx + 1, txd, count );
	memset(rx,0x00,count+1);
	tx[0] = baseReg | WRITE_MASK;
	SPI_RW_Data(spiChannel,tx,rx,count+1);
	return RESULT_OK;
}

uint8_t RFM9X_DumpRegisters(uint8_t *rx){
	return RFM9X_BurstRead(0x01,rx,FULL_REG_BUFFER_SIZE);
}

RFM9X_RegOpMode_T RFM9X_GetRegMode(){
	uint8_t val;
	RFM9X_ReadRegister(RFM9X_RegOpMode,&val);
	RFM9X_RegOpMode_T* mode = (RFM9X_RegOpMode_T*)((void*)&val);
	return *mode;
}
void RFM9X_SetRegMode(RFM9X_RegOpMode_T mode){
	uint8_t val = *((uint8_t*)((void*)&mode));
	RFM9X_WriteRegister(RFM9X_RegOpMode, val);
}


RFM9X_RegOpMode_Mode_T RFM9X_GetOpMode(){
//	uint8_t val;
//	RFM9X_ReadRegister(RFM9X_RegOpMode,&val);
//	RFM9X_RegOpMode_T* mode = (RFM9X_RegOpMode_T*)((void*)&val);
//	return mode->Mode;
	return RFM9X_GetRegMode().Mode;
}

void RFM9X_SetOpMode(RFM9X_RegOpMode_Mode_T mode){
	uint8_t val;
	RFM9X_ReadRegister(RFM9X_RegOpMode,&val);
	RFM9X_RegOpMode_T* m = (RFM9X_RegOpMode_T*)((void*)&val);
	m->Mode = mode;
	RFM9X_WriteRegister(RFM9X_RegOpMode,val);
}

RFM9X_RegOpMode_LongRangeMode_T RFM9X_GetLongRangeMode(){
	uint8_t val;
	RFM9X_ReadRegister(RFM9X_RegOpMode,&val);
	RFM9X_RegOpMode_T* mode = (RFM9X_RegOpMode_T*)((void*)&val);
	return mode->LongRangeMode;
}
void RFM9X_SetLongRangeMode(RFM9X_RegOpMode_LongRangeMode_T mode){
	uint8_t val;
	RFM9X_ReadRegister(RFM9X_RegOpMode,&val);
	RFM9X_RegOpMode_T* m = (RFM9X_RegOpMode_T*)((void*)&val);
	m->LongRangeMode = mode;
	RFM9X_WriteRegister(RFM9X_RegOpMode,val);
}
void RFM9X_SwitchLongRangeMode(RFM9X_RegOpMode_LongRangeMode_T mode)
{
	RFM9X_SetOpMode(Mode_SLEEP);
	RFM9X_SetLongRangeMode(mode);
	RFM9X_SetOpMode(Mode_STANDBY);
}

RFM9X_RegPaConfig_T RFM9X_GetPaConfig(){
	uint8_t val;
	RFM9X_ReadRegister(RFM9X_RegPaConfig,&val);
	RFM9X_RegPaConfig_T * conf = (RFM9X_RegPaConfig_T*)((void*)&val);
	return *conf;
}

void RFM9X_SetPaConfig(RFM9X_RegPaConfig_T conf){
	uint8_t val = *((uint8_t*)((void*)&conf));
	RFM9X_WriteRegister(RFM9X_RegPaConfig,val);
}

uint32_t RFM9X_GetFrequency(){
	uint8_t data[4];
	RFM9X_BurstRead(RFM9X_RegFrfMsb,data,4);
	uint32_t res = data[1] <<16 | data[2]<<8 | data[3];
	return res;
}
void RFM9X_SetFrequency(uint32_t freq){
	//	uint8_t data[3];
	//	data[2] = freq & 0xFF;
	//	data[1] = (freq>>8) & 0xFF;
	//	data[0] = (freq>>16) & 0xFF;
	//	RFM9X_BurstWrite(RFM9X_RegFrfMsb,data,3);
#ifdef DEBUG_LEVEL3
	DEBUGOUT("Setup Frequency 0x%06x\n",freq);
#endif
	RFM9X_WriteRegister(RFM9X_RegFrfMsb,(freq>>16) & 0xFF);
	RFM9X_WriteRegister(RFM9X_RegFrfMid,(freq>>8) & 0xFF);
	RFM9X_WriteRegister(RFM9X_RegFrfLsb,(freq) & 0xFF);
}







