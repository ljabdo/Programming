
// Hugh Smith April 2017
// Network code to support TCP/UDP client and server connections

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
#include "gethostbyname.h"
#include "cpe464.h"

int safeGetUdpSocket()
{
	int socketNumber = 0;

	if ((socketNumber = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
	{
		perror("safeGetUdpSocket(), socket() call: ");
		exit(-1);
	}
	return socketNumber;
}

int safeSendToUdp(uint8_t * packet, uint32_t len, Connection * to)
{
	int send_len = 0;

	if ((send_len = sendtoErr(to->sk_num, packet, len, 0,
		(struct sockaddr *) &(to->remote), to->len)) < 0)
		{
			perror("in safeSendto(), sendto() call");
			exit(-1);
		}

	return send_len;
}

int safeRecvFromUdp(int recv_sk_num, uint8_t * packet, int len, Connection * from)
{
	int recv_len = 0;
	from->len = sizeof(struct sockaddr_in6);

	if ((recv_len = recvfrom(recv_sk_num, packet, len, 0,
		(struct sockaddr *) &(from->remote), &from->len)) < 0)
		{
			perror("in safeRecvFrom, recvfrom call");
			exit(-1);
		}

	return recv_len;
}

int udpClientSetup(char * hostName, int portNumber, Connection * connection)
{
	memset(&connection->remote, 0, sizeof(struct sockaddr_in6));
	connection->sk_num = 0;
	connection->len = sizeof(struct sockaddr_in6);
	connection->remote.sin6_family = AF_INET6;
	connection->remote.sin6_port = htons(portNumber);

	//create socket
	connection->sk_num = safeGetUdpSocket();

	if (gethostbyname6(hostName, &connection->remote) == NULL)
	{
		printf("Host not found! : %s\n", hostName);
		return -1;
	}

	printf("Server info - ");
	printIPv6Info(&connection->remote);

	return 0;
}

void printIPv6Info(struct sockaddr_in6 * client)
{
	char ipString[INET6_ADDRSTRLEN];

	inet_ntop(AF_INET6, &client->sin6_addr, ipString, sizeof(ipString));
	printf("IP: %s Port: %d \n", ipString, ntohs(client->sin6_port));
}

int select_call(int32_t socket_num, int32_t seconds, int32_t microseconds)
{
	fd_set fdvar;
	struct timeval aTimeout;
	struct timeval * timeout = NULL;

	if (seconds != -1 && microseconds != -1)
	{
		aTimeout.tv_sec = seconds;
		aTimeout.tv_usec = microseconds;
		timeout = &aTimeout;
	}

	FD_ZERO(&fdvar);
	FD_SET(socket_num, &fdvar);

	if (select(socket_num + 1,(fd_set *) &fdvar, NULL, NULL, timeout) < 0)
	{
		perror("select");
		exit(-1);
	}

	if (FD_ISSET(socket_num, &fdvar))
	{
		return 1;
	}
	else{
		return 0;
	}
}


// This function sets the server socket. The function returns the server
// socket number and prints the port number to the screen.  

int tcpServerSetup(int serverPort)
{
	int mainServerSocket = 0;
	struct sockaddr_in6 serverAddress;     
	socklen_t serverAddressLen = sizeof(serverAddress);  

	mainServerSocket= socket(AF_INET6, SOCK_STREAM, 0);
	if(mainServerSocket < 0)
	{
		perror("socket call");
		exit(1);
	}

	memset(&serverAddress, 0, sizeof(struct sockaddr_in6));
	serverAddress.sin6_family= AF_INET6;         		
	serverAddress.sin6_addr = in6addr_any;   
	serverAddress.sin6_port= htons(serverPort);         

	// bind the name (address) to a port 
	if (bind(mainServerSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("bind call");
		exit(-1);
	}
	
	// get the port name and print it out
	if (getsockname(mainServerSocket, (struct sockaddr*)&serverAddress, &serverAddressLen) < 0)
	{
		perror("getsockname call");
		exit(-1);
	}

	if (listen(mainServerSocket, BACKLOG) < 0)
	{
		perror("listen call");
		exit(-1);
	}
	
	printf("Server Port Number %d \n", ntohs(serverAddress.sin6_port));
	
	return mainServerSocket;
}

// This function waits for a client to ask for services.  It returns
// the client socket number.   

int tcpAccept(int mainServerSocket, int debugFlag)
{
	struct sockaddr_in6 clientAddress;   
	int clientAddressSize = sizeof(clientAddress);
	int client_socket = 0;

	if ((client_socket = accept(mainServerSocket, (struct sockaddr*) &clientAddress, (socklen_t *) &clientAddressSize)) < 0)
	{
		perror("accept call");
		exit(-1);
	}
	  
	if (debugFlag)
	{
		printf("Client accepted.  Client IP: %s Client Port Number: %d\n",  
				getIPAddressString6(clientAddress.sin6_addr.s6_addr), ntohs(clientAddress.sin6_port));
	}
	

	return(client_socket);
}

int tcpClientSetup(char * serverName, char * serverPort, int debugFlag)
{
	// This is used by the client to connect to a server using TCP
	
	int socket_num;
	uint8_t * ipAddress = NULL;
	struct sockaddr_in6 serverAddress;      
	
	// create the socket
	if ((socket_num = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
	{
		perror("socket call");
		exit(-1);
	}

	// setup the server structure
	memset(&serverAddress, 0, sizeof(struct sockaddr_in6));
	serverAddress.sin6_family = AF_INET6;
	serverAddress.sin6_port = htons(atoi(serverPort));
	
	// get the address of the server 
	if ((ipAddress = gethostbyname6(serverName, &serverAddress)) == NULL)
	{
		exit(-1);
	}

	if(connect(socket_num, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("connect call");
		exit(-1);
	}

	if (debugFlag)
	{
		printf("Connected to %s IP: %s Port Number: %d\n", serverName, getIPAddressString6(ipAddress), atoi(serverPort));
	}
	
	return socket_num;
}

int udpServerSetup(int serverPort)
{
	struct sockaddr_in6 serverAddress;
	int socketNum = 0;
	int serverAddrLen = 0;	
	
	// create the socket
	if ((socketNum = socket(AF_INET6,SOCK_DGRAM,0)) < 0)
	{
		perror("socket() call error");
		exit(-1);
	}
	
	// set up the socket
	memset(&serverAddress, 0, sizeof(struct sockaddr_in6));
	serverAddress.sin6_family = AF_INET6;    		// internet (IPv6 or IPv4) family
	serverAddress.sin6_addr = in6addr_any ;  		// use any local IP address
	serverAddress.sin6_port = htons(serverPort);   // if 0 = os picks 

	// bind the name (address) to a port
	if (bind(socketNum,(struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("bind() call error");
		exit(-1);
	}

	/* Get the port number */
	serverAddrLen = sizeof(serverAddress);
	getsockname(socketNum,(struct sockaddr *) &serverAddress,  (socklen_t *) &serverAddrLen);
	printf("Server using Port #: %d\n", ntohs(serverAddress.sin6_port));

	return socketNum;	
	
}

int setupUdpClientToServer(struct sockaddr_in6 *serverAddress, char * hostName, int serverPort)
{
	// currently only setup for IPv4 
	int socketNum = 0;
	char ipString[INET6_ADDRSTRLEN];
	uint8_t * ipAddress = NULL;
	
	// create the socket
	if ((socketNum = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket() call error");
		exit(-1);
	}
  	 	
	memset(serverAddress, 0, sizeof(struct sockaddr_in6));
	serverAddress->sin6_port = ntohs(serverPort);
	serverAddress->sin6_family = AF_INET6;	
	
	if ((ipAddress = gethostbyname6(hostName, serverAddress)) == NULL)
	{
		exit(-1);
	}
		
	
	inet_ntop(AF_INET6, ipAddress, ipString, sizeof(ipString));
	printf("Server info - IP: %s Port: %d \n", ipString, serverPort);
		
	return socketNum;
}


