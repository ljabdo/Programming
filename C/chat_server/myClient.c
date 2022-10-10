/******************************************************************************
* myClient.c
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "networks.h"
#include "pdu.h"
#include "packet.h"
#include "pollLib.h"
#include "client.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1
#define MSGLENGTH 1400
#define M_MAX_HANDLES 9
#define MAX_HANDLE_SIZE 100
#define MAX_MSG_PACKET_SIZE 1015

int main(int argc, char * argv[])
{
	int serverSocketNum = 0;         //socket descriptor
	int recSocket = -1;
	
	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
	serverSocketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);
	setupPollSet();
	sendHandlePacket(serverSocketNum, argv[1]);
	addToPollSet(serverSocketNum);
	addToPollSet(0);
	
	while(1){

		printf("$: ");
		fflush(stdout);

		recSocket = pollCall(-1);

		if (recSocket < 0){
			continue;
		}
		else if (recSocket > 0){
			recvFromServer(serverSocketNum);
			continue;
		}

		if (recSocket == 0){
			parseCommand(serverSocketNum, argv[1]);
		}
	}

	close(serverSocketNum);
	
	return 0;
}

void parseCommand(int serverSocket, char * selfHandle)
{
	char sendBuf[MAXBUF];
	int sendLen = 0;
	sendLen = readFromStdin(sendBuf);
	if (sendLen > MSGLENGTH){
		printf("Command length greater than 1400 bytes\n");
		return;
	}

	else if ((strncmp(sendBuf, "\%e", 2) == 0) || (strncmp(sendBuf, "\%E", 2) == 0)){
		sendExitFlag(serverSocket);
	}

	else if ((strncmp(sendBuf, "\%L", 2) == 0) || (strncmp(sendBuf, "\%l", 2) == 0)){
		sendHandleListFlag(serverSocket);
	}

	else if ((strncmp(sendBuf, "\%M", 2) == 0) || (strncmp(sendBuf, "\%m", 2) == 0)){
		parseMessagePacket(serverSocket, sendBuf, sendLen, selfHandle);
	}

	else if ((strncmp(sendBuf, "\%B", 2) == 0) || (strncmp(sendBuf, "\%b", 2) == 0)){
		parseBroadcastPacket(serverSocket, sendBuf, sendLen, selfHandle);
	}
}


void parseBroadcastPacket(int serverSocket, char * sendBuf, int sendLen, char * selfHandle)
{
	char *token;

	token = strtok(sendBuf, " ");

	token = strtok(NULL, "");
	if (token == NULL){
		printMsgUsage();
		return;
	}

	sendBroadcastPacket(token, serverSocket, selfHandle);
}

void sendBroadcastPacket(char * token, int serverSocket, char * selfHandle)
{
	int i;
	uint8_t selfLength;
	int index = 0;
	int msgLength = 0;
	int temp = 0;
	int finalSend = 0;
	uint8_t data[MAX_MSG_PACKET_SIZE];
	selfLength = strlen(selfHandle) + 1;

	memcpy(&data[0], &selfLength, 1);
	memcpy(&data[1], selfHandle, selfLength);

	index = selfLength + 2;
	msgLength = strlen(token);
	temp = (msgLength / 200) + 1;
	finalSend = msgLength % 200;

	if (msgLength < 200){
		memcpy(&data[index], token, msgLength + 1);
		sendPDU(serverSocket, data, index + msgLength + 1, FLAG4);
		return;
	}

	for (i = 0; i < temp; i++)
	{
		msgLength = strlen(&token[i * 200]);
		if (!msgLength < 200)
			msgLength = 200;
		if (i == temp){
			memcpy(&data[index], &token[i * 200], finalSend);
			sendPDU(serverSocket, (uint8_t *) data, index + msgLength, FLAG4);
		}
		else{
			memcpy(&data[index], &token[i * 200], msgLength);
			sendPDU(serverSocket, (uint8_t *) data, index + msgLength, FLAG4);
		}
	}
}

void parseMessagePacket(int serverSocket, char * sendBuf, int sendLen, char * selfHandle)
{
	int i = 0;
	int handleCount = 0;
	char *token;

	char handles[M_MAX_HANDLES][MAX_HANDLE_SIZE];
	token = strtok(sendBuf, " ");

	token = strtok(NULL, " ");
	if (token == NULL){
		printMsgUsage();
		return;
	}
	handleCount = atoi(token);
	if (handleCount == 0){
		printMsgUsage();
		return;
	}
	if ((handleCount < 1) || (handleCount > 9)){
		printMsgUsage();
		return;
	}

	for (i = 0; i < handleCount; i++)
	{
		if (token == NULL){
			printMsgUsage();
			return;
		}
		token = strtok(NULL, " ");
		memcpy(handles[i], token, strlen(token) + 1);	
	}
	token = strtok(NULL, "");
	sendMessagePacket(handles, token, serverSocket, selfHandle, handleCount);
}

void sendMessagePacket(char handles[9][100], char * msg, int serverSocket, char *selfHandle, int handleCount)
{
	int i;
	uint8_t selfLength;
	int index = 0;
	int msgLength = 0;
	int bytesSent = 0;
	int temp = 0;
	int finalSend = 0;
	uint8_t data[MAX_MSG_PACKET_SIZE];

	selfLength = strlen(selfHandle) + 1;
	memcpy(&data[0], &selfLength, 1);
	memcpy(&data[1], selfHandle, selfLength);
	memcpy(&data[selfLength + 1], &handleCount, 1);
	index = selfLength + 2;

	for (i = 0; i < handleCount; i++)
	{
		temp = strlen(handles[i]) + 1;
		memcpy(&data[index], &temp, 1);
		memcpy(&data[index + 1], handles[i], temp);
		index += temp + 2;
	}

	msgLength = strlen(msg);
	temp = (msgLength / 200) + 1;
	finalSend = msgLength % 200;

	if (msgLength < 200){
		memcpy(&data[index], msg, msgLength + 1);
		bytesSent = sendPDU(serverSocket, data, index + msgLength + 1, FLAG5);
		return;
	}

	for (i = 0; i < temp; i++)
	{
		msgLength = strlen(&msg[i * 200]);
		if (!msgLength < 200)
			msgLength = 200;
		if (i == temp){
			memcpy(&data[index], &msg[i * 200], finalSend);
			bytesSent = sendPDU(serverSocket, (uint8_t *) data, index + msgLength, FLAG5);
		}
		else{
			memcpy(&data[index], &msg[i * 200], msgLength);
			bytesSent = sendPDU(serverSocket, (uint8_t *) data, index + msgLength, FLAG5);
		}
	}

	if (bytesSent < msgLength){
		printf("Message packet send less bytes than expected\n");
		return;
	}
}

void printMsgUsage()
{
	printf("Msg error: Usage: \%m [num handles] [handle name/s] [text]\n");
	return;
}

void sendToServer(int socketNum)
{
	char sendBuf[MAXBUF];   //data buffer
	int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */
	
	sendLen = readFromStdin(sendBuf);
	printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);
	
	sent = sendPDU(socketNum, (uint8_t *) sendBuf, sendLen, 1);//change to flag var

	if (strcmp(sendBuf, "exit") == 0)
	{
		close(socketNum);
		printf("Client is quitting.\n");
		exit(EXIT_SUCCESS);
	}

	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}

	printf("Amount of data sent is: %d\n", sent);
}


