#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define DEFAULT_R 12
#define START_CODE 257

void createNodeArray(Storage *store);
void parseDict(Storage *store);
void openWriteFile(Storage *store);
void fileError(char *str);
void openWriteFile(Storage *store);
void writeCode(int code, Storage *store);
void endFile(Storage* store);
void emptyFile(Storage* store);
void finishWrite(Storage *store);
void clearBits(Storage* store);
#endif
