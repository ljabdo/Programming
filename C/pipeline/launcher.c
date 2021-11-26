#include "launcher.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#define LENGTH 2

void reportError()
{
   perror(NULL);
   exit(EXIT_FAILURE);
}

void waitStatus(pipeStorage* ps, pid_t pid)
{
   int i, status;
   for (i = 0; i < ps->numPro; i++)
      if ((pid = wait(&status)) < 0)
         reportError();
}

void startChildren(int fd1[], int fd2[], int i, pipeStorage* ps)
{
   /*char read[LENGTH], write[LENGTH];*/
   int lastPro = ps->numPro;
   if (i == 1){
      sprintf(ps->read, "%d", 0);
      sprintf(ps->write, "%d", fd1[1]);
      close(fd1[0]);
   }
   else if (i == lastPro){
      sprintf(ps->read, "%d", fd1[0]);
      sprintf(ps->write, "%d", 1);
      close(fd2[0]);
      close(fd2[1]);
   }
   else{
      sprintf(ps->read, "%d", fd1[0]);
      sprintf(ps->write, "%d", fd2[1]);
      close(fd2[0]);
   }
   /*printf("read fd = %s    write fd = %s\n", ps->read, ps->write);*/
   execl(ps->path, ps->path, "-i", ps->read, "-o", ps->write, NULL);
   reportError();
}

void closeReplace(int fd1[], int fd2[])
{
   close(fd1[0]);
   close(fd2[1]);
   fd1[0] = fd2[0];
   fd1[1] = fd2[1];
}

void pipeHelper(int fd1[], int fd2[], int i, pipeStorage* ps, pid_t pid)
{
   if (i == 1){
      if (pipe(fd1) < 0)
         reportError();
   }
   else{
      close(fd1[1]);
      if (pipe(fd2) < 0)
         reportError();
   }
   if (-1 == (pid = fork()))
      reportError();
   else if (pid == 0)
      startChildren(fd1, fd2, i, ps);
   else if (i != 1)
      closeReplace(fd1, fd2);
}

int startPipe(pipeStorage* ps)
{
   int fd1[2], fd2[2], i;
   pid_t pid = 0; 
   /* READ FD pipes[0] WRITE FD pipes[1]*/
   for (i = 1; i < ps->numPro + 1; i++)
      pipeHelper(fd1, fd2, i, ps, pid);
   close(fd2[0]);
   waitStatus(ps, pid);
   return 0;
}
