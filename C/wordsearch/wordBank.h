#ifndef WORDBANK_H
#define WORDBANK_H
#include <stdlib.h>
#include <stdio.h>
#include "hashTable.h"

typedef struct
{
   int w, l, c;
   int words, bytes, newL;
   int totW, totB, totNL;
   int errorFlag, fileFlag;
   FILE *currentFile;
   HashTable *ht;
} fileInfo;

typedef struct
{
   int printFlag, wordState;
} printer;

fileInfo makeFileInfo();
printer makePrinter();

#endif
