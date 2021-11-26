#include "launcher.h"
#include "struct.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#define LENGTH 2

void reportError()
{
   perror(NULL);
   exit(EXIT_FAILURE);
}

int tryOpen(char* str)
{
   int fd;
   if ((fd = open(str, O_RDONLY)) < 0){
      fprintf(stderr, "cshell: %s: ", str);
      perror(NULL);
      return -1;
   }
   return fd;
}

int openOut(char* str)
{ 
   int fd;
   if ((fd = open(str, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0){
      fprintf(stderr, "cshell: %s: ", str);
      perror(NULL);
      exit(EXIT_FAILURE);
   }
   return fd;
}

void waitStatus(Parser *parse, pid_t pid)
{
   int i, status;
   for (i = 0; i < parse->numPipes + 1; i++)
      if ((pid = wait(&status)) < 0)
         reportError();
}

void childHelper(int fd1[], int fd2[], int i, Parser *parse)
{
   if (parse->RDoutFlag){
      parse->FDout = (openOut(parse->RDout));
      fd1[0] = parse->FDout;
      dup2(fd1[0], STDOUT_FILENO);
   }
   dup2(fd1[0], STDIN_FILENO);
   close(fd2[0]);
   close(fd2[1]);
}

void startChildren(int fd1[], int fd2[], int i, Parser *parse)
{
   if (i == 1){
      if (parse->RDinFlag){
         fd1[0] = parse->FDin;
         dup2(fd1[0], STDIN_FILENO);
      }
      else
         close(fd1[0]);
      if ((parse->pipeFlag == 0) && parse->RDoutFlag){
         parse->FDout = (openOut(parse->RDout));
         fd1[1] = parse->FDout;
         dup2(fd1[1], STDOUT_FILENO);
      }
      else if (parse->pipeFlag)
         dup2(fd1[1], STDOUT_FILENO);
   }
   else if (i == parse->numPipes + 1)
      childHelper(fd1, fd2, i, parse);
   else{
      close(fd2[0]);
      dup2(fd1[0], STDIN_FILENO);
      dup2(fd2[1], STDOUT_FILENO);
   }
   execvp(parse->comms[i-1].path, parse->comms[i-1].args);
   fprintf(stderr, "cshell: %s: ", parse->comms[i-1].path);
   reportError();
}

void closeReplace(int fd1[], int fd2[])
{
   close(fd1[0]);
   close(fd2[1]);
   fd1[0] = fd2[0];
   fd1[1] = fd2[1];
}

void pipeHelper(int fd1[], int fd2[], int i, pid_t pid, Parser *parse)
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
      startChildren(fd1, fd2, i, parse);
   else if (i != 1)
      closeReplace(fd1, fd2);
}

int startPipe(Parser *parse)
{
   int fd1[2], fd2[2], i, FDin;
   pid_t pid = 0; 
   /*READ FD pipes[0] WRITE FD pipes[1]*/
   if (parse->numPipes >= parse->numComms)
      fprintf(stderr, "cshell: Invalid pipe");
   if (parse->RDinFlag && (FDin = tryOpen(parse->RDin)) == -1)
      return 0;
   else
      parse->FDin = FDin;
   for (i = 1; i < parse->numPipes + 2; i++)
      pipeHelper(fd1, fd2, i, pid, parse);
   close(fd2[0]);
   waitStatus(parse, pid);
   return 0;
}
