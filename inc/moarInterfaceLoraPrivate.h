//
// Created by kryvashek on 09.07.16.
// for private notations

#ifndef MOARSTACK_MOARINTERFACEPRIVATE_H
#define MOARSTACK_MOARINTERFACEPRIVATE_H

#define _GNU_SOURCE
#include <stdint.h>
#include <stdbool.h>
#include <moarInterfaceChannel.h>
#include <sys/epoll.h>
#include <signal.h>
#include <poll.h>
#include <moarTime.h>
#include <time.h>

#include <stdio.h>

#define EPOLL_SOCKETS_COUNT 				2
#define EPOLL_CHANNEL_EVENTS 				EPOLLIN
#define EPOLL_TIMEOUT						1000
#define EPOLL_EVENTS_COUNT					EPOLL_SOCKETS_COUNT
#define CHANNEL_PROCESSING_RULES_COUNT		5
#define IFACE_ADDR_SIZE						4
#define NEIGHBORS_TABLE_SIZE				37
#define LOG_FILE_PATH						"/tmp/lora_iface.log"
typedef struct {
	uint8_t Address[IFACE_ADDR_SIZE];
}IfaceAddr_T;

#include <moarLoraSettings.h>
#include <hashTable.h>
#include <moarIfaceStructs.h>

typedef struct{
	// layer logic
	int 					ChannelSocket;
	int 					SignalFd;
	int 					EpollHandler;
	struct epoll_event 		EpollEvent[EPOLL_EVENTS_COUNT];
	int 					EpollCount;
	sigset_t				SignalMask;
	int 					EpollTimeout;
	bool 					Running;
	bool 					Registred;
	CommandProcessingRule_T ChannelProcessingRules[CHANNEL_PROCESSING_RULES_COUNT];
	SocketFilepath_T 		ChannelSocketPath;
	// interface settings
	IfaceSettings_T			Settings;
	IfaceAddr_T				LocalAddress;
	void* 					BeaconData;
	PayloadSize_T 			BeaconDataSize;
	// rf channel
	uint8_t 				ListeningChannel;
	uint16_t 				ListeningSeed;
	// neighbors
	hashTable_T				Neighbors;
	bool 					Busy;
	// inrerface states
	bool					ListenBeacon;
	bool 					Startup;
	bool					WaitingResponse;
	bool 					MonitorMode;
	bool 					LastIsBeacon;
	// timeouts
	moarTime_T 				LastBeacon;
	moarTime_T 				LastBeaconSent;
	moarTime_T 				StartupTime;
	moarTime_T 				LastBeaconReceived;
	moarTime_T 				ListenBeaconStart;
	moarTime_T 				TransmitResetTime;
	moarTime_T				BeaconSendInterval;
	moarTime_T 				WaitingResponseTime;
	// other
	uint16_t				NetSpeed;
	// current message
	CRCvalue_T 				CurrentCRC;
	CRCvalue_T 				CurrentFullCRC;
	MessageId_T				CurrentMid;
	//stats
	int 					BeaconCounter;
	int 					SentBeaconCounter;
	int 					TotalPacketsCounter;
	int 					BrokenCounter;
	// logging
	LogHandle_T 			Log;
}LoraIfaceLayer_T;

#pragma pack(push, 1)

typedef struct {
	UnIfaceAddrLen_T	Length;
	IfaceAddr_T			Value;
} IfaceRegisterMetadata_T;

// interface unregistration metadata
typedef struct {

} IfaceUnregisterMetadata_T;

// channel send command metadata
typedef struct {
	MessageId_T Id;
	bool 		NeedResponse;
	IfaceAddr_T	To;
} ChannelSendMetadata_T;

// interface receive command metadata
typedef struct {
	MessageId_T Id;
	IfaceAddr_T	From;
} IfaceReceiveMetadata_T;

// interface neighbor info command
typedef struct {
	IfaceAddr_T	Neighbor;
} IfaceNeighborMetadata_T;

#pragma pack(pop)

#endif //MOARSTACK_MOARINTERFACEPRIVATE_H
