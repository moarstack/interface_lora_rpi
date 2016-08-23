/*
 * swConfig.h
 *
 *  Created on: 27 февр. 2016 г.
 *      Author: svalov
 */

#ifndef SWCONFIG_H_
#define SWCONFIG_H_


#define STATIC_RESERVATION 0x800
#if defined(CHIP_LPC1347) || defined(CHIP_LPC1343)
#define BASE_MEMORY_SIZE 0x2000
#endif
#ifdef CHIP_LPC175X_6X
#define BASE_MEMORY_SIZE 0x8000
#endif

#ifndef BASE_MEMORY_SIZE
//#error Unsupported MCU
#endif
#define HEAP_BASE_SIZE (BASE_MEMORY_SIZE-STATIC_RESERVATION)
#define HEAP_TOTAL_SIZE HEAP_BASE_SIZE
#define HEAP_BLOCK1
#define HEAP_BLOCK2

#ifdef CHIP_LPC1347

#ifdef HEAP_BLOCK1
#define HEAP_BLOCK1_LOCATION 0x20000000UL
#define HEAP_BLOCK1_SIZE 0x800
#undef HEAP_TOTAL_SIZE
#define HEAP_TOTAL_SIZE (HEAP_BASE_SIZE+HEAP_BLOCK1_SIZE)
#endif //HEAP_BLOCK1

#ifdef HEAP_BLOCK2 //usb memory
#define HEAP_BLOCK2_LOCATION 0x20004000UL
#define HEAP_BLOCK2_SIZE 0x800
#undef HEAP_TOTAL_SIZE
#define HEAP_TOTAL_SIZE (HEAP_BASE_SIZE+HEAP_BLOCK1_SIZE+HEAP_BLOCK2_SIZE)
#endif //HEAP_BLOCK1

#endif //1347

#ifdef CHIP_LPC1343

#undef HEAP_BLOCK1
#undef HEAP_BLOCK2

#endif //1343

#define EEPROM_SETTINGS_ADDRESS 0x0
#define MALLOC_ASSERT true
#ifdef CHIP_LPC175X_6X

#ifdef HEAP_BLOCK1
#define HEAP_BLOCK1_LOCATION 0x2007c000UL
#define HEAP_BLOCK1_SIZE 0x8000
#undef HEAP_TOTAL_SIZE
#define HEAP_TOTAL_SIZE (HEAP_BASE_SIZE+HEAP_BLOCK1_SIZE)
#endif //HEAP_BLOCK1

#undef HEAP_BLOCK2

#endif //CHIP_LPC175X_6X

#define WWDT_WINDOW 512
#define WWDT_TIME 10
#define WWDT_WARNING 15

#define USE_LEDTASK
//#define USE_IOTASK

#define USE_INIT_ADC
#define USE_INIT_INTERFACE
#define USE_INIT_ROUTING
#define USE_INIT_SERVICE
#define USE_INIT_INT

//#define USE_INTERFACE
//#define USE_FAKE_INTERFACE
#ifdef USE_INTERFACE
#define USE_INIT_INTERFACE
#undef USE_FAKE_INTERFACE
#define USE_INIT_INT
#endif

//#define USE_ROUTING
#ifndef USE_ROUTING
//#define FAKE_ROUTING_DOWNSTREAM // from service to iface
//#define FAKE_ROUTING_SERVICEONLY // loop message back to service
#endif

//#define USE_XTRALITE_SERVICE
#define USE_LITE_SERVICE
//#define USE_LITE_SERVICE_TASK

//#define USE_APP1
//#define USE_APP2
#define USE_IOAPP
#define USE_MGMT

//#define USE_WWDT

#define PING_PORT_ID 0
#define PING_ENDS		4
#define LED_FLASH_INTERVAL_ON 	1000
#define LED_FLASH_INTERVAL_OFF 	1000
#define WWDT_RESET_TIME			1000

#endif /* SWCONFIG_H_ */
