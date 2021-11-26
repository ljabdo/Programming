#ifndef PROTO_H
#define PROTO_H

double getSeed();
void reportError(int ret);
void flushLine();
char getMaxLetter();
int getNumPositions();
int getNumGuesses();
int checkGuess(char board[], int numP, char maxL);
int checkLoop(char boardG[], char cpyArray[], int numP);
int runGuesses(char boardG[], char answerK[], int numG, int maxL, int numP);
int checkAnswer(char b[], char a[], int numP);
int helperLoop(char board[], char cpy[], int nump,int inex, int p);
double getSeedLoop(int seed);
int checkExact(int numP, char boardG[], char answerK[], char cpy[], int exact);
char getMaxLetterLoop();
int runGuessesHelper(Keeper GK, int currentG);

#endif
