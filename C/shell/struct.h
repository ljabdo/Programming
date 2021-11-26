#ifndef STRUCT_H
#define STRUCT_H

typedef struct
{
   int argc;
   char *args[12];
   int pathSet;
   int pipe;
   char *path;
   char *RDin;
   char *RDout;
} Command;

typedef struct
{
   int numComms;
   int pipeFlag;
   int directFlag;
   int numPipes;
   char *RDin;
   char *RDout;
   int RDinFlag;
   int RDoutFlag;
   int FDin;
   int FDout;
   Command comms[20];
}  Parser;

Parser makeParse();
Command makeCommand();
void startExec(Parser *parse);
#endif
