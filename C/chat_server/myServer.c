/******************************************************************************
* tcp_server.c
*
* CPE 464 - Program 1
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
#include "pollLib.h"
#include "handleTable.h"
#include "packet.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1
#define INIT_HANDLE_TABLE_SIZE 10

int recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);
void serverControl(int serverSocket);
void addNewClient(int serverSocket);
void processClient(int clientSocket);

int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	serverSocket = tcpServerSetup(portNumber);

	//set up poll set
	setupPollSet();
	createHandleTable(INIT_HANDLE_TABLE_SIZE);

	//initiate server
	serverControl(serverSocket);
	
	/* close the sockets */
	close(clientSocket);
	close(serverSocket);

	return 0;
}

void addNewClient(int serverSocket)
{
	int clientSocket;
	clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
	addToPollSet(clientSocket);
}

void processClient(int clientSocket)
{
	int recVal = recvFromClient(clientSocket);
	if (recVal == 0){
		removeFromPollSet(clientSocket);
		if (removeHandle(clientSocket)){
			printf("Could not remove handle\n");
			//exit(EXIT_FAILURE);
		}
		close(clientSocket);
	}
}

void serverControl(int serverSocket)
{
	int socketNum = 0;
	addToPollSet(serverSocket);
	
	while (1)
	{
		socketNum = pollCall(-1);
		if (socketNum < 0){
			continue;
		}

		else if (socketNum == serverSocket){
			addNewClient(serverSocket);
			//printHandleTable();
		} 

		else
			processClient(socketNum);
	}
}

int recvFromClient(int clientSocket)
{
	char buf[MAXBUF];
	int messageLen = 0;
	
	//now get the data from the client_socket
	if ((messageLen = recvPDU(clientSocket, (uint8_t *) buf, MAXBUF)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	//client ^c returning 0 bytes
	else if(messageLen == 0){
		printf("Client ^C socket = %d\n", clientSocket);
		return 0;
	}

	processFlags(clientSocket, (uint8_t *) buf, messageLen);

	return 1;
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	
	return portNumber;
}

