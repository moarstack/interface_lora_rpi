//
// Created by svalov on 8/26/16.
//

#include "interfaceNeighbors.h"
#include <hashFunc.h>
#include <hashTable.h>
#include <funcResults.h>
#include <interfaceNeighbors.h>
#include <moarInterfaceLoraPrivate.h>
#include <moarCommons.h>
#include <moarInterfaceCommand.h>

#define HASH_CONST	0xf2e143

uint32_t addressHash(void* addr, size_t size){
	return hashBytesEx(addr, size, HASH_CONST);
}

int neighborsInit(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LogWrite(layer->Log, LogLevel_DebugQuiet, "Init neighbors");
	int res = hashInit(&(layer->Neighbors),addressHash, NEIGHBORS_TABLE_SIZE , sizeof(IfaceAddr_T), sizeof(NeighborInfo_T));
	return res;
}
int neighborsAdd(LoraIfaceLayer_T* layer, NeighborInfo_T* neighbor, void* payload, PayloadSize_T size){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == neighbor)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LogWrite(layer->Log, LogLevel_Information, "Add new neighbor %b", &(neighbor->Address), sizeof(IfaceAddr_T));
	int res = hashAdd(&(layer->Neighbors),&(neighbor->Address), neighbor);
	int notifyRes = processIfaceNeighbors(layer, LayerCommandType_NewNeighbor, &(neighbor->Address), payload, size);
	return res;
}
int neighborsUpdate(LoraIfaceLayer_T* layer, NeighborInfo_T* neighbor, void* payload, PayloadSize_T size){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == neighbor)
		return FUNC_RESULT_FAILED_ARGUMENT;

	NeighborInfo_T* stored = hashGetPtr(&(layer->Neighbors),&(neighbor->Address));
	if(NULL == stored){
		int res = neighborsAdd(layer,neighbor, payload, size);
		return res;
	}
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Update neighbor %b", &(neighbor->Address), sizeof(IfaceAddr_T));
	*stored = *neighbor;
	int notifyRes = processIfaceNeighbors(layer, LayerCommandType_UpdateNeighbor, &(neighbor->Address), payload, size);
	return FUNC_RESULT_SUCCESS;
}
int neighborsGet(LoraIfaceLayer_T* layer, IfaceAddr_T* addr, NeighborInfo_T* neighbor){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == addr)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == neighbor)
		return FUNC_RESULT_FAILED_ARGUMENT;

	int res = hashGet(&(layer->Neighbors), addr,neighbor);
	return res;
}
int neighborsRemove(LoraIfaceLayer_T* layer, IfaceAddr_T* addr){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == addr)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LogWrite(layer->Log, LogLevel_Information, "Remove neighbor %b", &(addr), sizeof(IfaceAddr_T));
	int res = hashRemove(&(layer->Neighbors), addr);
	int notifyRes = processIfaceNeighbors(layer, LayerCommandType_LostNeighbor, addr, NULL, 0);
	return res;
}
int neighborsUpdateSendtrys(LoraIfaceLayer_T* layer, IfaceAddr_T* addr, bool responded){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == addr)
		return FUNC_RESULT_FAILED_ARGUMENT;
	NeighborInfo_T* stored = hashGetPtr(&(layer->Neighbors),addr);
	if(NULL != stored){
		if(responded) {
			stored->LastFailedTrys = 0;
			stored->LastSeen = timeGetCurrent();
			return FUNC_RESULT_SUCCESS;
		}
		else if(stored->LastFailedTrys>layer->Settings.MaxNeighborSendTrys) {
			int res = neighborsRemove(layer, addr);
			return res;
		}
	}
	return FUNC_RESULT_FAILED;
}

int neighborsUpdateLastSeen(LoraIfaceLayer_T* layer, IfaceAddr_T* addr, int16_t loss){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == addr)
		return FUNC_RESULT_FAILED_ARGUMENT;
	NeighborInfo_T* stored = hashGetPtr(&(layer->Neighbors),addr);
	if(NULL != stored){
		stored->LastSeen = timeGetCurrent();
		stored->SignalLoss = loss;
		return FUNC_RESULT_SUCCESS;
	}
	return FUNC_RESULT_FAILED;
}