int recvFromServer(int serverSocket)
{
	char buf[MAXBUF];
	int messageLen = 0;
	
	//now get the data from the client_socket
	if ((messageLen = recvPDU(serverSocket, (uint8_t *) buf, MAXBUF)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	//client ^c returning 0 bytes
	else if(messageLen == 0){
		printf("Client ^C socket = %d\n", serverSocket);
		return 0;
	}

	// THIS CHECKS IF CLIENT IS DISCONNECTING
	else if (strcmp(buf, "exit") == 0)
	{
		printf("Client is quitting socket = %d\n", serverSocket);
		return 0;
	}

	if (buf[1] == FLAG5){
		recvFlag5(serverSocket, (uint8_t *) buf, messageLen);
	}
	else if (buf[1] == FLAG4){
		recvFlag4(serverSocket, (uint8_t *) buf, messageLen);
	}
	else
		processFlags(serverSocket, (uint8_t *) buf, messageLen);

	//printf("Message received on socket %d, length: %d Data: %s\n\n", serverSocket, messageLen, buf);
	return 1;
}

void recvFlag5(int serverSocket, uint8_t * buf, int messageLen)
{
	char recvHandle[HANDLE_SIZE];
	char msg[200];
	uint8_t recvHandleLength;
	uint8_t numHandles;
	uint8_t handleLen;
	uint8_t index = 0;
	int i = 0;

	memcpy(&recvHandleLength, &buf[2], 1);
	memcpy(&recvHandle, &buf[3], recvHandleLength);
	memcpy(&numHandles, &buf[recvHandleLength + 3], 1);
	printf("\n%s: ", recvHandle);

	index = recvHandleLength + 4;
	for (i = 0; i < numHandles; i++)
	{
		memcpy(&handleLen, &buf[index], 1);
		index += handleLen + 2;
	}
	memcpy(&msg, &buf[index], messageLen - index);
	printf("%s\n", msg);
	return;
}

void recvFlag4(int serverSocket, uint8_t * buf, int messageLen)
{
	char recvHandle[HANDLE_SIZE];
	char msg[200];
	uint8_t recvHandleLength;
	uint8_t index = 0;

	memcpy(&recvHandleLength, &buf[2], 1);
	memcpy(&recvHandle, &buf[3], recvHandleLength);
	printf("\n%s: ", recvHandle);
	
	index = recvHandleLength + 4;
	memcpy(&msg, &buf[index], messageLen - index);
	printf("%s\n", msg);
	return;
}

int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	//printf("Enter data: ");
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}
