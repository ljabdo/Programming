/* Server side - UDP Code				    */
/* By Hugh Smith	4/1/2017	*/

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "pdu.h"
#include "cpe464.h"
#include "srej.h"
#include "window.h"

#define MAXBUF 80

typedef enum State STATE;

enum State
{
	START, DONE, FILENAME, SEND_DATA, WAIT_ON_ACK, TIMEOUT_ON_ACK, WAIT_ON_EOF_ACK, TIMEOUT_ON_EOF_ACK, WINDOW_CLOSED, END_WINDOWING
};

void processClient(int32_t serverSocketNumber, uint8_t * buf, int32_t recv_len,
	Connection * client);
int checkArgs(int argc, char *argv[], double* errorRate);
void processServer(int serverSocketNumber);
//int filename(int socketNum, struct sockaddr_in6 * client, uint8_t * pdu, int clientAddrLen, int * dataFd, int* winBuf);
void handleZombies(int sig);
STATE filename(Connection * client, uint8_t * buf, int32_t recv_len,
	int32_t * data_file, int32_t * buf_size, int32_t * win_size);
STATE send_data(Connection *client, uint8_t * packet, int32_t * packet_len,
	int32_t data_file, int buf_size, uint32_t * seq_num);
STATE timeout_on_eof_ack(Connection * client, uint8_t * packet, int32_t packet_len);
STATE timeout_on_ack(Connection * client, uint8_t * packet, int32_t packet_len);
STATE wait_on_eof_ack(Connection * client);
STATE wait_on_ack(Connection * client);
STATE window_closed(Connection * client);


