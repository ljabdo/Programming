#ifndef SCOREKEEP_H
#define SCOREKEEP_H
#define SIZE 8

typedef struct
{
   int seed, numP, numG;
   char maxL;
   char answerK[SIZE];
   char currentG[SIZE];
} Keeper;

Keeper makeGameKeeper();
void generateKey(int size, char maxL, char board[]);
int checkGuess(char board[], int numP, char maxL);
int getGuess(int numP, char maxL, int guessNum, char board[]);
void fillBoard(int numP, char maxL, char board[]);

#endif

