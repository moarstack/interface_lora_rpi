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