#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "wordBank.h"

void grabCommandFlagLoop(char checkStr[], fileInfo *currentFile)
{
   if (!(strcmp(checkStr, "-w")))
      currentFile->w = 1;
   else if(!(strcmp(checkStr, "-l")))
      currentFile->l = 1;
   else if (!(strcmp(checkStr, "-c")))
      currentFile->c = 1;
   else if(checkStr[0] == '-'){
      fprintf(stderr, "swc: invalid option: '%s'\n", checkStr);
      fprintf(stderr, "Usage: swc [-l|-w|-c]... [file]...\n");
      exit(EXIT_FAILURE);
   }
}

void grabCommandFlags(int argc, char *argv[], fileInfo *fileStore)
{
   int i;
   for (i = 1; i < argc; i++)
      grabCommandFlagLoop(argv[i], fileStore);
}

int validateFile(char *str)
{
   if ((strcmp(str, "-w") && strcmp(str, "-l")
      && strcmp(str, "-c")))
      return 1;
   return 0;
}
