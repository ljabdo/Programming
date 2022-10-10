#include <stdio.h>
#include <stdlib.h>

#include "networks.h"
#include "pdu.h"
#include "packet.h"
#include "handleTable.h"
#include "pollLib.h"

#define HANDLE_SIZE 100
#define MAXBUF 1024


void sendHandlePacket(int socketNum, char * handle)
{
	size_t handleLen;
    int bytesSent = 0;
	if ((handleLen = strlen(handle)) > HANDLE_SIZE){
		perror("Handle size to big\n");
		exit(EXIT_FAILURE);
	}
    //handlelen + 1 if breaks
	bytesSent = sendPDU(socketNum, (uint8_t *) handle, handleLen + 1, FLAG1);
    if (bytesSent < handleLen){
        perror("Error on initial packet send\n");
        exit(EXIT_FAILURE);
    }
}

void processFlags(int clientSocket, uint8_t * buf, int messageLen)
{
    uint8_t flag = buf[0];

    switch(flag)
    {
        case 1: 
            processFlag1(clientSocket, buf, messageLen);
            break;

        case 2:
            processFlag2();
            break;

        case 3:
            processFlag3();
            break;
        
        case 4:
            processFlag4(clientSocket, buf, messageLen);
            break;

        case 5:
            processFlag5(clientSocket, buf, messageLen);
            break;

        case 8:
            processFlag8(clientSocket);
            break;

        case 7:
            processFlag7(clientSocket, buf, messageLen);
            break;

        case 9:
            cleanExit();
            break;

        case 10:
            processFlag10(clientSocket, buf, messageLen);
            break;

        case 11:
            processFlag11(clientSocket, buf, messageLen);
            break;
    }
    //printf("flag = %d\n", flag);
    //printf("handle = %s\n", &(buf[1]));
}

void processFlag1(int clientSocket, uint8_t * buf, int  messageLen)
{
    //need to compare if handle is already in table to send back flag 2
    if (validHandle((char *) &(buf[1]), messageLen - 1)){
        addHandle(clientSocket, (char *) &(buf[1]), messageLen - 1); //to remove flag
        //printf("Added handle to table = %s socket = %d len = %d\n\n", &(buf[1]), clientSocket,  messageLen - 1);
        sendHeader(clientSocket, FLAG2);
    }
    else
        sendHeader(clientSocket, FLAG3);

    //return (char *) &(buf[1]);
}

void processFlag2()
{
    printf("\nGood handle successful connection!\n");
}

void processFlag3()
{
    perror("Error when connecting - handle already exists");
    exit(EXIT_FAILURE);
}

void processFlag4(int clientSocket, uint8_t * buf, int  messageLen)
{
    uint8_t senderLength;
    char senderHandle[HANDLE_SIZE];
    int i;
    handleEntry * handle;
    memcpy(&senderLength, &buf[1], 1);
    memcpy(&senderHandle, &buf[2], senderLength);

    for (i = 0; i < getTableSize(); i++)
    {
        handle = getHandleOf(i);
        if ((handle->socketNumber > 0) && ((handle->socketNumber != clientSocket)))
        {
            sendPDU(handle->socketNumber, buf, messageLen, FLAG4);
        }
    }  
}

void processFlag5(int clientSocket, uint8_t * buf, int  messageLen)
{
    uint8_t senderLength;
    char senderHandle[HANDLE_SIZE];
    uint8_t numHandles = 0;
    int i;
    uint8_t handleLen;
    char nextHandle[HANDLE_SIZE];
    int index;
    int sendSocket = 0;
    memcpy(&senderLength, &buf[1], 1);
    memcpy(&senderHandle, &buf[2], senderLength);
    memcpy(&numHandles, &buf[senderLength + 2], 1);
    index = senderLength + 3;
    for (i = 0; i < numHandles; i++)
    {
        memcpy(&handleLen, &buf[index], 1);
        memcpy(&nextHandle, &buf[index + 1], handleLen);
        if (validHandle(nextHandle, handleLen)){
            sendFlag7(clientSocket, nextHandle, handleLen);
        }
        else{
            sendSocket = searchHandle(nextHandle, handleLen)->socketNumber;
            sendPDU(sendSocket, buf, messageLen, FLAG5);
        }
        index += handleLen + 2;
    }
}

