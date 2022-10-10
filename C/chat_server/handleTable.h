#ifndef __HANDLETABLE_H__
#define __HANDLETABLE_H__


#define HANDLE_SIZE 100
#define TABLE_GROW_SIZE 10
#define EMPTY 0

typedef struct
{
    char handle[HANDLE_SIZE];
    int valid;
    int socketNumber;
} __attribute__((packed)) handleEntry;

int removeHandle(int socket);

void reallocMem();

void addHandle(int socketNumber, char * handle, int handleLen);

void createHandleTable(int size);

handleEntry * searchHandle(char * handle, int handleLen);

int validHandle(char * handle, int handleLen);

void printHandleTable();

int getNumEntries();

handleEntry * getHandleOf(int index);

int getTableSize();

#endif