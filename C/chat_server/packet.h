#ifndef __PACKET_H__
#define __PACKET_H__

#include "handleTable.h"

#define FLAG1 1
#define FLAG2 2
#define FLAG3 3
#define FLAG4 4
#define FLAG5 5
#define FLAG7 7
#define FLAG8 8
#define FLAG9 9
#define FLAG10 10
#define FLAG11 11
#define FLAG12 12
#define FLAG13 13
#define HEADER_LEN 3
#define FLAG11_SIZE 4

void sendHandlePacket(int socketNum, char * handle);

void processFlag1(int clientSocket, uint8_t * buf, int  messageLen);

void processFlags(int clientSocket, uint8_t * buf, int messageLen);

void sendHeader(int clientSocket, int flag);

void processFlag2();

void processFlag3();

void sendExitFlag(int serverSocket);

void processFlag8(int cSocket);

void cleanExit();

void sendHandleListFlag(int serverSocket);

void processFlag10(int clientSocket, uint8_t * buf, int  messageLen);

void processFlag11(int clientSocket, uint8_t * buf, int  messageLen);

void sendHandleList(int clientSocket);

void sendLPacket(int clientSocket, handleEntry * entry);

void recvHandleTableData(int serverSocket);

void processFlag5(int clientSocket, uint8_t * buf, int  messageLen);

void sendFlag7(int clientSocket, char * nextHandle, uint8_t handleLen);

void processFlag7(int clientSocket, uint8_t * buf, int  messageLen);

void processFlag4(int clientSocket, uint8_t * buf, int  messageLen);


#endif