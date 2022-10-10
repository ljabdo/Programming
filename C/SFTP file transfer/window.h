#ifndef __WINDOW_H__
#define ____WINDOW_H__

#define MAXPDULEN 1407

typedef struct
{
    uint8_t valid;
    uint16_t length;
    uint8_t flag;
    int seqNum;
    uint8_t pdu[MAXPDULEN];
}__attribute__((packed)) buffEntry;

void printWindowTable();

void printWindowData();

void createWindowTable(int winSize);

void addPdu(uint8_t * pduBuffer, uint32_t sequenceNumber, uint8_t flag, int pduLen);

buffEntry * getPdu(int sequenceNumber);

void processRR(int rrNum);

int checkStatus();

buffEntry * getLowestPdu();

int getUpper();

int getLower();

int getCurrent();

int getWindowSize();

int checkEmpty();

#endif