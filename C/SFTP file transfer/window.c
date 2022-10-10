#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

#include "window.h"
#include "safeUtil.h"

int upper;
int lower;
int current;
int windowSize;
buffEntry * windowTable;

int getUpper()
{
    return upper;
}

int getLower()
{
    return lower;
}

int getCurrent()
{
    return current;
}

int getWindowSize()
{
    return windowSize;
}

int checkEmpty()
{
    if (upper - lower < windowSize)
        return 1;
    return 0;
}

void createWindowTable(int winSize)
{
    windowSize = winSize;
    windowTable = sCalloc(windowSize, sizeof(buffEntry));
    lower = 0;
    current = 0;
    upper = lower + windowSize;
}

void addPdu(uint8_t * pduBuffer, uint32_t sequenceNumber, 
    uint8_t flag, int pduLen)
{
    int index;
    index = current % windowSize;
    memcpy(&(windowTable[index].pdu), &pduBuffer, pduLen);
    //windowTable[current] = pduBuffer; imight work prob not
    windowTable[index].length = pduLen;
    windowTable[index].valid = 1;
    windowTable[index].seqNum = sequenceNumber;
    windowTable[index].flag = flag;
    current ++;
}

buffEntry * getPdu(int sequenceNumber)
{
    int i;
    for (i = 0; i < windowSize; i++)
    {
        if (windowTable[i].seqNum == sequenceNumber){
            return &windowTable[i];
            //memset(&windowTable[i], 0, MAXPDULEN);
        }
    }
    printf("Could not find pdu seqnum = %d\n", sequenceNumber);
    return NULL;
}

buffEntry * getLowestPdu()
{
    buffEntry * cur;
    buffEntry * low;
    int i;
    for (i = 0; i < windowSize; i++){
        if (windowTable[i].valid == 1){
            low = &windowTable[i];
            break;
        }
    }
    for (i = 0; i < windowSize; i++)
    {
        if (windowTable[i].valid == 1)
            cur = &windowTable[i];
        if (cur->seqNum < low->seqNum){
            low = &windowTable[i];
        }
    }
    return low;
}

void processRR(int rrNum)
{
    int i;
    for (i = 0; i < windowSize; i++)
    {
        if (windowTable[i].seqNum < rrNum){
            memset(&windowTable[i], 0, MAXPDULEN);
        }
    }
    lower = rrNum;
    upper = rrNum + windowSize;
}

int checkStatus()
{
    if (current == upper)
        return 0;
    else
        return 1;
}

void printWindowData()
{
    printf("Server Window - Window Size: %d, Lower: %d,"
    " Upper: %d, Current: %d Window open?: %d\n"
    , windowSize, lower, upper, current, checkStatus());   
}

void printWindowTable()
{
    int i;
    printf("Window size is: %d\n", windowSize);
    for (i = 0; i < windowSize; i++)
    {
        printf("   %d ", i);
        if (windowTable[i].valid == 0)
            printf("not valid\n");
        if (windowTable[i].valid == 1)
            printf("sequenceNumber: %d pduSize: %d\n",
            windowTable[i].seqNum, windowTable[i].length);
    }
}

