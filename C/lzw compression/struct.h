#ifndef STRUCT_H
#define STRUCT_H
#define NUMBER_OF_SYMBOLS 256
#define NUM_CODES 1000

typedef unsigned short Code;

typedef struct node
{
   Code code;
   int status;
   short next[NUMBER_OF_SYMBOLS];
   /*struct node *next;*/
} Node;

typedef struct
{
   int recycleNum;
   int index;
   int arraySize;
   int eof;
   int nextCode:
   int packet;
   unsigned bits, bitCount;
   Node *array;
   char *fName;
   FILE *file;
   FILE *write;
} Storage;

Storage makeStorage();
Node makeNode();
#endif
