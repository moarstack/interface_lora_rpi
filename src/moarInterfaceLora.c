//
// Created by svalov, kryvashek on 05.07.16.
//

#include <funcResults.h>
#include <moarInterface.h>
#include <string.h>
#include <moarInterfaceLoraPrivate.h>
#include <moarInterfaceCommand.h>
#include <moarCommons.h>
#include <wiringPi.h>
#include "moarLayerEntryPoint.h"
#include <settings.h>
#include <loraInterface.h>


int initEpoll(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	// setup settings
	layer->EpollCount = EPOLL_EVENTS_COUNT;
	layer->EpollTimeout = EPOLL_TIMEOUT;
	//epoll init here
	memset(layer->EpollEvent,0,layer->EpollCount*sizeof(struct epoll_event));
	//init
	layer->EpollHandler = epoll_create(1);
	if(-1 == layer->EpollHandler) {
		return FUNC_RESULT_FAILED;
	}
	// add channel socket
	struct epoll_event epollEventChannel;
	epollEventChannel.events = EPOLL_CHANNEL_EVENTS;
	epollEventChannel.data.fd = layer->ChannelSocket;
	int channelRes = epoll_ctl(layer->EpollHandler, EPOLL_CTL_ADD, layer->ChannelSocket, &epollEventChannel);
	if(0 != channelRes)
		return FUNC_RESULT_FAILED;
	//return
	return FUNC_RESULT_SUCCESS;
}
int ifaceInit(LoraIfaceLayer_T* layer, void* arg){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == arg)
		return FUNC_RESULT_FAILED_ARGUMENT;

	MoarIfaceStartupParams_T* params = (MoarIfaceStartupParams_T*)arg;
	//setup socket to channel
	if(NULL == params->socketToChannel)
		return FUNC_RESULT_FAILED_ARGUMENT;
	// conect and add socket
	strncpy( layer->ChannelSocketPath, params->socketToChannel, SOCKET_FILEPATH_SIZE );
	// connect
	int result = SocketOpenFile( layer->ChannelSocketPath, false, &(layer->ChannelSocket));
	// socket should be setted
	if(FUNC_RESULT_SUCCESS != result)
		return result;
	//fill processing pointers
	layer->ChannelProcessingRules[0] = MakeProcessingRule(LayerCommandType_Send, processSendCommand);
	layer->ChannelProcessingRules[1] = MakeProcessingRule(LayerCommandType_RegisterInterfaceResult, processRegResultCommand);
	layer->ChannelProcessingRules[2] = MakeProcessingRule(LayerCommandType_UpdateBeaconPayload, processBeaconUpdateCommand);
	layer->ChannelProcessingRules[3] = MakeProcessingRule(LayerCommandType_None, NULL);
	return FUNC_RESULT_SUCCESS;
}

int registerInterface(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	//send register command
	IfaceRegisterMetadata_T metadata = {0};
	metadata.Length = IFACE_ADDR_SIZE;
	metadata.Value = layer->LocalAddress;

	LayerCommandStruct_T command = {0};
	command.Command = LayerCommandType_RegisterInterface;
	command.Data = NULL;
	command.DataSize = 0;
	command.MetaData = &metadata;
	command.MetaSize = sizeof(IfaceRegisterMetadata_T);

	int res = WriteCommand(layer->ChannelSocket, &command);

	return res;
}

void * MOAR_LAYER_ENTRY_POINT(void* arg){
	LoraIfaceLayer_T layer = {0};
	int initRes = ifaceInit(&layer, arg);
	if(FUNC_RESULT_SUCCESS != initRes){
		return NULL;
	}
	// load configuration
	// init settings
	Init_IfaceSettings(&(layer.Settings));
	// init hardware

#ifdef ENABLE_IO
	int wpiRes = wiringPiSetup();
	int wpiGpioRes = wiringPiSetupGpio();
#endif
	// init interrupts
	// init internal interface
	int ifaceRes = interfaceInit(&layer);

	// init epoll
	int epollInitRes = initEpoll(&layer);
	if(FUNC_RESULT_SUCCESS != epollInitRes)
		return NULL;
	//start registration here
	int regRes = registerInterface(&layer);
	if(FUNC_RESULT_SUCCESS != regRes)
		return NULL;
	// enable process
	layer.Running = true;
	while(layer.Running) {
		// in poll
		int epollRes = epoll_wait(layer.EpollHandler, layer.EpollEvent,
								  layer.EpollCount, layer.EpollTimeout);
		// in poll
		if(epollRes<0){
			//perror("Routing epoll_wait");
		}
		for(int i=0; i<epollRes;i++) {
			uint32_t event = layer.EpollEvent[i].events;
			int fd = layer.EpollEvent[i].data.fd;
			int processRes = FUNC_RESULT_FAILED;
			if(fd == layer.ChannelSocket){
				processRes = ProcessCommand(&layer, fd, event, EPOLL_CHANNEL_EVENTS, layer.ChannelProcessingRules);
			}
			else{
				// wtf? i don`t add another sockets
			}
			//error processing
			if(FUNC_RESULT_SUCCESS != processRes){
				// we have problems
				// return NULL;
			}
		}
		int stateProcess = stateProcessing(&layer);

	}
	return NULL;
}