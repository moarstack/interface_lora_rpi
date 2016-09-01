//
// Created by svalov, kryvashek on 05.07.16.
//
#define _GNU_SOURCE
#include <funcResults.h>
#include <moarInterface.h>
#include <string.h>
#include <moarInterfaceLoraPrivate.h>
#include <moarInterfaceCommand.h>
#include "moarLayerEntryPoint.h"
#include <settings.h>
#include <loraInterface.h>
#include <signal.h>
#include <stdio.h>
#include <sys/signalfd.h>
#include <unistd.h>

int initEpoll(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	// setup settings
	LogWrite(layer->Log, LogLevel_DebugQuiet, "Epoll init");
	layer->EpollCount = EPOLL_EVENTS_COUNT;
	layer->EpollTimeout = EPOLL_TIMEOUT;
	//epoll init here
	memset(layer->EpollEvent,0,layer->EpollCount*sizeof(struct epoll_event));
	//init
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Creating epoll");
	layer->EpollHandler = epoll_create(1);
	if(-1 == layer->EpollHandler) {
		return FUNC_RESULT_FAILED;
	}
	// add channel socket
	struct epoll_event epollEventChannel;
	epollEventChannel.events = EPOLL_CHANNEL_EVENTS;
	epollEventChannel.data.fd = layer->ChannelSocket;
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Add channel layer handler");
	int channelRes = epoll_ctl(layer->EpollHandler, EPOLL_CTL_ADD, layer->ChannelSocket, &epollEventChannel);
	if(0 != channelRes)
		return FUNC_RESULT_FAILED;
	//signals
	struct epoll_event epollEventSignals;
	epollEventSignals.events = EPOLL_CHANNEL_EVENTS;
	epollEventSignals.data.fd = layer->SignalFd;
	if(-1 == epollEventSignals.data.fd)
		return FUNC_RESULT_FAILED;
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Add signal handler");
	int signalRes = epoll_ctl(layer->EpollHandler, EPOLL_CTL_ADD, epollEventSignals.data.fd , &epollEventSignals);
	if(0 != signalRes)
		return FUNC_RESULT_FAILED;
	//return
	return FUNC_RESULT_SUCCESS;
}
int ifaceInit(LoraIfaceLayer_T* layer, void* arg){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	if(NULL == arg)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LogWrite(layer->Log, LogLevel_DebugQuiet, "Interface init");
	MoarIfaceStartupParams_T* params = (MoarIfaceStartupParams_T*)arg;
	//setup socket to channel
	if(NULL == params->socketToChannel)
		return FUNC_RESULT_FAILED_ARGUMENT;
	// conect and add socket
	strncpy( layer->ChannelSocketPath, params->socketToChannel, SOCKET_FILEPATH_SIZE );
	// connect
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Open socket to channel");
	int result = SocketOpenFile( layer->ChannelSocketPath, false, &(layer->ChannelSocket));
	// socket should be setted
	if(FUNC_RESULT_SUCCESS != result)
		return result;
	//fill processing pointers
	layer->ChannelProcessingRules[0] = MakeProcessingRule(LayerCommandType_Send, processSendCommand);
	layer->ChannelProcessingRules[1] = MakeProcessingRule(LayerCommandType_RegisterInterfaceResult, processRegResultCommand);
	layer->ChannelProcessingRules[2] = MakeProcessingRule(LayerCommandType_UpdateBeaconPayload, processBeaconUpdateCommand);
	layer->ChannelProcessingRules[3] = MakeProcessingRule(LayerCommandType_None, NULL);

	//init signals
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Set up signal mask");
	sigemptyset (&(layer->SignalMask));
	sigaddset(&(layer->SignalMask),SIGUSR1);
	int procRes = sigprocmask(SIG_BLOCK, &(layer->SignalMask),NULL);
	if(0 != procRes)
		return FUNC_RESULT_FAILED;
	// init signal socket
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Creating file handler from signal");
	layer->SignalFd = signalfd(-1,&(layer->SignalMask), 0);
	if(-1 == layer->SignalFd)
		return FUNC_RESULT_FAILED;

	return FUNC_RESULT_SUCCESS;
}

