#include <limits.h>
#include "hashTable.h"
#include <string.h>
#include <assert.h>

typedef struct node
{
   void *data;
   int freq;
   struct node *next;
} HashNode;

typedef struct
{
   HashNode **hashArray;
   HTFunctions *funcs;
   unsigned *sizes;
   int numSizes;
   float rehashLoadFactor;
   int capacity;
   int uEntrys;
   int totEntrys;
   int sizeIndex;
} HashTable;

void memoryError(void *mem)
{
   if (mem == NULL){
      perror("Failed to allocate memory");
      exit(EXIT_FAILURE);
   }
}

void htAsserts(unsigned sizes[], int numSizes, float RLF)
{
   int i;
   assert(numSizes > 0);
   assert(sizes[0] != 0);
   assert(RLF > 0 && RLF <= 1);
   for (i = 0; i < numSizes - 1; i++)
      assert(sizes[i] < sizes[i + 1]);
}

void* htCreate(
   HTFunctions *functions,
   unsigned sizes[],
   int numSizes,
   float rehashLoadFactor)
{  
   HashTable *ht = malloc(sizeof(HashTable));
   HTFunctions *funcs;
   unsigned *sizePt = malloc(sizeof(unsigned) * numSizes);
   htAsserts(sizes, numSizes, rehashLoadFactor);
   memoryError(ht);
   memoryError(sizePt);
   memcpy(sizePt, sizes, sizeof(unsigned) * numSizes);
   ht->hashArray = calloc(sizes[0], sizeof(HashNode*));
   memoryError(ht->hashArray);
   ht->sizeIndex = 0;
   ht->sizes = sizePt;
   ht->numSizes = numSizes;
   ht->capacity = sizes[ht->sizeIndex];
   ht->uEntrys = 0;
   ht->totEntrys = 0;
   ht->rehashLoadFactor = rehashLoadFactor;
   funcs = malloc(sizeof(HTFunctions));
   memoryError(funcs);
   memcpy(funcs, functions, sizeof(HTFunctions));
   ht->funcs = funcs;
   return ht;
}

void freeLinkedList(HashNode **ht, int i)
{
   HashNode *current = ht[i];
   HashNode *next = current->next;
   while(1)
   {
      free(current->data);
      free(current);
      if(next == NULL)
         return;
      current = next;
      next = current->next;
   }
}

void htDestroy(void *hashTable)
{
   HashTable *ht;
   int i;
   ht = (HashTable*) hashTable;
   for(i = 0; i < ht->capacity; i++){
      if ((ht->funcs->destroy != NULL) && (ht->hashArray[i] != NULL))
         ht->funcs->destroy(ht->hashArray[i]);
      if (ht->hashArray[i] != NULL)
         freeLinkedList(ht->hashArray, i);
   }
   free(ht->funcs);
   free(ht->sizes);
   free(ht->hashArray);
   free(ht);
}

HashNode *createNode(void *data)
{
   HashNode *node;
   assert(data != NULL);
   node = malloc(sizeof(HashNode));
   memoryError(node);
   node->data = data;
   node->freq = 0;
   node->next = NULL;
   return node;
}

void rehashLinkedList(HashTable *new, HashNode *old)
{ 
   HashNode* current;
   current = old;
   while (1){
      if (htAdd(new, current->data) == 1)
         new->uEntrys -=1;        
      new->totEntrys -= 1;
      if (current->next == NULL)
         return;
      current = current->next;
   }
}

void destroyOldHash(HashTable *ht, HashNode **table)
{
   int i;
   for(i = 0; i < ht->sizes[ht->sizeIndex - 1]; i++){
      if ((ht->funcs->destroy != NULL) && (table[i] != NULL))
         ht->funcs->destroy(table[i]);
      if (table[i] != NULL){
         freeLinkedList(table, i);
      }
   }
   free(table);
}

