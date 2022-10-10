#ifndef __PDU_H__
#define __PDU_H__

#define MAXPDULEN 1407

void outputPDU(uint8_t * aPDU, int pduLength);

int createPDU(uint8_t * pduBuffer, uint32_t sequenceNumber, 
    uint8_t flag, uint8_t * payload, int payloadLen);

int checkPacket(uint8_t * pdu, int pduLen);

int getFlag(uint8_t * aPDU, int pduLength);

#endif