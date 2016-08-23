//
// Created by kryvashek on 09.07.16.
// for private notations

#ifndef MOARSTACK_MOARINTERFACEPRIVATE_H
#define MOARSTACK_MOARINTERFACEPRIVATE_H

#include <stdint.h>

#define EPOLL_SOCKETS_COUNT 				1
#define EPOLL_CHANNEL_EVENTS 				EPOLLIN
#define EPOLL_TIMEOUT						1000
#define EPOLL_EVENTS_COUNT					EPOLL_SOCKETS_COUNT
#define CHANNEL_PROCESSING_RULES_COUNT		5
#define IFACE_ADDR_SIZE	4

typedef struct {
	uint8_t Address[IFACE_ADDR_SIZE];
}IfaceAddress_T;


typedef struct{
	int 					ChannelSocket;
	int 					EpollHandler;
	struct epoll_event 		EpollEvent[EPOLL_EVENTS_COUNT];
	int 					EpollCount;
	bool 					Running;
	int 					EpollTimeout;
	bool 					Enabled;
	CommandProcessingRule_T ChannelProcessingRules[CHANNEL_PROCESSING_RULES_COUNT];
	SocketFilepath_T 		ChannelSocketPath;
}LoraIfaceLayer_T;

#endif //MOARSTACK_MOARINTERFACEPRIVATE_H
