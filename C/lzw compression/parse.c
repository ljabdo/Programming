#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "struct.h"
#include "functions.h"
#include "math.h"

Node makeNode()
{
   Node node;
   node.code = 0;
   node.status = 0;
   return node;
}

void reportError()
{
   perror(NULL);
   exit(EXIT_FAILURE);
}

void fileError(char *str)
{
   fprintf(stderr, "lzwCompress: %s: ", str);
   perror(NULL);
   exit(EXIT_FAILURE);
}

void createNodeArray(Storage *store)
{
   if ((store->array = (Node *) calloc(NUM_CODES, sizeof(Node))) == NULL)
      reportError();
}

int getNextLetter(Storage *store)
{
   int let;
   if((let = fgetc(store->file)) == EOF)
      store->eof = 1;
   return let;
}

void emptyFile(Storage* store)
{
   int newLet;
   newLet = getNextLetter(store);
   newLet ++;
   if (feof(store->file)){
      fclose(store->file);
      fclose(store->write);
      exit(EXIT_SUCCESS);
   }
   fseek(store->file, 0, SEEK_SET);
}

void fileOpen(Storage *store)
{
   FILE *file;
   if ((file = fopen(store->fName, "rb")) == NULL)
      fileError(store->fName);
   store->file = file;
}

void makeFirstCodes(Storage *store)
{
   int i;
   for (i = 0; i < 256; i++){
      store->array[store->index].code = i;
      store->array[0].next[i] = store->index;
      store->index ++;
   }
}

void recreateNodeArray(Storage* store)
{
   store->arraySize *= 2;
   if ((store->array = ((Node *) realloc(store->array, 
      store->arraySize * sizeof(Node)))) == NULL)
      reportError();
   memset(&(store->array[(store->arraySize)/2]), 0, 
      (store->arraySize/2) * sizeof(Node));
   /*printf("REALLOCATED ARRAY\n");*/
}

void recycleDict(Storage* store)
{
   /*finishWrite(store);*/
   memset(store->array, 0, store->arraySize * sizeof(Node));
   store->nextCode = START_CODE;
   store->index = 1;
   store->packet = 9;
   store->bits = 0;
   store->bitCount = 0;
   makeFirstCodes(store);
   /*printf("RECYCLE DICT\n");*/
}

int createCodeHelper(Storage* store, int newLet, int i)
{
   store->array[i].next[newLet] = store->index;
   store->array[store->index].code = store->nextCode;
   /*printf("%d ", store->array[i].code);*/
   writeCode(store->array[i].code, store);
   i = newLet + 1;
   store->nextCode++;
   store->index++;
   return i;
}

void createCode(Storage* store)
{
   int newLet, i;
   i = 0;
   while (store->eof == 0){
      if (store->nextCode > (pow((double)2, (double)store->recycleNum))){
         recycleDict(store);
         /*finishWrite(store);*/
         i = 0;
      }
      else
         newLet = getNextLetter(store);
      if (store->array[i].next[newLet] == 0)
         i = createCodeHelper(store, newLet, i);
      else
         i = store->array[i].next[newLet];
      if (store->index == store->arraySize)
         recreateNodeArray(store);
   }
   /*printf("DICTIONARY FINISHED\n");*/
}

void parseDict(Storage *store)
{
   createNodeArray(store);
   fileOpen(store);
   makeFirstCodes(store);
   openWriteFile(store);
   createCode(store);
   endFile(store);
   fclose(store->file);
   fclose(store->write);
   free(store->array);
}
