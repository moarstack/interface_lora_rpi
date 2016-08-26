/*
 * interrupts.c
 *
 *  Created on: 03 февр. 2016 г.
 *      Author: svalov
 */
#include "interrupts.h"
#include <stdio.h>
void interuptHandler(uint8_t port, uint8_t pin){
	RFM9X_DIO_T dio = -1;
#ifdef DIO0_ENABLE
	if(port == DIO0_PORT && pin == DIO0_PIN) dio = RFM9X_DIO0;
#endif
#ifdef DIO1_ENABLE
	if(port == DIO1_PORT && pin == DIO1_PIN) dio = RFM9X_DIO1;
#endif
#ifdef DIO2_ENABLE
	if(port == DIO2_PORT && pin == DIO2_PIN) dio = RFM9X_DIO2;
#endif
#ifdef DIO3_ENABLE
	if(port == DIO3_PORT && pin == DIO3_PIN) dio = RFM9X_DIO3;
#endif
#ifdef DIO4_ENABLE
	if(port == DIO4_PORT && pin == DIO4_PIN) dio = RFM9X_DIO4;
#endif
#ifdef DIO5_ENABLE
	if(port == DIO5_PORT && pin == DIO5_PIN) dio = RFM9X_DIO5;
#endif
	printf("Interrupt %d\n",dio);
	INTERRUPT_HANDLER(dio);
}

void Init_interupts(){
#ifdef DEBUG_LEVEL0
	DEBUGOUT("Configuring Interupts - ");
#endif
	Int_Init(interuptHandler);
	//ifdef
#ifdef DIO0_ENABLE
	Int_AddPinInt(DIO0_PORT,DIO0_PIN,0,Level_High,Mode_Edge);
#endif
#ifdef DIO1_ENABLE
	Int_AddPinInt(DIO1_PORT,DIO1_PIN,0,Level_High,Mode_Edge);
#endif
#ifdef DIO2_ENABLE
	Int_AddPinInt(DIO2_PORT,DIO2_PIN,0,Level_High,Mode_Edge);
#endif
#ifdef DIO3_ENABLE
	Int_AddPinInt(DIO3_PORT,DIO3_PIN,0,Level_High,Mode_Edge);
#endif
#ifdef DIO4_ENABLE
	Int_AddPinInt(DIO4_PORT,DIO4_PIN,IOCON_MODE_INACT,Level_High,Mode_Edge);
#endif
#ifdef DIO5_ENABLE
	Int_AddPinInt(DIO5_PORT,DIO5_PIN,IOCON_MODE_INACT,Level_High,Mode_Edge);
#endif
#ifdef DEBUG_LEVEL0
	DEBUGOUT("OK \n");
#endif
}

