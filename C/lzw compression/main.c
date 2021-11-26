#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "struct.h"
#include "functions.h"

Storage makeStorage()
{
   Storage store;
   store.recycleNum = 0;
   store.index = 1;
   store.eof = 0;
   store.bits = 0;
   store.bitCount = 0;
   store.packet = 9;
   store.nextCode = START_CODE;
   store.arraySize = NUM_CODES;
   return store;
}

void printUsage()
{
   fprintf(stderr, "Usage: lzwCompress [-rN] file\n");
   fprintf(stderr, "Where: N is a number from 9 to 15 specifying the recycle"
      " code as a power of 2\n");
   exit(EXIT_FAILURE);
}

void fillStorage(Storage *store, char *argv[], int i)
{ 
   char newStr[10]; 
   if (!memcmp("-r", argv[i], 2)){
      strcpy(newStr, &(argv[i][2]));
      sscanf(newStr, "%d", &(store->recycleNum));
   }
   else if (argv[i] != NULL)
      store->fName = argv[i];
   else
      printUsage();
}

void checkFlagFile(Storage *store, char *argv[], int argc)
{
   int i;
   for (i = 1; i < argc; i++)
      fillStorage(store, argv, i);
   if (store->recycleNum < 9 || store->recycleNum > 15)
      printUsage();
}

void fillFlagFile(Storage *store, char *str)
{
   store->fName = str;
   store->recycleNum = DEFAULT_R;
}

void checkArgs(char *argv[], int argc, Storage* store)
{ 
   if ((argc == 1) || (argc > 3))
      printUsage();
   if (argc == 2)
      fillFlagFile(store, argv[1]);
   checkFlagFile(store, argv, argc);
}

int main(int argc, char *argv[])
{
   Storage store = makeStorage();
   checkArgs(argv, argc, &store);
   parseDict(&store);
   return 0;
}
