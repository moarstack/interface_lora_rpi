//
// Created by svalov on 8/23/16.
//

#include <funcResults.h>
#include <moarInterfaceLoraPrivate.h>
#include <moarInterfaceCommand.h>
#include <moarCommons.h>
#include <loraInterface.h>

int processSendCommand(void* layerRef, int fd, LayerCommandStruct_T* command){
	if(NULL == layerRef)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(fd <= 0)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == command)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LoraIfaceLayer_T* layer = (LoraIfaceLayer_T*)layerRef;
	//logic here
	return FUNC_RESULT_SUCCESS;
}

int processBeaconUpdateCommand(void* layerRef, int fd, LayerCommandStruct_T* command){
	if(NULL == layerRef)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(fd <= 0)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == command)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LoraIfaceLayer_T* layer = (LoraIfaceLayer_T*)layerRef;
	int res = interfaceMakeBeacon(layer, command->Data, command->DataSize);
	return res;
}

int processRegResultCommand(void* layerRef, int fd, LayerCommandStruct_T* command){
	if(NULL == layerRef)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(fd <= 0)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == command)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LoraIfaceLayer_T* layer = (LoraIfaceLayer_T*)layerRef;
	ChannelRegisterResultMetadata_T* regResult = (ChannelRegisterResultMetadata_T*)command->MetaData;
	layer->Registred = regResult;
	return FUNC_RESULT_SUCCESS;
}

int processIfaceNeighbors(LoraIfaceLayer_T* layer, LayerCommandType_T type, IfaceAddr_T* addr, void* payload, PayloadSize_T size){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(LayerCommandType_LostNeighbor != type &&
	   LayerCommandType_NewNeighbor != type &&
	   LayerCommandType_UpdateNeighbor != type)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == addr)
		return FUNC_RESULT_FAILED_ARGUMENT;

	if(NULL == payload | 0 == size){
		payload = NULL;
		size = 0;
	}
	IfaceNeighborMetadata_T metadata = {0};
	metadata.Neighbor = *addr;
	LayerCommandStruct_T command = {0};
	command.Command = type;
	command.Data = payload;
	command.DataSize = size;
	command.MetaData = &metadata;
	command.MetaSize = sizeof(IfaceNeighborMetadata_T);
	int res = WriteCommand(layer->ChannelSocket, &command);
	return res;
}