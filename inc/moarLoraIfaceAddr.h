//
// Created by svalov on 12/28/16.
//

#ifndef MOARSTACK_MOARLORAIFACEADDR_H
#define MOARSTACK_MOARLORAIFACEADDR_H

#include <stdint.h>

#define IFACE_ADDR_BASE_TYPE				uint32_t
#define IFACE_ADDR_SIZE						sizeof( IFACE_ADDR_BASE_TYPE )

typedef struct {
	uint8_t	Address[ IFACE_ADDR_SIZE ];
} IfaceAddr_T;


#endif //MOARSTACK_MOARLORAIFACEADDR_H
