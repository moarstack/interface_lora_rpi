//
// Created by svalov on 8/25/16.
//

#include <funcResults.h>
#include <moarInterfaceLoraPrivate.h>
#include <moarILoraSettings.h>
#include <hwInterface.h>
#include "loraInterface.h"

int interfaceInit(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	bool res = Init_LORA(&(layer->Settings.LORA_Settings));
	if(!res)
		return FUNC_RESULT_FAILED;
	// init neighbors here
	// create beacon packet here
	// reset interface state here
	return FUNC_RESULT_SUCCESS;
}

// process message function here


int stateProcessing(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;

	return FUNC_RESULT_SUCCESS;
}
