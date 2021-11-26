#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "struct.h"
#include "functions.h"

void openWriteFile(Storage *store)
{
   FILE *file;
   char *str = store->fName;
   char lwz[5] = {'.', 'l', 'z', 'w', '\0'};
   strcat(str, lwz);
   if ((file = fopen(str, "w")) == NULL)
      fileError(str);
   store->write = file;
   /*printf("write file opened %s", str);*/
   emptyFile(store);
   fwrite(&(store->recycleNum), 1, 1, file);
}

void finishWrite(Storage* store)
{
   unsigned char reto;
   if (store->bitCount == 0)
      return;
   store->bits >>= store->packet;
   reto = store->bits;
   fwrite(&reto, 1, 1, store->write);
}

void clearBits(Storage* store)
{
   unsigned char reto;
   reto = (store->bits << (8 - store->bitCount));
   fwrite(&reto, 1, 1, store->write);
}

void writeCodeHelper(Storage *store)
{
   unsigned char reto;
   reto = ((store->bits) >> (store->bitCount - 8));
   store->bitCount -= 8;
   store->bits <<= store->packet;
   fwrite(&reto, 1, 1, store->write);
}

void writeCode(int code, Storage *store)
{
   if (store->nextCode > (pow((double)2, (double)store->packet))){
      store->bits <<= (store->packet - 8);
      store->packet++;
   }
   store->bitCount += store->packet;
   store->bits += code;
   writeCodeHelper(store);
   if (store->bitCount >= 8){
      store->bits >>= store->packet;
      writeCodeHelper(store);
   }
}

void endFile(Storage* store)
{
   int eod;
   eod = 256;
   writeCode(eod, store);
   finishWrite(store);
}
