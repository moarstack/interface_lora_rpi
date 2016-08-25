//
// Created by kryvashek on 09.07.16.
// for private notations

#ifndef MOARSTACK_MOARINTERFACEPRIVATE_H
#define MOARSTACK_MOARINTERFACEPRIVATE_H

#include <stdint.h>
#include <stdbool.h>
#include <moarInterfaceChannel.h>
#include <sys/epoll.h>

#define EPOLL_SOCKETS_COUNT 				1
#define EPOLL_CHANNEL_EVENTS 				EPOLLIN
#define EPOLL_TIMEOUT						1000
#define EPOLL_EVENTS_COUNT					EPOLL_SOCKETS_COUNT
#define CHANNEL_PROCESSING_RULES_COUNT		5
#define IFACE_ADDR_SIZE						4

typedef struct {
	uint8_t Address[IFACE_ADDR_SIZE];
}IfaceAddr_T;

#include <moarILoraSettings.h>

typedef struct{
	int 					ChannelSocket;
	int 					EpollHandler;
	struct epoll_event 		EpollEvent[EPOLL_EVENTS_COUNT];
	int 					EpollCount;
	bool 					Running;
	int 					EpollTimeout;
	bool 					Registred;
	CommandProcessingRule_T ChannelProcessingRules[CHANNEL_PROCESSING_RULES_COUNT];
	SocketFilepath_T 		ChannelSocketPath;
	IfaceAddr_T				LocalAddress;
	IfaceSettings_T			Settings;
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
