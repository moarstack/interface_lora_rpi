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
	if(NULL == command->MetaData)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LoraIfaceLayer_T* layer = (LoraIfaceLayer_T*)layerRef;
	LogWrite(layer->Log, LogLevel_DebugQuiet, "Processing send command");
	ChannelSendMetadata_T* metadata = (ChannelSendMetadata_T*)command->MetaData;
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Message to %b", &(metadata->To), sizeof(IfaceAddr_T));
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Message ID %b", &(metadata->Id), sizeof(MessageId_T));
	// check size
	IfacePackState_T state = IfacePackState_Notsent;
	if(NULL != command->Data && 0 != command->DataSize && !layer->Busy) {
		LogWrite(layer->Log, LogLevel_Dump, "Message content %b", command->Data, command->DataSize);
		int sendRes = sendData(layer, &(metadata->To), metadata->NeedResponse, false, command->Data, command->DataSize);
		LogErrMoar(layer->Log, LogLevel_DebugVerbose, sendRes, "Send result");
		if (FUNC_RESULT_SUCCESS == sendRes) {
			//set mid
			layer->CurrentMid = metadata->Id;
			// no notify here, notification from tx done handler
			return FUNC_RESULT_SUCCESS;
		}
		if(FUNC_RESULT_FAILED_NEIGHBORS == sendRes) {
			state = IfacePackState_UnknownDest;
			LogWrite(layer->Log, LogLevel_Warning, "Neighbor %b not found", &(metadata->To), sizeof(IfaceAddr_T) );
		}
	}
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Notify send failed");
	int notifyRes = processIfaceMsgState(layer, &(metadata->Id), state);
	return notifyRes;
}

int processBeaconUpdateCommand(void* layerRef, int fd, LayerCommandStruct_T* command){
	if(NULL == layerRef)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(fd <= 0)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == command)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LoraIfaceLayer_T* layer = (LoraIfaceLayer_T*)layerRef;
	LogWrite(layer->Log, LogLevel_DebugQuiet, "Processing beacon update command");
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
	LogWrite(layer->Log, LogLevel_DebugQuiet, "Processing reg result command");
	ChannelRegisterResultMetadata_T* regResult = (ChannelRegisterResultMetadata_T*)command->MetaData;
	layer->Registred = regResult->Registred;
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Registration result %d", layer->Registred);
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
	LogWrite(layer->Log, LogLevel_DebugQuiet, "Processing interface received command");
	IfaceReceiveMetadata_T metadata = {0};
	metadata.From = *address;
	midGenerate(&(metadata.Id), MoarLayer_Interface);
	LayerCommandStruct_T command = {0};
	command.Command = LayerCommandType_Receive;
	command.MetaSize = sizeof(IfaceReceiveMetadata_T);
	command.MetaData = &metadata;
	command.Data = payload;
	command.DataSize = size;
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Send received command to channel");
	int res = WriteCommand(layer->ChannelSocket,&command);
	return res;
}

int processIfaceMsgState(LoraIfaceLayer_T* layer, MessageId_T* mid, IfacePackState_T state){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == mid)
		return FUNC_RESULT_FAILED_ARGUMENT;

	LogWrite(layer->Log, LogLevel_DebugQuiet, "Processing interface message state command");

	IfacePackStateMetadata_T metadata = {0};
	metadata.Id = *mid;
	metadata.State = state;
	LayerCommandStruct_T command = {0};
	command.Command = LayerCommandType_MessageState;
	command.MetaSize = sizeof(IfacePackStateMetadata_T);
	command.MetaData = &metadata;
	command.Data = NULL;
	command.DataSize = 0;
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Send message %b state %d",mid, sizeof(MessageId_T), state);
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

	LogWrite(layer->Log, LogLevel_DebugVerbose, "Processing interface neighbors command");

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
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Send neighbor %b event to channel",addr, sizeof(IfaceAddr_T));
	int res = WriteCommand(layer->ChannelSocket, &command);
	return res;
}