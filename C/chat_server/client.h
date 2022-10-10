#ifndef __CLIENT_H__
#define __CLIENT_H__

void sendToServer(int socketNum);
int readFromStdin(char * buffer);
void checkArgs(int argc, char * argv[]);
int recvFromServer(int clientSocket);
void parseCommand(int serverSocket, char * selfHandle);
void parseMessagePacket(int serverSocket, char * sendBuf, int sendLen, char * selfHandle);
void printMsgUsage();
void sendMessagePacket(char handles[9][100], char * msg, int serverSocket, char *selfHandle, int handleCount);
void recvFlag5(int serverSocket, uint8_t * buf, int messageLen);
void parseBroadcastPacket(int serverSocket, char * sendBuf, int sendLen, char * selfHandle);
void sendBroadcastPacket(char * token, int serverSocket, char * selfHandle);
void recvFlag4(int serverSocket, uint8_t * buf, int messageLen);

#endif