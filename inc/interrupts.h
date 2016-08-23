/*
 * interrupts.h
 *
 *  Created on: 03 февр. 2016 г.
 *      Author: svalov
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include "intHandler.h"
#include "lora.h"
#include "hwConfig.h"

#define INTERRUPT_HANDLER LORA_DIOHandler


void Init_interupts();

#endif /* INTERRUPTS_H_ */

