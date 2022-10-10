#ifndef __STRUCT_H__
#define ____STRUCT_H__

typedef struct
{
    char fromFilename[101];
    char toFilename[101];
    char host[101];
    int windowSize;
    int bufferSize;
    double error;
    int port;
    int serverSocket;
}__attribute__((packed)) argStorage;

#endif