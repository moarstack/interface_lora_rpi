//
// Created by svalov on 8/25/16.
//

#ifndef MOARSTACK_LORAINTERFACE_H
#define MOARSTACK_LORAINTERFACE_H

#include <moarInterfaceLoraPrivate.h>

int interfaceInit(LoraIfaceLayer_T* layer);
int interfaceMakeBeacon(LoraIfaceLayer_T* layer, void* payload, PayloadSize_T size);
int interfaceStateProcessing(LoraIfaceLayer_T* layer);

#endif //MOARSTACK_LORAINTERFACE_H
