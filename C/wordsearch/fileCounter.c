#include "wordBank.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "wordBank.h"

fileInfo makeFileInfo()
{
   fileInfo newFile;
   int words, bytes, newL, w, l, c;
   words = 0; 
   bytes = 0; 
   newL = 0; 
   w = 0; 
   l = 0; 
   c = 0;
   newFile.words = words;
   newFile.bytes = bytes;
   newFile.newL = newL;
   newFile.w = w;
   newFile.c = c;
   newFile.l = l;
   newFile.fileFlag = 0;
   newFile.errorFlag = 0;
   return newFile;
}

printer makePrinter()
{
   printer S;
   int printFlag, wordState;
   printFlag = 0; 
   wordState = 0;
   S.printFlag = printFlag;
   S.wordState = wordState;
   return S;
}

int tryOpen(fileInfo *fileStore, char *str)
{
   FILE *tryFile;
   tryFile = fopen(str, "rb");
   if (tryFile == NULL){
      fprintf(stderr ,"swc: ");
      perror(str);
      fileStore->errorFlag = 1;
      return 0;
   }
   fileStore->currentFile = tryFile;
   return 1;
}

void helpCountAll(fileInfo *fileStore, printer *print, int nChar)
{
   if (nChar == '\n')
      fileStore->newL ++;
   if (isprint(nChar) && nChar != ' ')
      print->printFlag = 1;
   if (!isspace(nChar))
      print->wordState = 1;
   else if (print->printFlag == 1 && print->wordState == 1){
      fileStore->words ++;
      print->printFlag = 0;
      print->wordState = 0;
   }
   fileStore->bytes ++;
}

void countAll(fileInfo *fileStore)
{
   printer print;
   printer *PT = &print;
   int nChar;
   while ((nChar = fgetc(fileStore->currentFile)) != EOF)
      helpCountAll(fileStore, PT, nChar);
   if (nChar == EOF && print.printFlag == 1 && print.wordState == 1)
      fileStore->words ++;
}