void sendFlag7(int clientSocket, char * nextHandle, uint8_t handleLen)
{
    char data[HANDLE_SIZE + 1];
    memcpy(&data[0], &handleLen, 1);
    memcpy(&data[1], nextHandle, handleLen);
    sendPDU(clientSocket, (uint8_t *) data, handleLen + 1, FLAG7);
}

void processFlag7(int clientSocket, uint8_t * buf, int  messageLen)
{
    char clientHandle[HANDLE_SIZE];
    uint8_t handleLen;
    memcpy(&handleLen, &buf[1], 1);
    memcpy(&clientHandle, &buf[2], handleLen);
    printf("Client %s does not exist\n", clientHandle);
}

void processFlag8(int cSocket)
{
    removeFromPollSet(cSocket);
	if (removeHandle(cSocket)){
		printf("Could not remove handle\n");
	}
    sendHeader(cSocket, FLAG9);
    close(cSocket);
    printf("Client %d closing cleanly\n", cSocket);
}

void processFlag10(int clientSocket, uint8_t * buf, int  messageLen)
{
    int bytesSent = 0;
    int numEntries = getNumEntries();
    numEntries = htonl(numEntries);
    uint8_t data[FLAG11_SIZE];
    memcpy(&data[0], &numEntries, FLAG11_SIZE);
    bytesSent = sendPDU(clientSocket, data, FLAG11_SIZE, FLAG11);
    if (bytesSent < FLAG11_SIZE){
        printf("Flag 11 sending error - did not send num table entries\n");
        exit(EXIT_FAILURE);
    }
    sendHandleList(clientSocket);
}

void sendHandleList(int clientSocket)
{
    int i;
    handleEntry * entry;
    for (i = 0; i < getTableSize(); i++){
        if (((entry = getHandleOf(i))->socketNumber) > 3)
        {
            sendLPacket(clientSocket, entry);
        }
    }
    //send flag 13 packet
    sendHeader(clientSocket, FLAG13);
}

void sendLPacket(int clientSocket, handleEntry * entry)
{
    int handleLen;
    handleLen = strlen(entry->handle) + 1;
    sendPDU(clientSocket, (uint8_t *) entry->handle, handleLen, FLAG12);
}

void processFlag11(int clientSocket, uint8_t * buf, int  messageLen)
{
    int num = 0;
    memcpy(&num, &(buf[1]), messageLen - 1);
    num = ntohl(num);
    printf("\nList of handles received from server (# = %d)\n", num);
    recvHandleTableData(clientSocket);
}

void recvHandleTableData(int serverSocket)
{
    char buf[MAXBUF];
	int messageLen = 0;
    while(1){
        if ((messageLen = recvPDU(serverSocket, (uint8_t *) buf, MAXBUF)) < 0){
            perror("recv handle table error\n");
            exit(-1);
	    }
        if (buf[0] == FLAG12){
            printf("    %s\n", &(buf[1]));
        }
        else if (buf[0] == FLAG13)
            return;
    }
}

void sendExitFlag(int serverSocket)
{
    sendHeader(serverSocket, FLAG8);
}

void sendHandleListFlag(int serverSocket)
{
    sendHeader(serverSocket, FLAG10);
}

void sendHeader(int clientSocket, int flag)
{
    int bytesSent = 0;
	bytesSent = sendPDU(clientSocket, 0, 0, flag);
    if (bytesSent < HEADER_LEN){
        perror("Server flag send error\n");
        exit(EXIT_FAILURE);
    }
}

void cleanExit()
{
	printf("Client quitting cleanly\n");
	exit(EXIT_SUCCESS);
}