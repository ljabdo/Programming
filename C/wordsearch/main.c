#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "wordBank.h"
#include "proto.h"

void printResults(fileInfo *fileS)
{
   if (fileS->w == 0 && fileS->l == 0 && fileS->c == 0){
      fileS->w = 1;
      fileS->l = 1;
      fileS->c = 1;
   }

   if (fileS->l)
      printf("%10u ", fileS->newL);
   if (fileS->w)
      printf("%10u ", fileS->words);
   if (fileS->c)
      printf("%10u ", fileS->bytes);
}

void printSTD(fileInfo *fileS)
{
   if (fileS->w == 0 && fileS->l == 0 && fileS->c == 0){
      fileS->w = 1;
      fileS->l = 1;
      fileS->c = 1;
   }

   if (fileS->l)
      printf("%10u", fileS->newL);
   if (fileS->w)
      printf(" %10u", fileS->words);
   if (fileS->c)
      printf(" %10u", fileS->bytes);
}

void printTotals(fileInfo *FS)
{
   if (FS->fileFlag <= 1
      eturn;
   if (FS->l)
      printf("%10u ", FS->totNL);
   if (FS->w)
      printf("%10u ", FS->totW);
   if (FS->c)
      printf("%10u ", FS->totB);
   printf("total\n");
}

void setTotals(fileInfo *fileStore)
{
   fileStore->totW += fileStore->words;
   fileStore->totB += fileStore->bytes;
   fileStore->totNL += fileStore->newL;
}

void resetFileStore(fileInfo *fileStore)
{
   fileStore->words = 0;
   fileStore->bytes = 0;
   fileStore->newL = 0;
}

void helpRunFiles(char str[], fileInfo *fileStore)
{
   if (tryOpen(fileStore, str)){
      countAll(fileStore);
      setTotals(fileStore);
      printResults(fileStore);
      printf("%s\n", str);
   }
}

int runFiles(int argc, char *argv[], fileInfo *fileStore)
{
   int i, fileFlag;
   for (i = 1; i < argc; i++){
      if (validateFile(argv[i])){
         helpRunFiles(argv[i], fileStore);
         fileStore->fileFlag ++;
         fileFlag = 1;
         resetFileStore(fileStore);
      }
   }
   printTotals(fileStore);
   return fileFlag;
}

void handleSTD(fileInfo *fileStore)
{
   fileStore->currentFile = stdin;
   countAll(fileStore);
   printSTD(fileStore);
   printf("\n");
}

int main(int argc, char *argv[])
{
   fileInfo fileStore = makeFileInfo();
   grabCommandFlags(argc, argv, &fileStore);
   if (!runFiles(argc, argv, &fileStore))
      handleSTD(&fileStore);
   if (fileStore.errorFlag)
      return 1;
   return 0;
}
