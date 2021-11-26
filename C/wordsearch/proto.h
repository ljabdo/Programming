#ifndef PROTO_H
#define PROTO_H

int tryOpen(fileInfo *f, char *str);
void countAll(fileInfo *f);
void grabCommandFlagLoop(char a[], fileInfo *c);
void grabCommandFlags(int argc, char *argv[], fileInfo *f);
int validateFile(char *str);

#endif
