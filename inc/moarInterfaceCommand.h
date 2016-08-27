//
// Created by svalov on 8/23/16.
//

#ifndef MOARSTACK_MOARINTERFACECOMMAND_H
#define MOARSTACK_MOARINTERFACECOMMAND_H

#include <moarCommons.h>

int processSendCommand(void* layerRef, int fd, LayerCommandStruct_T* command);
int processBeaconUpdateCommand(void* layerRef, int fd, LayerCommandStruct_T* command);
int processRegResultCommand(void* layerRef, int fd, LayerCommandStruct_T* command);
int processIfaceNeighbors(LoraIfaceLayer_T* layer, LayerCommandType_T type, IfaceAddr_T* addr, void* payload, PayloadSize_T size);


#endif //MOARSTACK_MOARINTERFACECOMMAND_H
