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
#include "checksum.h"

int createPDU(uint8_t * pduBuffer, uint32_t sequenceNumber, 
    uint8_t flag, uint8_t * payload, int payloadLen)
{
    int pduLength = payloadLen + 7;
    uint8_t cksum;

    //add sequence number to pdu
    sequenceNumber = htonl(sequenceNumber);
    memcpy(&pduBuffer[0], &sequenceNumber, 4);

    //zero out cksum
    //memset(&pduBuffer[4], 0, 2);
    pduBuffer[4] = 0;
    pduBuffer[5] = 0;

    //add flag number to pdu
    memcpy(&pduBuffer[6], &flag, 1);

    //add payload number to pdu
    memcpy(&pduBuffer[7], &payload[0], payloadLen);

    //calculate cksum of pdu
    cksum = in_cksum((short unsigned int*) &pduBuffer[0], pduLength);

    //add cksum to pdu                  
    memcpy(&pduBuffer[4], &cksum, 2); //cksum

    return pduLength;
}

void outputPDU(uint8_t * aPDU, int pduLength)
{
    uint32_t sequenceNumber = 0;
    uint8_t flag = 0;
    int payloadLen = pduLength - 7;
    char payload[1400];
    uint8_t cksum;

    //verify checksum
    cksum = in_cksum((short unsigned int*) aPDU, pduLength);
    if (cksum == 0)
        printf("\nChecksum verified (good packet!)\n");
    else
        printf("\nChecksum invalid (you suck!)\n");

    //get sequence number
    memcpy(&sequenceNumber, &aPDU[0], 4);
    sequenceNumber = ntohl(sequenceNumber);
    //printf("Sequence number = %d\n", sequenceNumber);

    //get flag
    memcpy(&flag, &aPDU[6], 1);
    //printf("Flag = %d\n", flag);

    //get payload
    memcpy(&payload[0], &aPDU[7], payloadLen);
    //printf("Payload = %s\n", payload);

    //payload length
    //printf("Payload length = %d\n", payloadLen);

    printf("Seq # = %d Flag = %d Payload length = %d\nPayload = %s\n\n",
        sequenceNumber, flag, payloadLen, payload);
}

int checkPacket(uint8_t * pdu, int pduLen)
{
    uint8_t cksum;
    cksum = in_cksum((short unsigned int*) pdu, pduLen);
    if (cksum == 0){
        printf("\nChecksum verified (good packet!)\n");
        return 1;
    }
    else{
        printf("\nChecksum invalid (you suck!)\n");
        return 0;
    }
}

int getFlag(uint8_t * aPDU, int pduLength)
{
    int flag;
    memcpy(&flag, &aPDU[6], 1);
    return flag;
}