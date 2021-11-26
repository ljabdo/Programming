#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "scoreKeep.h"
#include "proto.h"

#define BOARDSIZE 8

Keeper makeGameKeeper()
{
   Keeper gameKeeper;
   int seed = getSeed();
   char maxL = getMaxLetter();
   int numP = getNumPositions();
   int numG = getNumGuesses();
   gameKeeper.seed = seed;
   gameKeeper.maxL = maxL;
   gameKeeper.numP = numP;
   gameKeeper.numG = numG;
   return gameKeeper;
}

double getSeed()
{
   int seed = 0;
   while(seed == 0)
      seed = getSeedLoop(seed);
   srand(seed);
   return seed;
}

double getSeedLoop(int seed)
{
   int ret;
   printf("Enter the seed for random number generation: ");
   ret = scanf("%d", &seed);
   reportError(ret);
   flushLine();
   if (ret == 1)
      return seed;
   printf("The seed must be an integer value, please try again\n");
   return 0;
}

int getGuess(int numP, char maxL, int guessNum, char board[])
{
   int check;
   while (1){
      check = 0;
      printf("\nEnter guess %d: ", guessNum); 
      fillBoard(numP, maxL, board);
      check = checkGuess(board, numP, maxL);
      flushLine();
      if (check)
         return 1;
      printf("Invalid guess, please try again\n");
   }
}

void fillBoard(int numP, char maxL, char board[])
{
   char nextL;
   int ret = 0;
   int i = 0;
   while(i < numP){
      ret = scanf(" %c", &nextL);
      reportError(ret);
      if (ret == 1 && nextL != '\n'){
         board[i] = nextL;
         i++;
      }
   }
}

int checkGuess(char board[], int numP, char maxL)
{
   int i;
   for(i = 0; i < numP; i++)
      if(!(isupper(board[i]) && board[i] >= 'A' && board[i] <= maxL))
         return 0;
   return 1;
}

char getMaxLetter()
{
   char maxL = 0;
   while(maxL == 0)
      maxL = getMaxLetterLoop();
   return maxL;
}

char getMaxLetterLoop()
{
   char maxL;
   int ret = 0;
   printf("Enter the maximum letter for the game (A-Z): ");
   ret = scanf(" %c", &maxL);
   flushLine();
   reportError(ret);
   if (ret == 1 && isupper(maxL))
      return maxL;
   printf("The letter must be an uppercase A-Z, please try again\n");
   return 0;
}

int getNumPositions()
{
   int ret, numP;
   while(1){
      printf("Enter the number of positions for the game (1-8): ");
      ret = scanf("%d", &numP);
      reportError(ret);
      flushLine();
      if (ret == 1 && numP > 0 && numP < 9)
         return numP;
      printf("The number of positions must be 1-8, please try again\n");
   } 
}

int getNumGuesses()
{
   int numG, ret;
   while (1){
      printf("Enter the number of guesses allowed for the game: ");
      ret = scanf("%d", &numG);
      reportError(ret);
      flushLine();
      if (ret == 1 && numG >= 1)
         return numG;
      printf("The number of guesses must be a postive\n"
         "integer, please try again");     
   }
}

void reportError(int ret)
{
   if (ret == EOF){
      fprintf(stderr, "Unexpected EOF\n");
      exit(EXIT_FAILURE);
   }
   return;
}

void flushLine()
{
   char c;
   while (EOF != scanf("%c", &c))
      if (c == '\n')
         return;
   reportError(EOF);
}
