#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "launcher.h"

pipeStorage makePStorage()
{
   int y = 0;
   pipeStorage ps;
   ps.numPro = y;
   ps.pFlag = y;
   return ps;
}

void printUsage()
{  
   fprintf(stderr, "Usage: pipeline path -p processes\n");
   fprintf(stderr, "Where: path is some \"random child\" and processes is 2");
   fprintf(stderr, " to 10, inclusive\n");
   exit(EXIT_FAILURE);
}

void argInt(int argc, char *argv[], int i, pipeStorage* ps)
{ 
   if ((argc - i) == 1)
      printUsage();
   else if (memcmp(argv[i + 1], "10", 2) == 0)
      ps->numPro = 10;
   else if (isdigit(argv[i + 1][0]) && (argv[i+1][0] > 49 && argv[i+1][0] < 58))
      ps->numPro = (argv[i+1][0] - '0');
   else 
      printUsage();
}

void getArgs(int argc, char *argv[], pipeStorage* ps)
{
   int i;
   for (i = 1; i < argc; i++){
      if (strcmp(argv[i], "-p") == 0){
         argInt(argc, argv, i, ps);
         ps->pFlag = 1;
         i++;
      }
      else 
         ps->path = argv[i];
   }
}

void checkArgs(int argc, char *argv[], pipeStorage* ps)
{
   if (argc != 4)
      printUsage();
   getArgs(argc, argv, ps);
   if (ps->pFlag == 0)
      printUsage();
}

int main(int argc, char *argv[])
{
   pipeStorage ps = makePStorage();
   checkArgs(argc, argv, &ps);
   startPipe(&ps);
   return 0;
}