void htRehash(HashTable *ht)
{ 
   int i, old;
   HashNode **hashArr;
   HashNode **oldArr = ht->hashArray;
   ht->sizeIndex += 1;
   old = ht->capacity - 1;
   ht->capacity = ht->sizes[ht->sizeIndex];
   hashArr = calloc(ht->sizes[ht->sizeIndex], sizeof(HashNode*));
   memoryError(hashArr);
   ht->hashArray = hashArr;
   for (i = old; i >= 0; i--){
      if (oldArr[i] != NULL){
         rehashLinkedList(ht, oldArr[i]);
      }
   }
   printf("finishedrehash\n");
   destroyOldHash(ht, oldArr);
}

int checkList(HashNode* current, HashNode* new, HashTable* ht)
{
   while (1)
   {
      if (ht->funcs->compare(current->data, new->data) == 0){
         current->freq += 1;
         free(new);
         return current->freq;
      }
      if (current->next == NULL){
         new->freq += 1;
         current->next = new;
         ht->uEntrys += 1;
         return 1;
      }
      current = current->next;
   }
}

unsigned htAdd(void *hashTable, void *data)
{
   int reto;
   HashNode *node;
   HashTable *ht;
   int index;
   reto = 1;
   node = createNode(data);
   ht = (HashTable*) hashTable;
   if (((((float)ht->uEntrys/ (ht->capacity))) > ht->rehashLoadFactor) &&
      ((ht->numSizes - ht->sizeIndex) > 1) && (ht->rehashLoadFactor != 1.0))
      htRehash(ht); 
   index = (ht->funcs->hash(data)) % (ht->capacity);
   if (ht->hashArray[index] == NULL){
      node->freq += 1;
      ht->hashArray[index] = node;
      ht->uEntrys += 1;
      ht->totEntrys += 1;
      return 1;
   }
   reto = checkList(ht->hashArray[index], node, ht);
   ht->totEntrys += 1;
   return reto;
}

void htLookUpHelp(void *data, int index, HashTable *ht, HTEntry *entry)
{  
   HashNode *current;
   current = ht->hashArray[index];
   while (current != NULL){
      if (ht->funcs->compare(current->data, data) == 0){
         entry->data = current->data;
         entry->frequency = current->freq;
      }
      current = current->next;
   }
}

HTEntry htLookUp(void *hashTable, void *data)
{
   HTEntry entry;
   HashTable *ht = (HashTable*) hashTable;
   int index;
   assert(data != NULL);
   entry.data = NULL;
   entry.frequency = 0;
   index = (ht->funcs->hash(data)) % (ht->capacity);
   htLookUpHelp(data, index, ht, &entry);
   return entry;
}

int ArrHelp(HashTable *ht,HTEntry *array, unsigned *size, int ind, int numEle)
{
   HashNode *current;
   current = ht->hashArray[ind];
   while (1){
      HTEntry entry;
      entry.data = current->data;
      entry.frequency = current->freq;
      array[numEle] = entry;
      if (current->next == NULL)
         return numEle;
      current = current->next;
   }
}

HTEntry* htToArray(void *hashTable, unsigned *size)
{ 
   HTEntry *array;
   HashTable *ht;
   int i, numEle;
   numEle = 0;
   ht = (HashTable*) hashTable;
   *size = ht->capacity;
   if (ht->totEntrys == 0)
      return NULL;
   array = calloc(ht->totEntrys, sizeof(HTEntry));
   memoryError(array);
   for(i = 0; i < ht->capacity; i++){
      if (ht->hashArray[i] != NULL)
         numEle += ArrHelp(ht, array, size, i, numEle);
   }
   *size = ht->totEntrys;
   return array;
}

unsigned htCapacity(void *hashTable)
{
   HashTable *ht = (HashTable*) hashTable;  
   return ht->capacity;
}

unsigned htUniqueEntries(void *hashTable)
{ 
   HashTable *ht = (HashTable*) hashTable;  
   return ht->uEntrys;
}

unsigned htTotalEntries(void *hashTable)
{
   HashTable *ht = (HashTable*) hashTable;  
   return ht->totEntrys;
}

HTMetrics htMetrics(void *hashTable)
{
   HTMetrics metrics;

   metrics.numberOfChains = UINT_MAX;
   metrics.maxChainLength = UINT_MAX;
   metrics.avgChainLength = -1.0;

   return metrics;
}
