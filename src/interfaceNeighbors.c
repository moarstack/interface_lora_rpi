//
// Created by svalov on 8/26/16.
//

#include "interfaceNeighbors.h"
#include <hashFunc.h>
#include <hashTable.h>
#include <funcResults.h>
#include <interfaceNeighbors.h>

#define HASH_CONST	0xf2e143

uint32_t addressHash(void* addr, size_t size){
	return hashBytesEx(addr, size, HASH_CONST);
}

int notifyChannel(LoraIfaceLayer_T* layer, LayerCommandType_T type, IfaceAddr_T* addr, void* payload, PayloadSize_T size){

}

int neighborsInit(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	hashInit(&(layer->Neighbors),addressHash,NEIGHBORS_TABLE_SIZE , sizeof(IfaceAddr_T), sizeof(NeighborInfo_T));
	return FUNC_RESULT_SUCCESS;
}
int neighborsAdd(LoraIfaceLayer_T* layer, NeighborInfo_T* neighbor, void* payload, PayloadSize_T size){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == neighbor)
		return FUNC_RESULT_FAILED_ARGUMENT;
	int notifyRes = notifyChannel(layer, LayerCommandType_NewNeighbor, &(neighbor->Address), payload, size);
	int res = hashAdd(&(layer->Neighbors),&(neighbor->Address), neighbor);
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
	*stored = *neighbor;
	int notifyRes = notifyChannel(layer, LayerCommandType_UpdateNeighbor, &(neighbor->Address), payload, size);
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
	int notifyRes = notifyChannel(layer, LayerCommandType_LostNeighbor, addr, NULL, 0);
	int res = hashRemove(&(layer->Neighbors), addr);
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