#ifndef LAUNCHER_H
#define LAUNCHER_H

typedef struct
{
   int numPro, pFlag;
   char *path;
   char read[2], write[2];
} pipeStorage;

pipeStorage makePStorage();
int startPipe(pipeStorage* ps);
#endif
