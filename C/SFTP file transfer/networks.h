
// 	Writen - HMS April 2017
//  Supports TCP and UDP - both client and server


#ifndef __NETWORKS_H__
#define __NETWORKS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "gethostbyname.h"

#define BACKLOG 10

typedef struct connection Connection;

struct connection
{
    int32_t sk_num;
    struct sockaddr_in6 remote;
    uint32_t len;
};


void printIPv6Info(struct sockaddr_in6 * client);
int safeGetUdpSocket();
int safeSendToUdp(uint8_t * packet, uint32_t len, Connection * to);
int safeRecvFromUdp(int recv_sk_num, uint8_t * packet, int len, Connection * from);
int udpClientSetup(char * hostName, int portNumber, Connection * connection);
int select_call(int32_t socket_num, int32_t seconds, int32_t microseconds);


// for the server side
int tcpServerSetup(int serverPort);
int tcpAccept(int mainServerSocket, int debugFlag);
int udpServerSetup(int serverPort);

// for the client side
int tcpClientSetup(char * serverName, char * serverPort, int debugFlag);
int setupUdpClientToServer(struct sockaddr_in6 *serverAddress, char * hostName, int serverPort);


#endif
