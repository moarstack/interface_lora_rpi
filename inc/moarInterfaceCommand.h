//
// Created by svalov on 8/23/16.
//

#ifndef MOARSTACK_MOARINTERFACECOMMAND_H
#define MOARSTACK_MOARINTERFACECOMMAND_H

#include <moarCommons.h>

int processSendCommand(void* layerRef, int fd, LayerCommandStruct_T* command);
int processBeaconUpdateCommand(void* layerRef, int fd, LayerCommandStruct_T* command);
int processRegResultCommand(void* layerRef, int fd, LayerCommandStruct_T* command);


#endif //MOARSTACK_MOARINTERFACECOMMAND_H
