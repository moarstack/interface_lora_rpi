/*
 * rfm9x.h
 *
 *  Created on: 03 дек. 2015 г.
 *      Author: svalov
 */

#ifndef INC_RFM9X_H_
#define INC_RFM9X_H_

#include "stdint.h"
#include "rfm9x_reg.h"
#include <stdbool.h>

#define SINGLE_REG_BUFFER_SIZE 	2
#define FULL_REG_BUFFER_SIZE 	0x65
#define READ_MASK 				0x7F
#define WRITE_MASK 				0x80
#define RFM9X_FIFO_SIZE			0x100
//results
#define RESULT_OK				0
#define RESULT_ERROR_REINIT		1
#define RESULT_ERROR_NOTINIT	2
#define RESULT_ERROR_NORESET	3
#define DELAY_FUNC				delay_ms

typedef struct{
	void* sspPort;
	void* gpioPort;
	bool useReset;
	int8_t resetPort;
	int8_t resetPin;
	bool altChipSelect;
	int8_t csPort;
	int8_t csPin;
} RFM9X_Settings_T;

uint8_t RFM9X_Init(RFM9X_Settings_T* settings);
uint8_t RFM9X_DeInit();
uint8_t RFM9X_Reset();
uint8_t RFM9X_ReadRegister(uint8_t reg, uint8_t *val);
uint8_t RFM9X_WriteRegister(uint8_t reg, uint8_t val);
uint8_t RFM9X_ReadVariable(uint8_t reg, uint8_t mask, uint8_t *val);
uint8_t RFM9X_WriteVariable(uint8_t reg, uint8_t mask, uint8_t val);
uint8_t RFM9X_BurstRead(uint8_t baseReg, uint8_t *rx, uint8_t count);
uint8_t RFM9X_DumpRegisters(uint8_t *rx);

RFM9X_RegOpMode_T RFM9X_GetMode();
void RFM9X_SetMode(RFM9X_RegOpMode_T mode);

RFM9X_RegOpMode_Mode_T RFM9X_GetOpMode();
void RFM9X_SetOpMode(RFM9X_RegOpMode_Mode_T mode);

RFM9X_RegOpMode_LongRangeMode_T RFM9X_GetLongRangeMode();
void RFM9X_SetLongRangeMode(RFM9X_RegOpMode_LongRangeMode_T mode);

void RFM9X_SwitchLongRangeMode(RFM9X_RegOpMode_LongRangeMode_T mode);

RFM9X_RegPaConfig_T RFM9X_GetPaConfig();
void RFM9X_SetPaConfig(RFM9X_RegPaConfig_T conf);

uint32_t RFM9X_GetFrequency();
void RFM9X_SetFrequency(uint32_t freq);


#endif /* INC_RFM9X_H_ */
