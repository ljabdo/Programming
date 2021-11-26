#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "struct.h"
#include "launcher.h"
#define MAXCHAR 1024

int startPipe(Parser* ps);

Parser makeParse()
{
   Parser parse;
   int zero = 0;
   parse.numComms = zero;
   parse.pipeFlag = zero;
   parse.directFlag = zero;
   parse.numPipes = zero;
   parse.RDinFlag = zero;
   parse.RDoutFlag = zero;
   parse.FDin = zero;
   parse.FDout = zero;
   return parse;
}

pipeStorage makePStorage()
{
   int y = 0;
   pipeStorage ps;
   ps.numPro = y;
   ps.pFlag = y;
   return ps;
}

Command makeCommand()
{
   Command comm;
   int zero = 0;
   comm.argc = zero;
   comm.pipe = zero;
   return comm;
}

void fillArray(Parser *parse, char *arr)
{
   int i, c;
   i = 0;
   printf(":-) ");
   while (i < MAXCHAR){
      c = getchar();
      if ((c == '\n')){
         fflush(stdin);
         arr[i] = '\0';
         return;
      }
      else if (c == EOF){
         printf("exit\n");
         exit(EXIT_SUCCESS);
      }
      arr[i] = c;
      i++;
   }
}

int getToken(char *arr, char **tokens, Parser *parse)
{
   char *str;
   int i;
   char space[2] = " ";
   i = 0;
   str = strtok(arr, space);
   while (str != NULL){
      tokens[i] = str;
      /*printf("%s\n", str);*/
      str = strtok(NULL, space);
      i++;
   }
   return i;
}

void syntaxError()
{ 
   fprintf(stderr, "cshell: Syntax error\n");
}

void fillCommand(Parser *parse, char** tokens, Command* comm, int i)
{
   comm->args[comm->argc] = tokens[i];
   comm->argc++;
   comm->path = tokens[i];
   comm->pathSet = 1;
}

int parseToken(Parser *parse, char** tokens, Command* comm, int i)
{
   if (!strcmp(tokens[i], "<")){
      parse->RDinFlag = 1;
      parse->RDin = tokens[i+1];
      if (!tokens[i+1])
         return 3;
      return 4;
   }
   else if (!strcmp(tokens[i], ">")){
      parse->RDoutFlag = 1;
      parse->RDout = tokens[i+1];
      if (!tokens[i+1])
         return 3;
      return 4;
   }
   else if (!strcmp(tokens[i], "|")){
      comm->pipe = 1;
      parse->numPipes += 1;
      parse->pipeFlag = 1;
      return 1;
   }
   else if ((tokens[i][0] == '-') || (comm->pathSet == 1)){
      if (i > 0 && (!strcmp(tokens[i-1], "<") || !strcmp(tokens[i-1], ">")))
         return 0;
      if (comm->pathSet == 0){
         comm->path = tokens[i];
         return 2;
      }
      comm->args[comm->argc] = tokens[i];
      comm->argc++;
   }
   else
      fillCommand(parse, tokens, comm, i);
   return 0;
}

void commandCountError(char *file)
{
   fprintf(stderr, "cshell: %s: Too many arguments\n", file);
}

void reportNoFile(char* file)
{
   fprintf(stderr, "cshell: %s: No such file or directory\n", file);
}

int parseTokenHelper(Parser *parse, char** tokens, int tokenNum, int i)
{
   int y, count;
   Command comm = makeCommand();
   y = 0;
   count = 0;
   while((i < tokenNum) && ((y == 0) || (y == 4))){
      y = parseToken(parse, tokens, &comm, i);
      if (comm.argc > 10){
         commandCountError(comm.path);
         return 0;
      }
      if (y == 4)
         i++;
      count++;
      i++;
   }
   /*comm.args[comm.argc] = NULL;*/
   if (y == 2){
      reportNoFile(comm.path);
      return 0;
   }
   if (y == 3){
      syntaxError();
      return 0;
   }
   if (!strcmp(comm.path, "exit"))
      exit(EXIT_SUCCESS);
   parse->comms[parse->numComms] = comm;
   parse->numComms++;
   return count;
}

int parseTokenLoop(Parser *parse, char** tokens, int tokenNum)
{
   int reto, i;
   reto = 0;
   i = 0;
   /*printf("tokennum %d\n", tokenNum);*/
   do{
      reto = parseTokenHelper(parse, tokens, tokenNum, i);
      /*printf("Pipe number %d\n", i);*/
      i+=reto;
   } while (reto != 0 && ((parse->comms[parse->numComms - 1]).pipe) == 1);
   if (reto == 0)
      return 0;
   return 1;
}

void parseShell()
{
   while (1){
      int token;
      Parser parse;
      char arr[MAXCHAR];
      char *tokens[200] = {NULL};
      parse = makeParse();
      fillArray(&parse, arr);
      /*printf("string = %s\n", arr);*/
      token = getToken(arr, tokens, &parse);
      if (parseTokenLoop(&parse, tokens, token))
         startPipe(&parse);
      parseShell();
   }
}

int main(int argc, char *argv[])
{
   setbuf(stdout, NULL);
   parseShell();
   return 0;
}
