#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "handleTable.h"
#include "safeUtil.h"


/*
typedef struct
{
    char handle[HANDLE_SIZE];
    int valid;
    int socketNumber;
} __attribute__((packed)) handleEntry;
*/


handleEntry * handleTable;
int numEntries = 0;
int tableSize = 0;
int tableIndex = 0;

void createHandleTable(int size)
{
    tableSize = size;
    handleTable = sCalloc(size, sizeof(handleEntry));
}

void addHandle(int socketNumber, char * handle, int handleLen)
{
    reallocMem();
    handleEntry newHandle;
    newHandle.socketNumber = socketNumber;
    memcpy(newHandle.handle, handle, handleLen);
    memcpy(&handleTable[socketNumber], &newHandle, sizeof(handleEntry));
    numEntries++;
    tableIndex++;
}

int getNumEntries()
{
    return numEntries;
}

int getTableSize()
{
    return tableSize;
}

handleEntry * getHandleOf(int index)
{
    return &(handleTable[index]);
}

void reallocMem()
{
    if (numEntries == tableSize)
    {
        tableSize += TABLE_GROW_SIZE;
        handleTable = srealloc(handleTable, tableSize);
    }
}

handleEntry * searchHandle(char * handle, int handleLen)
{
    int i;
    for (i = 0; i < tableSize; i++)
    {
        if (strcmp(handle, handleTable[i].handle) == 0)
            return &handleTable[i];
    }
    return NULL;
}

int validHandle(char * handle, int handleLen)
{
    int i;
    for (i = 0; i < tableSize; i++)
    {   
        if (strcmp(handle, handleTable[i].handle) == 0)
            return 0; 
    }
    return 1;
}

int removeHandle(int socket)
{

    if (handleTable[socket].socketNumber == socket){
        memset(&handleTable[socket], EMPTY, sizeof(handleEntry));
        numEntries--;
        printf("Removing %s\n", handleTable[socket].handle);
        return 0;
    }

    return 1;
}

void printHandleTable()
{
    int i;
    for (i = 0; i < tableSize; i++)
    {
        if (handleTable[i].socketNumber != 0){
            printf("%s\n", handleTable[i].handle);
        }
    }
}
