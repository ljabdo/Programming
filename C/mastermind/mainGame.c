#include "scoreKeep.h"
#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int checkArgs(char *argv[])
{
   if (!(strcmp(argv[1], "-show")))
      return 1;
   fprintf(stderr, "Usage: mastermind [-show]\n");
   exit(EXIT_FAILURE);
   return 0;
}

void printBoard(char board [], int numP)
{
   int i;
   printf("Initialized Game Board: ");
   for (i = 0; i < numP; i++)
      printf("%c", board[i]);
   printf("\n");
}

int arguements(char *argv[],int argc)
{
   if (argc > 1)
      if (checkArgs(argv))
         return 1;
   return 0;
}

int runGuessesMain(Keeper GK)
{
   int win, currentG = 1;
   while (currentG < GK.numG + 1){
      win = runGuessesHelper(GK, currentG);
      if (win)
         return 1;
      currentG++;
   }
   return 0;
}

int runGuessesHelper(Keeper GK, int currentG)
{
   int win;
   if(getGuess(GK.numP, GK.maxL, currentG, GK.currentG)){
      win = checkAnswer(GK.currentG, GK.answerK, GK.numP);
      if (win){
         printf("\nWow, you won in %d guesses - "
            "well done!\n", currentG);
         return 1;
      }
   }
   return 0;
}

int main(int argc, char *argv[])
{
   int showFlag = 0;
   Keeper GK;
   showFlag = arguements(argv, argc);
   GK = makeGameKeeper();
   generateKey(GK.numP, GK.maxL, GK.answerK);
   if (showFlag)
      printBoard(GK.answerK, GK.numP);
   if (runGuessesMain(GK))
      return 0;
   printf("\nGame over, you ran out of guesses. Better luck next time!\n");
   return 0;
}
