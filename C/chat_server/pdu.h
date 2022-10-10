#ifndef __PDU_H__
#define __PDU_H__

int recvPDU(int clientSocket, uint8_t * dataBuffer, int bufferLen);

int sendPDU(int socketNumber, uint8_t * dataBuffer, int lengthOfData, uint8_t flag);

#endif
