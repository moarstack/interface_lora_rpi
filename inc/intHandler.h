/*
 * intHandler.h
 *
 *  Created on: 08 янв. 2016 г.
 *      Author: svalov
 */

#ifndef INC_INTHANDLER_H_
#define INC_INTHANDLER_H_

#include "board.h"


typedef enum{
	Level_None=0,
	Level_Low=1,
	Level_High=2,
} Level_T;

typedef enum {
	Mode_None=0,
	Mode_Edge=1,
	Mode_Level=2,
} Mode_T;
typedef void (*pinIrqHandler)(uint8_t, uint8_t);
void Int_Init(pinIrqHandler pinHandler);
uint8_t Int_AddPinInt(uint8_t port, uint8_t pin, uint32_t modefunc, Level_T level, Mode_T mode);

#endif /* INC_INTHANDLER_H_ */