int main ( int argc, char *argv[])
{ 
	int32_t serverSocketNum = 0;				
	int portNumber = 0;
	double errorRate = 0;
	
	portNumber = checkArgs(argc, argv, &errorRate);
	printf("errorRate = %lf\n", errorRate);

	sendErr_init(errorRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
		
	serverSocketNum = udpServerSetup(portNumber);

	processServer(serverSocketNum);

	//processClient(socketNum);

	//close(socketNum);
	
	return 0;
}

/*
int main ( int argc, char *argv[])
{ 
	int socketNum = 0;				
	int portNumber = 0;
	double errorRate = 0;
	

	portNumber = checkArgs(argc, argv, &errorRate);
	printf("errorRate = %lf\n", errorRate);

	sendErr_init(errorRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
		
	socketNum = udpServerSetup(portNumber);

	processServer(socketNum);

	processClient(socketNum);

	close(socketNum);
	
	return 0;
}
*/

void processServer(int serverSocketNumber)
{
	pid_t pid = 0;
	uint8_t buf[MAX_LEN];
	Connection * client = (Connection *) calloc(1, sizeof(Connection));
	uint8_t flag = 0;
	uint32_t seq_num = 0;
	int32_t recv_len = 0;

	signal(SIGCHLD, handleZombies);

	while (1)
	{
		//blocking wait for new client
		recv_len = recv_buf(buf, MAX_LEN, serverSocketNumber, client, &flag, &seq_num);

		if (recv_len != CRC_ERROR)
		{
			if ((pid = fork()) < 0)
			{
				perror("Fork");
				exit(-1);
			}

			if (pid == 0)
			{
				//child process
				printf("Child fork() - child-pid %d\n", getpid());
				processClient(serverSocketNumber, buf, recv_len, client);
				exit(0);
			}
		}
	}
}

void processClient(int32_t serverSocketNumber, uint8_t * buf, int32_t recv_len,
	Connection * client)
{  
	STATE state = START;
	int32_t data_file = 0;
	int32_t packet_len = 0;
	uint8_t packet[MAX_LEN];
	int32_t buf_size = 0;
	int32_t win_size = 0;
	uint32_t seq_num = START_SEQ_NUM;

	while(state != DONE)
	{
		switch(state)
		{
			case START:
			{
				state = FILENAME;
				break;
			}

			case FILENAME:
			{
				state = filename(client, buf, recv_len, &data_file, &buf_size, &win_size);
				createWindowTable(win_size);
				break;
			}

			case SEND_DATA:
			{
				state = send_data(client, packet, &packet_len, data_file, buf_size,
				&seq_num);
				break;
			}

			case WAIT_ON_ACK:
			{
				state = wait_on_ack(client);
				break;
			}

			case TIMEOUT_ON_ACK:
			{
				state = timeout_on_ack(client, packet, packet_len);
				break;
			}

			case WAIT_ON_EOF_ACK:
			{
				state = wait_on_eof_ack(client);
				break;
			}

			case TIMEOUT_ON_EOF_ACK:
			{
				state = timeout_on_eof_ack(client, packet, packet_len);
				break;
			}

			case WINDOW_CLOSED:
			{
				state = window_closed(client);
				break;
			}

			case DONE:
			{
				printf("server done");
				exit(EXIT_FAILURE);
				break;
			}

			default:
			{
				printf("Error in default case\n");
				state = DONE;
				break;
			}
		}
	}
}

STATE window_closed(Connection * client)
{
	STATE returnValue = SEND_DATA;
	uint32_t crc_check = 0;
	uint8_t buf[MAX_LEN];
	uint8_t packet[MAX_LEN];
	int32_t len = MAX_LEN;
	uint8_t flag = 0;
	uint32_t seq_num = 0;
	int count = 0;	

	buffEntry * bufEnt;
	while(count < 10)
	{
		if (select_call(client->sk_num, SHORT_TIME, 0) == 1)
		{
			crc_check = recv_buf(buf, len, client->sk_num, client, &flag, &seq_num);

			if (crc_check == CRC_ERROR)
			{
				returnValue = SEND_DATA;
			}

			else if (flag == ACK){
				printf("RR received == %d\n", seq_num);
				processRR(seq_num);
				if (checkStatus())
					returnValue = SEND_DATA;
				else
					returnValue = WINDOW_CLOSED;
				return returnValue;
			}
			else if (flag == SREJ){
				bufEnt = getPdu(seq_num);
				send_buf(bufEnt->pdu, bufEnt->length, client, DATA, seq_num, packet);
				returnValue = SEND_DATA;
				return returnValue;
			}
		}

	bufEnt = getLowestPdu();
	send_buf(bufEnt->pdu, bufEnt->length, client, DATA, bufEnt->seqNum, packet);
	count++;
	}
	returnValue = DONE;
	return returnValue;
}

STATE filename(Connection * client, uint8_t * buf, int32_t recv_len,
	int32_t * data_file, int32_t * buf_size, int32_t * win_size)
{
	uint8_t response[1];
	char fname[MAX_LEN];
	STATE returnValue = DONE;

	//extract buffer
	memcpy(buf_size, buf, SIZE_OF_BUF_SIZE);
	*buf_size = ntohl(*buf_size);

	//extract window size
	memcpy(win_size, &buf[4], SIZE_OF_BUF_SIZE);
	*win_size = ntohl(*win_size);

	//get filename
	memcpy(fname, &buf[8], recv_len - 8);
	//memcpy(fname, &buf[sizeof(*buf_size)], recv_len - SIZE_OF_BUF_SIZE);

	printf("File name = %s\n", fname);

	//create client socket
	client->sk_num = safeGetUdpSocket();

	if (((*data_file) = open(fname, O_RDONLY)) < 0)
	{
		send_buf(response, 0, client, FNAME_BAD, 0, buf);
		returnValue = DONE;
		perror("file open");
	}
	else{
		send_buf(response, 0, client, FNAME_OK, 0, buf);
		returnValue = SEND_DATA;
	}

	return returnValue;
}

STATE send_data(Connection *client, uint8_t * packet, int32_t * packet_len,
	int32_t data_file, int buf_size, uint32_t * seq_num)
{
	uint8_t buf[MAX_LEN];
	int32_t len_read = 0;
	STATE returnValue = DONE;

	len_read = read(data_file, buf, buf_size);

	switch(len_read)
	{
		case -1:
		{
			perror("send data, read error");
			returnValue = DONE;
			break;
		}

		case 0:
		{
			(*packet_len) = send_buf(buf, 1, client, END_OF_FILE, *seq_num, packet);
			//addPdu(buf, *seq_num, DATA, *packet_len);
			returnValue = WAIT_ON_EOF_ACK;
			close(data_file);
			break;
		}

		default:
		{
			(*packet_len) = send_buf(buf, len_read, client, DATA, *seq_num, packet);
			addPdu(buf, *seq_num, DATA, *packet_len);
			(*seq_num)++;
			returnValue = WAIT_ON_ACK;
			if (checkStatus())
				returnValue = WAIT_ON_ACK;
			else
				returnValue = WINDOW_CLOSED;
			break;
		}
	}
	return returnValue;
}

STATE wait_on_ack(Connection * client)
{
	STATE returnValue = SEND_DATA;
	uint32_t crc_check = 0;
	uint8_t buf[MAX_LEN];
	uint8_t packet[MAX_LEN];
	int32_t len = MAX_LEN;
	uint8_t flag = 0;
	uint32_t seq_num = 0;
	//static int retryCount = 0;
	buffEntry * bufEnt;

	//if ((returnValue = processSelect(client, &retryCount, TIMEOUT_ON_ACK,
	//	SEND_DATA, DONE)) == SEND_DATA)
	if (select_call(client->sk_num, 0, 0) == 1)
	{

		crc_check = recv_buf(buf, len, client->sk_num, client, &flag, &seq_num);

		if (crc_check == CRC_ERROR)
		{
			returnValue = SEND_DATA;
		}
		else if (flag == ACK){
			printf("RR received == %d\n", seq_num);
			processRR(seq_num);
			if (checkStatus())
				returnValue = SEND_DATA;
			else
				returnValue = WINDOW_CLOSED;
		}
		else if (flag == SREJ){
			bufEnt = getPdu(seq_num);
			send_buf(bufEnt->pdu, bufEnt->length, client, DATA, seq_num, packet);
		}
		//else if (flag != ACK)
		//{
		//	printf("wait_on_ack but not ack flag - shouild not happen");
		//	returnValue = DONE;
		//}
	}

	return returnValue;
}

STATE wait_on_eof_ack(Connection * client)
{
	//STATE returnValue = DONE;
	uint32_t crc_check = 0;
	uint8_t buf[MAX_LEN];
	int32_t len = MAX_LEN;
	uint8_t flag = 0;
	uint32_t seq_num = 0;
	//static int retryCount = 0;
	buffEntry * bufEnt;
	int count = 0;
		
		while(count < 10)
		{
			if (select_call(client->sk_num, SHORT_TIME, 0) == 1)
			{
				crc_check = recv_buf(buf, len, client->sk_num, client, &flag, &seq_num);

				if (crc_check == CRC_ERROR)
				{
					//returnValue = SEND_DATA;
				}

				else if (flag == ACK){
					printf("RR received == %d\n", seq_num);
					processRR(seq_num);
				}
				else if (flag == SREJ){
					bufEnt = getPdu(seq_num);
					send_buf(bufEnt->pdu, bufEnt->length, client, DATA, seq_num, buf);
				}
				else if (flag == EOF_ACK)
				{
					printf("File transfer completed successfully\n");
					return DONE;
				}
			}
			else
			{
				bufEnt = getLowestPdu();
				send_buf(bufEnt->pdu, bufEnt->length, client, DATA, bufEnt->seqNum, buf);
				count++;
			}
			
		}
	
	return DONE;
	
/*
	if ((returnValue = processSelect(client, &retryCount, TIMEOUT_ON_EOF_ACK,
		DONE, DONE)) == DONE)
	{
		crc_check = recv_buf(buf, len, client->sk_num, client, &flag, &seq_num);

		if (crc_check == CRC_ERROR)
		{
			returnValue = WAIT_ON_EOF_ACK;
		}
		else if (flag != EOF_ACK)
		{
			printf("wait_on_ack but not ack flag - shouild not happen");
			returnValue = DONE;
		}
		else
		{
			printf("File transfer completed successfully\n");
			returnValue = DONE;
		}
	}

	return returnValue;
	*/
}

STATE timeout_on_ack(Connection * client, uint8_t * packet, int32_t packet_len)
{
	safeSendToUdp(packet, packet_len, client);

	return WAIT_ON_ACK;
}

STATE timeout_on_eof_ack(Connection * client, uint8_t * packet, int32_t packet_len)
{
	safeSendToUdp(packet, packet_len, client);

	return WAIT_ON_EOF_ACK;
}

void handleZombies(int sig)
{
	int stat = 0;
	while (waitpid(-1, &stat, WNOHANG) > 0)
	{}
}

/*
int filename(int socketNum, struct sockaddr_in6 * client, uint8_t * pdu, int clientAddrLen, int * dataFd, int* winBuf)
{
	int pduLen = 0;
	char fileName[101];
	int fileLen = 0;
	while(1)
	{
		//do checksum for bad packet
		//check pduLen to actually know when packet received	
		pduLen = safeRecvfrom(socketNum, pdu, MAXPDULEN, 0, (struct sockaddr *) &client, &clientAddrLen);
		if (pduLen > 0){
			if(checkPacket(pdu, pduLen))
				break;
		}
	}

	//recv filename length
	memcpy(&fileLen, &pdu[7], 4);

	//recv filename
	memcpy(&fileName[0], &pdu[11], fileLen + 1);

	//buffersize
	memcpy(&winBuf[1], &pdu[12 + fileLen], 4);

	//windowsize
	memcpy(&winBuf[0], &pdu[16 + fileLen], 4);

	printf("filename = %s\n", fileName);

	if (((*dataFd) = open(fileName, O_RDONLY)) > 0)
	{
		printf("file ok\n");
		pduLen = createPDU(pdu, 0, 8, NULL, 0);
		sendtoErr(socketNum, pdu, pduLen, 0, (struct sockaddr *) &client, clientAddrLen);
	}
	else{
		printf("file not ok\n");
		pduLen = createPDU(pdu, 0, 9, NULL, 0);
		sendtoErr(socketNum, pdu, pduLen, 0, (struct sockaddr *) &client, clientAddrLen);
	}
	return DONE;
}
*/

/*
void processClient(int socketNum)
{  
	struct sockaddr_in6 client;		
	int clientAddrLen = sizeof(client);

	int pduLen = 0;	
	uint8_t pdu[MAXPDULEN];

	//test buffer for server ack
	//char buffer[MAXBUF+1];
	
	while (1)
	{
		memset(&pdu[0], 0, MAXPDULEN);
		pduLen = safeRecvfrom(socketNum, pdu, MAXPDULEN, 0, (struct sockaddr *) &client, &clientAddrLen);

		printf("\nReceived message from client with ");
		printIPInfo(&client);

		outputPDU(pdu, pduLen);

		//send back ACK for testing
		//strcpy(buffer, "ACK");
		//pduLen = createPDU(pdu, 99, 3,(uint8_t *) buffer, 4);

		//receive from server
		//safeSendto will recieve broken message from server
		//safeSendto(socketNum, pdu, pduLen, 0, (struct sockaddr *) &client, clientAddrLen);

		//when duplicate message sent back to client - does not send broken message

		//send to error will recieve fixed message from server
		//sendtoErr(socketNum, pdu, pduLen, 0, (struct sockaddr *) &client, clientAddrLen);
	}
}
*/

int checkArgs(int argc, char *argv[], double* errorRate)
{
	// Checks args and returns port number
	int portNumber = 0;
	double temp = 0;

	if ((argc > 3) || (argc == 1))
	{
		fprintf(stderr, "Usage %s error-rate [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 3)
	{
		portNumber = atoi(argv[2]);
	}

	temp = atof(argv[1]);
	if ((temp >= 0) && (temp < 1))
		*errorRate = temp;
	else{
		fprintf(stderr, "Usage %s error-rate [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	return portNumber;
}


