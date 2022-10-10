// Client side - UDP Code				    
// By Hugh Smith	4/1/2017		

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

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "pdu.h"
#include "cpe464.h"
#include "struct.h"
#include "pollLib.h"
#include "pdu.h"
#include "srej.h"
#include "window.h"

#define MAXBUF 80

typedef enum State STATE;

enum State {DONE, FILENAME, RECV_DATA, FILE_OK, START_STATE};

void talkToServer(int socketNum, struct sockaddr_in6 * server);
int readFromStdin(char * buffer);
void checkArgs(int argc, char * argv[]);
void printUsage();
int sendStartUpPacket(argStorage * args, struct sockaddr_in6 * server);
int connectionSetup(argStorage * args, struct sockaddr_in6 server);
int createStartPacket(argStorage * args, uint8_t * pdu);
void processFile(char *argv[]);
STATE start_state(char **argv, Connection * server, uint32_t * clientSeqNum);
STATE filename(char * fname, int32_t buf_size, Connection * server);
STATE file_ok(int * outputFileFd, char* outputFileName);
STATE recv_data(int32_t output_file, Connection * server, uint32_t * clientSeqNum);


int main (int argc, char *argv[])
{
	checkArgs(argc, argv);
	sendtoErr_init(atof(argv[5]), DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
	printf("errorRate = %lf\n", atof(argv[5]));
	createWindowTable(atoi(argv[3]));
	processFile(argv);
	return 0;
}

void processFile(char *argv[])
{
	Connection * server = (Connection *) calloc(1, sizeof(Connection));
	uint32_t clientSeqNum = 0;
	int32_t output_file_fd = 0;
	STATE state = START_STATE;

	while (state != DONE)
	{
		switch(state)
		{
			case START_STATE:
			{
				state = start_state(argv, server, &clientSeqNum);
				break;
			}

			case FILENAME:
			{
				state = filename(argv[1], atoi(argv[4]), server);
				break;
			}

			case FILE_OK:
			{
				state = file_ok(&output_file_fd, argv[2]);
				break;
			}

			case RECV_DATA:
			{
				state = recv_data(output_file_fd, server, &clientSeqNum);
				break;
			}

			case DONE:
			{
				printf("client done state\n");
				exit(EXIT_FAILURE);
				break;
			}

			default:
			{
				printf("Error - in default state\n");
				break;
			}
		}
	}
}

STATE start_state(char **argv, Connection * server, uint32_t * clientSeqNum)
{
	//return FILENAME if no error, otherwise DONE
	uint8_t packet[MAX_LEN];
	uint8_t buf[MAX_LEN];
	int fileNameLen = strlen(argv[1]) + 1;
	STATE returnValue = FILENAME;
	uint32_t bufferSize = 0;
	uint32_t windowSize = 0;

	//if connecected before, close b4 reconnect
	if (server->sk_num > 0)
		close(server->sk_num);

	if (udpClientSetup(argv[6], atoi(argv[7]), server) < 0)
		returnValue = DONE;
	else
	{
		//put in buffer size for sending and filename
		bufferSize = htonl(atoi(argv[4]));
		windowSize = htonl(atoi(argv[3]));
		memcpy(buf, &bufferSize, SIZE_OF_BUF_SIZE);
		memcpy(&buf[SIZE_OF_BUF_SIZE], &windowSize, SIZE_OF_BUF_SIZE);
		memcpy(&buf[SIZE_OF_BUF_SIZE * 2], argv[1], fileNameLen);
		printIPv6Info(&server->remote);
		send_buf(buf, fileNameLen + SIZE_OF_BUF_SIZE * 2, server, FNAME,
		*clientSeqNum, packet);
		(*clientSeqNum)++;

		returnValue = FILENAME;
	}

	return returnValue;
}

STATE filename(char * fname, int32_t buf_size, Connection * server)
{
	//send filename and get response
	int returnValue = START_STATE;
	uint8_t packet[MAX_LEN];
	uint8_t flag = 0;
	uint32_t seq_num = 0;
	int32_t recv_check = 0;
	static int retryCount = 0;

	if ((returnValue = processSelect(server, &retryCount, START_STATE, FILE_OK, DONE))
	== FILE_OK)
	{
		recv_check = recv_buf(packet, MAX_LEN, server->sk_num, server, &flag, &seq_num);
		
		printf("recving from server\n");
		//check bit flip
		if (recv_check == CRC_ERROR){
			returnValue = START_STATE;
		}
		else if (flag == FNAME_BAD)
		{
			printf("File %s not found\n", fname);
			returnValue = DONE;
		}
		else if (flag == DATA)
		{
			returnValue = FILE_OK;
		}
	}
	return returnValue;
}

STATE file_ok(int * outputFileFd, char* outputFileName)
{
	STATE returnValue = DONE;

	if ((*outputFileFd = open(outputFileName, O_CREAT | O_TRUNC | O_WRONLY, 0600)) < 0)
	{
		perror("File open error: ");
		returnValue = DONE;
	}
	else
	{
		returnValue = RECV_DATA;
	}

	return returnValue;
}

STATE recv_data(int32_t output_file, Connection * server, uint32_t * clientSeqNum)
{
	uint32_t seq_num = 0;
	uint32_t ackSeqNum = 0;
	uint8_t flag = 0;
	int32_t data_len = 0;
	uint8_t data_buf[MAX_LEN];
	//int i = 0;
	uint8_t packet[MAX_LEN];
	static int32_t expected_seq_num = START_SEQ_NUM;
	//buffEntry * bufEnt;

	if (select_call(server->sk_num, LONG_TIME, 0) == 0)
	{
		printf("Timeout after 10 seconds, server must be gone\n");
		return DONE;
	}

	data_len = recv_buf(data_buf, MAX_LEN, server->sk_num, server, &flag, &seq_num);

	if (data_len == CRC_ERROR)
		return RECV_DATA;

	addPdu(data_buf, seq_num, flag, data_len);

	if (flag == END_OF_FILE)
	{
		//send ack
		send_buf(packet, 1, server, EOF_ACK, *clientSeqNum, packet);
		(*clientSeqNum)++;
		printf("File done\n");
		close(output_file);
		return DONE;
	}
	else if (seq_num == expected_seq_num)
	{
		//send ack
		ackSeqNum = htonl(seq_num);
		send_buf((uint8_t *)&ackSeqNum, sizeof(ackSeqNum), server, ACK, *clientSeqNum,
		packet);
		write(output_file, &data_buf, data_len);
		(*clientSeqNum)++;
		expected_seq_num++;
	}

	/*
	else //(seq_num == expected_seq_num)
	{
		expected_seq_num++;
		write(output_file, &data_buf, data_len);
		for (i = 0; i < getWindowSize(); i++){
			bufEnt = getLowestPdu();
			if (bufEnt->seqNum == expected_seq_num){
        		write(output_file, &(bufEnt->pdu[7]), bufEnt->length);
				expected_seq_num++;
				processRR(expected_seq_num);
			}
    	}
	}
	*/
	else
	{
		ackSeqNum = htonl(expected_seq_num);
		send_buf((uint8_t *)&ackSeqNum, sizeof(ackSeqNum), server, SREJ, *clientSeqNum,
		packet);
	}

	return RECV_DATA;
}



/*int main (int argc, char *argv[])
 {
		// Supports 4 and 6 but requires IPv6 struct
	//double errorRate = 0;

	argStorage args;
	
	checkArgs(argc, argv, &args);
	printf("errorRate = %lf\n", args.error);
	setupPollSet();

	sendErr_init(args.error, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);

	processFile(&args);

	return 0;
}
*/

/*
void processFile(argStorage * args)
{
	int socketNum = 0;
	int seqNum = 0;
	int outputFileFd = 0;
	struct sockaddr_in6 server;	
	int state = START;

	//establish server connection	

	while (state != DONE)
	{
		switch(state)
		{
			case START:
			{
				state = connectionSetup(args, server);
				break;
			}

			case FILENAME:
			{
				break;
			}

			case FILE_OK:
			{
				break;
			}

			case RECV_DATA:
			{
				break;
			}

			case DONE:
			{
				printf("client done state\n");
				exit(EXIT_FAILURE);
				break;
			}
		}
	}

	//connectionSetup(args, &server);

	//talkToServer(socketNum, &server);
	
	close(socketNum);
}
*/

/*
int connectionSetup(argStorage * args, struct sockaddr_in6 server)
{
	//send startup packet
	//poll and wait for response
	//resend if you have to
	//recv ack
	int temp;
	int socketNum = 0;

	if (args->serverSocket > 0){
		close(args->serverSocket);
		removeFromPollSet(args->serverSocket);
	}

	if ((socketNum = (setupUdpClientToServer(&server, args->host, args->port))) < 0){
		return DONE;
	}

	args->serverSocket = socketNum;
	addToPollSet(args->serverSocket);
	temp = sendStartUpPacket(args, &server);
	if (temp == 1)
		return FILE_OK;
	if (temp == 2)
		return START;
	return DONE;
}
*/

int sendStartUpPacket(argStorage * args, struct sockaddr_in6 * server)
{
	uint8_t pdu[120];
	int pduLen;
	int count = 0;
	int serverAddrLen = sizeof(struct sockaddr_in6);
	int socketNum = 0;
	int flag;
	pduLen = createStartPacket(args, pdu);

	while(count < 10)
	{
		printf("Sending startup packet\n");
		sendtoErr(args->serverSocket, pdu, pduLen, 0, (struct sockaddr *) server, serverAddrLen);
		socketNum = pollCall(1000);
		if (socketNum == args->serverSocket){
			printf("recved\n");
			pduLen = safeRecvfrom(socketNum, pdu, MAXPDULEN, 0, (struct sockaddr *) server, &serverAddrLen);
			break;
		}
		else
			count += 1;
	}

	if (count > 9){
		return 0;
	}

	if (checkPacket(pdu, pduLen)){
		printf("Startup packet good\n");
		flag = getFlag(pdu, pduLen);
		if (flag == 8){
			printf("filename correct ready to transfer");
			return 1;
		}
		if (flag == 9){
			printf("File does not exist on server\n");
			return 0;
		}
	}

	else{
		printf("Bad packet\n");
		return 2;
	}

	return 0;
}

int createStartPacket(argStorage * args, uint8_t * pdu)
{
	uint8_t data[112];
	int pduLength;

	//filename length and filename
	int fileLen = strlen(args->fromFilename);
	//len
	memcpy(&data[0], &fileLen, 4);
	//filename
	memcpy(&data[4], &args->fromFilename, fileLen + 1);

	//file buffersize
	memcpy(&data[fileLen + 4 + 1], &args->bufferSize, 4);

	//file windowsize
	memcpy(&data[fileLen + 8 + 1], &args->windowSize, 4);

	pduLength = fileLen + 13;

	pduLength = createPDU(pdu, 0, 7, data, pduLength);

	return pduLength;
}

void talkToServer(int socketNum, struct sockaddr_in6 * server)
{
	uint8_t pdu[MAXPDULEN];
	int serverAddrLen = sizeof(struct sockaddr_in6);
	char * ipString = NULL;

	//read data from stdin variables
	int dataLen = 0; 
	char buffer[MAXBUF+1];

	//new pdu variables
	int pduLen = 0;
	
	int seqNum = 0;
	
	buffer[0] = '\0';
	while (buffer[0] != '.')
	{
		dataLen = readFromStdin(buffer);

		//create cool pdu
		pduLen = createPDU(pdu, seqNum, 3,(uint8_t *) buffer, dataLen);

		//increment seq number
		seqNum++;
	
		sendtoErr(socketNum, pdu, pduLen, 0, (struct sockaddr *) server, serverAddrLen);

		pduLen = 0;

		memset(&pdu[0], 0, MAXPDULEN);
		
		pduLen = safeRecvfrom(socketNum, pdu, MAXPDULEN, 0, (struct sockaddr *) server, &serverAddrLen);
		
		//receive ack from server
		if (pduLen > 0){
			ipString = ipAddressToString(server);
			printf("\nServer with ip: %s and port %d said it received\n", ipString, ntohs(server->sin6_port));
			outputPDU(pdu, pduLen);
		}
		memset(&pdu[0], 0, MAXPDULEN);
	}
}

int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("Enter data: ");
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

void printUsage()
{
	printf("usage: rcopy from-filename to-filename window-size buffer-size error-percent host-name port-number \n");
	exit(1);
}

void checkArgs(int argc, char * argv[])
{
	double temp = 0;

	//correct # args
	if (argc != 8)
		printUsage();

	//check fromFilename - 1
	if (strlen(argv[1]) > 100)
		printUsage();

	//check toFilename - 2
	if (strlen(argv[2]) > 100)
		printUsage();

	//check hostName - 6
	if (strlen(argv[3]) > 100)
		printUsage();

	//check windowSize
	temp = atoi(argv[3]);
	if ((temp > 1073741823) || temp < 1)
		printUsage();

	//check bufferSize
	temp = atoi(argv[4]);
	if ((temp > 1400) || temp < 1)
		printUsage();

	//check error percent
	temp = atof(argv[5]);
	if (!((temp >= 0) && (temp < 1)))
		printUsage();
}





