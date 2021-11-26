#include <stdlib.h>
#include <stdio.h>
#include "scoreKeep.h"
#include "proto.h"
#include <string.h>

#define BOARDS 8

void generateKey(int size, char maxL, char board[])
{
   int i, c;
   int letDiff = maxL - 'A' + 1;
   for(i = 0; i < size; i++){
      c = rand();
      c = ((c % letDiff) + 'A'); 
      board[i] = (char) c;
   }
}

int checkAnswer(char boardG[], char answerK[], int numP)
{
   char cpyArray[BOARDS];
   int exact = 0, ret = 0, nonEx = 0;
   strcpy(cpyArray, answerK);
   ret = checkExact(numP, boardG, answerK, cpyArray, exact);
   nonEx = checkLoop(boardG, cpyArray, numP);
   if(ret == numP)
      return 1;
   printf("Nope, %d exact guesses and %d inexact guesses\n", ret, nonEx);
   return 0;
}

int checkExact(int numP, char boardG[], char answerK[], char cpy[], int exact)
{
   int i;
   for (i = 0; i < numP; i++){
      if (boardG[i] == answerK[i]){
         cpy[i] = 0;
         boardG[i] = 1;
         exact += 1;
      } 
   }
   return exact;
}

int checkLoop(char boardG[], char cpyArray[], int numP)
{
   int p, inexactly;
   inexactly = 0;
   for (p = 0; p < numP; p++)
      inexactly = helperLoop(boardG, cpyArray, numP, inexactly, p);
   p = inexactly;
   inexactly = 0;
   return p;
}

int helperLoop(char boardG[], char cpyArray[], int numP,int inexactly, int p)
{
   int i;
   for(i = 0; i < numP; i++)
   {
      if(cpyArray[p] == boardG[i]){
         boardG[i] = 1;
         cpyArray[p] = 0;
         inexactly++;
      }
   }
   return inexactly;
}
