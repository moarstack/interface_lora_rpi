/*
 * intHandler_1343.c
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: svalov
 */
#include "intHandler.h"
#include <hwConfig.h>
#include <funcResults.h>
#include <wiringPi.h>
#define INT_COUNT 6

//#define ENABLE_PORT0
#define ENABLE_PORT1
//#define ENABLE_PORT2
//#define ENABLE_PORT3
typedef void(*intFunc)(void);

typedef struct {
	uint8_t Port;
	uint8_t Pin;
	intFunc Function;
} INT_PORT_PIN_T;


static pinIrqHandler handler;
static int handlersCount;
static INT_PORT_PIN_T handlerPins[INT_COUNT];

#define INT_HANDLER(n) void INT##n##_HANDLER(void){ \
		int index =  ##n## ; \
        if(handler)     handler(handlerPins[ index ].Port, handlerPins[ index ].Pin);\
}

#if INT_COUNT>0
void INT0_HANDLER(void){
	int index = 0;
	int state = digitalRead(handlerPins[index].Pin);
       if(handler && state == HIGH)
		   handler(handlerPins[index].Port, handlerPins[index].Pin);
}
#if INT_COUNT>1
void INT1_HANDLER(void){
	int index = 1;
	int state = digitalRead(handlerPins[index].Pin);
	if(handler && state == HIGH)
		   handler(handlerPins[index].Port, handlerPins[index].Pin);
}
#if INT_COUNT>2
void INT2_HANDLER(void){
	int index = 2;
	int state = digitalRead(handlerPins[index].Pin);
	if(handler && state == HIGH)
		   handler(handlerPins[index].Port, handlerPins[index].Pin);
}
#if INT_COUNT>3
void INT3_HANDLER(void){
	int index = 3;
	int state = digitalRead(handlerPins[index].Pin);
	if(handler && state == HIGH)
		   handler(handlerPins[index].Port, handlerPins[index].Pin);
}
#if INT_COUNT>4
void INT4_HANDLER(void){
	int index = 4;
	int state = digitalRead(handlerPins[index].Pin);
	if(handler && state == HIGH)
		   handler(handlerPins[index].Port, handlerPins[index].Pin);
}
#if INT_COUNT>5
void INT5_HANDLER(void){
	int index = 5;
	int state = digitalRead(handlerPins[index].Pin);
	if(handler && state == HIGH)
		   handler(handlerPins[index].Port, handlerPins[index].Pin);
}
#endif //5
#endif //4
#endif //3
#endif //2
#endif //1
#endif //0

void Int_Init(pinIrqHandler pinHandler){
	//handler here
	handler = pinHandler;
	handlersCount = 0;
#if INT_COUNT>0
	handlerPins[0].Function = INT0_HANDLER;
#if INT_COUNT>1
	handlerPins[1].Function = INT1_HANDLER;
#if INT_COUNT>2
	handlerPins[2].Function = INT2_HANDLER;
#if INT_COUNT>3
	handlerPins[3].Function = INT3_HANDLER;
#if INT_COUNT>4
	handlerPins[4].Function = INT4_HANDLER;
#if INT_COUNT>5
	handlerPins[5].Function = INT5_HANDLER;
#endif //5
#endif //4
#endif //3
#endif //2
#endif //1
#endif //0
}
uint8_t Int_AddPinInt(uint8_t port, uint8_t pin, uint32_t modefunc, Level_T level, Mode_T mode){
	if(handlersCount>=INT_COUNT)
		return FUNC_RESULT_FAILED;
	handlerPins[handlersCount].Pin = pin;
	handlerPins[handlersCount].Port = port;
#ifdef ENABLE_IO
	pinMode(pin, INPUT);
	pullUpDnControl(pin,PUD_DOWN);
	if(level & Level_Low)
		wiringPiISR(pin, INT_EDGE_FALLING, handlerPins[handlersCount].Function);
	if(level & Level_High)
		wiringPiISR(pin, INT_EDGE_RISING, handlerPins[handlersCount].Function);
#endif
	handlersCount++;
	return FUNC_RESULT_SUCCESS;
}

