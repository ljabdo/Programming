#include "networks.h" 
#define MAXBUF 1024
#define HEADER_LEN 3

//receives ONLY length part of header BUFFER still has flag at beg
int recvPDU(int clientSocket, uint8_t * dataBuffer, int bufferLen)
{
   int reto = 0;
   uint16_t PDULength = 0;

   if ((reto = recv(clientSocket, dataBuffer, 2, MSG_WAITALL)) < 0){
      perror("Error on 1st RECV\n");
      exit(EXIT_FAILURE);
   }

   else if (reto == 0)
      return reto;

   memcpy(&PDULength, &dataBuffer[0], 2);
   PDULength = ntohs(PDULength);

   if (PDULength > bufferLen)
   {
      perror("PDULength is greater than bufferLen\n");
      exit(EXIT_FAILURE);
   }

   if ((reto = recv(clientSocket, dataBuffer, PDULength - 2, MSG_WAITALL)) < 0){
      perror("Error on 2nd RECV\n");
      exit(EXIT_FAILURE);
   }

   return reto; 
}

//sends pdu with HEADER
int sendPDU(int socketNumber, uint8_t * dataBuffer, int lengthOfData, uint8_t flag)
{
   uint8_t PDU[1024];
   uint16_t PDULength = lengthOfData + HEADER_LEN;
   int retoVal = 0;

   if (PDULength > MAXBUF)
   {
      perror("PDULength of max buff\n");
      exit(EXIT_FAILURE);
   }

   PDULength = htons(PDULength);

   memcpy(&PDU[0], &PDULength, 2);
   memcpy(&PDU[2], &flag, 1);
   memcpy(&PDU[3], dataBuffer, lengthOfData);

   retoVal = send(socketNumber, PDU, lengthOfData + 3, 0);

   if (retoVal < 0){
      perror("Error on sending data\n");
      exit(EXIT_FAILURE);
   }
   return retoVal;
}