int registerInterface(LoraIfaceLayer_T* layer){
	if(NULL == layer)
		return FUNC_RESULT_FAILED_ARGUMENT;
	LogWrite(layer->Log, LogLevel_DebugQuiet, "Interface registration");
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
	LogWrite(layer->Log, LogLevel_DebugVerbose, "Interface registration command sending");
	int res = WriteCommand(layer->ChannelSocket, &command);
	LogErrMoar(layer->Log,LogLevel_DebugVerbose, res, "Interface registration result");
	return res;
}

void * MOAR_LAYER_ENTRY_POINT(void* arg){
	LoraIfaceLayer_T layer = {0};
	int logOpen = LogOpen(LOG_FILE_PATH, &(layer.Log));
	LogSetLevelLog(layer.Log, LogLevel_Dump);
	LogWrite(layer.Log, LogLevel_Information, "LORA Interface layer started");
	int initRes = ifaceInit(&layer, arg);
	if(FUNC_RESULT_SUCCESS != initRes) {
		LogErrMoar(layer.Log,LogLevel_Critical, initRes, "Layer init failed");
		return NULL;
	}
	// load configuration
	// init settings
	LogWrite(layer.Log, LogLevel_Information, "Init interface in settings");
	Init_IfaceSettings(&(layer.Settings));
	// init internal interface
	int ifaceRes = interfaceInit(&layer);
	if(FUNC_RESULT_SUCCESS != ifaceRes) {
		LogErrMoar(layer.Log,LogLevel_Critical, ifaceRes, "Low level interface init failed");
		return NULL;
	}
	// init epoll
	int epollInitRes = initEpoll(&layer);
	if(FUNC_RESULT_SUCCESS != epollInitRes) {
		LogErrMoar(layer.Log,LogLevel_Critical, epollInitRes, "Epoll init failed");
		return NULL;
	}
	//start registration here
	int regRes = registerInterface(&layer);
	if(FUNC_RESULT_SUCCESS != regRes) {
		LogErrMoar(layer.Log,LogLevel_Critical, regRes, "Interface registration failed");
		return NULL;
	}
	// enable process
	layer.Running = true;
	//layer.Busy = true;
	while(layer.Running) {
		moarTime_T start = timeGetCurrent();
		if(!layer.Busy) {
			// in poll
			int epollRes = epoll_pwait(layer.EpollHandler, layer.EpollEvent,
									  layer.EpollCount, layer.EpollTimeout, &(layer.SignalMask));
//			int epollRes = ppoll(&(layer.EpollDesc), layer.EpollCount, &(timeout), &(layer.EpollMask));
			// in poll
			if (epollRes < 0) {
				//perror("Routing epoll_wait");
			}
			for(int i=0; i< epollRes; i++) {
				uint32_t event = layer.EpollEvent[i].events;
				int fd = layer.EpollEvent[i].data.fd;
				int processRes = FUNC_RESULT_FAILED;
				if (fd == layer.ChannelSocket) {
					LogWrite(layer.Log, LogLevel_DebugVerbose, "Processing command");
					processRes = ProcessCommand(&layer, fd, event, EPOLL_CHANNEL_EVENTS, layer.ChannelProcessingRules);
				}
				else if(fd == layer.SignalFd) { //signal here
					LogWrite(layer.Log, LogLevel_DebugVerbose, "Processing signal");
					struct signalfd_siginfo info;
					ssize_t bytes = read(fd, &info, sizeof(info));
					processRes = FUNC_RESULT_SUCCESS;
				} else{
					// wtf? i don`t add another sockets
					LogWrite(layer.Log, LogLevel_Warning, "Unknown file handler event");
				}
				//error processing
				if (FUNC_RESULT_SUCCESS != processRes) {
					// we have problems
					LogErrMoar(layer.Log, LogLevel_Warning, processRes, "Command processing problem");
					// return NULL;
				}
			}
		}
		else{
			// wait timeout or signal here
			struct timespec timeout;
			timeout.tv_sec = layer.EpollTimeout/1000;
			timeout.tv_nsec = (layer.EpollTimeout - timeout.tv_sec*1000)*1000000;
			int res = sigtimedwait(&(layer.SignalMask), NULL, &(timeout));
		}
		printf("time %d\n",timeGetCurrent()-start);
		int stateProcess = interfaceStateProcessing(&layer);

	}
	return NULL;
}