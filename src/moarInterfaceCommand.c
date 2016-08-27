//
// Created by svalov on 8/23/16.
//

#include <funcResults.h>
#include <moarInterfaceLoraPrivate.h>
#include <moarInterfaceCommand.h>
#include <moarCommons.h>
#include <loraInterface.h>
#include <moarInterfaceChannel.h>

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
int processIfaceReceived(LoraIfaceLayer_T* layer, IfaceAddr_T* address, void* payload, PayloadSize_T size){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == address)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == payload)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(0 == size)
		return FUNC_RESULT_FAILED_ARGUMENT;

	IfaceReceiveMetadata_T metadata = {0};
	metadata.From = *address;
	midGenerate(&(metadata.Id), MoarLayer_Interface);
	LayerCommandStruct_T command = {0};
	command.Command = LayerCommandType_Receive;
	command.MetaSize = sizeof(IfaceReceiveMetadata_T);
	command.MetaData = &metadata;
	command.Data = payload;
	command.DataSize = size;
	int res = WriteCommand(layer->ChannelSocket,&command);
	return res;
}

int processIfaceMsgState(LoraIfaceLayer_T* layer, MessageId_T* mid, IfacePackState_T state){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == mid)
		return FUNC_RESULT_FAILED_ARGUMENT;
	IfacePackStateMetadata_T metadata = {0};
	metadata.Id = *mid;
	metadata.State = state;
	LayerCommandStruct_T command = {0};
	command.Command = LayerCommandType_MessageState;
	command.MetaSize = sizeof(IfacePackStateMetadata_T);
	command.MetaData = &metadata;
	command.Data = NULL;
	command.DataSize = 0;
	int res = WriteCommand(layer->ChannelSocket,&command);
	return res;
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