/*
 * intHandler_1343.c
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: svalov
 */
#include "../inc/intHandler.h"

//#define CHIP_LPC1343

#ifdef CHIP_LPC1343

//#define ENABLE_PORT0
#define ENABLE_PORT1
//#define ENABLE_PORT2
//#define ENABLE_PORT3

typedef struct {
	uint8_t Port;
	uint8_t Pin;
} INT_PORT_PIN_T;

static pinIrqHandler handler;
static CHIP_IOCON_PIO_T mapping_PIO0[] =	{IOCON_PIO0_0, IOCON_PIO0_1, IOCON_PIO0_2, IOCON_PIO0_3, IOCON_PIO0_4, IOCON_PIO0_5,
											IOCON_PIO0_6, IOCON_PIO0_7, IOCON_PIO0_8, IOCON_PIO0_9, IOCON_PIO0_11, IOCON_PIO0_11}; //workaround, no pio0_10
static CHIP_IOCON_PIO_T mapping_PIO1[] =	{IOCON_PIO1_0, IOCON_PIO1_1, IOCON_PIO1_2, IOCON_PIO1_3, IOCON_PIO1_4, IOCON_PIO1_5,
											IOCON_PIO1_6, IOCON_PIO1_7, IOCON_PIO1_8, IOCON_PIO1_9, IOCON_PIO1_10, IOCON_PIO1_11};
static CHIP_IOCON_PIO_T mapping_PIO2[] =	{IOCON_PIO2_0, IOCON_PIO2_1, IOCON_PIO2_2, IOCON_PIO2_3, IOCON_PIO2_4, IOCON_PIO2_5,
											IOCON_PIO2_6, IOCON_PIO2_7, IOCON_PIO2_8, IOCON_PIO2_9, IOCON_PIO2_10, IOCON_PIO2_11};
static CHIP_IOCON_PIO_T mapping_PIO3[] =	{IOCON_PIO3_0, IOCON_PIO3_1, IOCON_PIO3_2, IOCON_PIO3_3, IOCON_PIO3_4, IOCON_PIO3_5};


#define PININT_IRQ_HANDLER(n)  PIOINT##n##_IRQHandler
#define PININT_NVIC_NAME(n)    EINT##n##_IRQn
#define PINT_OFFSET			   PININT_NVIC_NAME(0)
#define GPIO_PININT_INDEX(n)   (n)

#define INT_HANDLER(n) void PININT_IRQ_HANDLER(n)(void){ \
		uint32_t pinMask = LPC_GPIO_PORT[n].MIS; \
		uint8_t i = 0; 	\
		for(i=0;i<32;i++){ 	\
			uint32_t mask = 1<<i;	\
			if(pinMask & mask){	\
				Chip_GPIO_ClearInts(LPC_GPIO_PORT, n, mask); 	\
				if(handler) handler(n, i);	\
			}	\
		}	\
}

#ifdef ENABLE_PORT0
INT_HANDLER(0)
#endif

#ifdef ENABLE_PORT1
INT_HANDLER(1)
#endif

#ifdef ENABLE_PORT2
INT_HANDLER(2)
#endif

#ifdef ENABLE_PORT3
INT_HANDLER(3)
#endif

CHIP_IOCON_PIO_T convertToIOCON(uint8_t port, uint8_t pin){
	if(port==0) return mapping_PIO0[pin];
	if(port==1) return mapping_PIO1[pin];
	if(port==2) return mapping_PIO2[pin];
	if(port==3) return mapping_PIO3[pin];
	return -1;
}

void Int_Init(pinIrqHandler pinHandler){

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);
	//handler here
	handler = pinHandler;
}
uint8_t Int_AddPinInt(uint8_t port, uint8_t pin, uint32_t modefunc, Level_T level, Mode_T mode){

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, port, pin);
	Chip_IOCON_PinMuxSet(LPC_IOCON, convertToIOCON(port, pin),
			(IOCON_FUNC0 | modefunc |	IOCON_RESERVED_BIT_6 | IOCON_RESERVED_BIT_7));

	if(mode == Mode_Level)
		Chip_GPIO_SetPinModeLevel(LPC_GPIO_PORT, port, (1 << pin));
	else if(mode == Mode_Edge)
		Chip_GPIO_SetPinModeEdge(LPC_GPIO_PORT, port, (1 << pin));

	if(level & Level_Low)
		Chip_GPIO_SetModeLow(LPC_GPIO_PORT, port, (1 << pin));
	if(level & Level_High)
		Chip_GPIO_SetModeHigh(LPC_GPIO_PORT, port, (1 << pin));

	Chip_GPIO_EnableInt(LPC_GPIO_PORT, port, (1 << pin));
	Chip_GPIO_ClearInts(LPC_GPIO_PORT, port, (1 << pin));

	uint32_t irq = EINT0_IRQn-port;
	if(!NVIC_GetActive(irq)){
		//NVIC_SetPriority(irq,0x00);
		NVIC_ClearPendingIRQ(irq);
		NVIC_EnableIRQ(irq);
	}
	return 0;

}

#endif // 1343


