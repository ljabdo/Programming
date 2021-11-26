/*
 * A general purpose C unit testing framework developed for CPE 357 and
 * provided to students to expose them to and encourage their use of unit
 * testing and test driven development (TDD). While it was provided in
 * conjunction with the hash table project and associated exercise(s), students
 * are welcome and encourage to use and/or modify it, as they see fit, on
 * subsequent assignments in CPE 357 or anywhere else it they find it useful.
 *
 * Author: Kurt Mammen
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <float.h>
#include "unitTest.h"
#include "hashTable.h"

#define TEST_ALL -1
#define REGULAR -2 
#define SPECIAL -3

#define MAX_RANDOM_STR 72 /* Max length of any random string */

/* Prototype for all test functions. This allows the creation of an array of
 * function pointers which makes the testing code shorter and more clear. It
 * also makes it easier/faster to add new tests - NICE!
 */
typedef void (*TestFunc)();

/* Structure for a test. See initRegularTests and initSpecialTests to see
 * how it is used.
 */
typedef struct
{
   TestFunc fn;
   char *fnName;
} Test;

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

/* Modified hash algorithm from K&R (page 144). This version does NOT mod the
 * calculated hash value by the table size because it does not know the table
 * size - the caller (the hash table itself) does though!
 *
 * Also note the following:
 *   * This hash only works for C strings.
 *   * Not the best/fastest/most flexible hash - you'll want something better
 *     for the Word Frequency project.
 */
static unsigned hashString(const void *data)
{
   unsigned hash;
   const char *str = data;

   for (hash = 0; *str;  str++)
      hash = *str + 31 * hash;

   /* Return the raw hash value - the modulo must be done by caller */
   return hash;
}

static unsigned simpleHash(const void *data)
{
   return 5;
}

static int compareString(const void *a, const void *b)
{
   return strcmp(a, b);
}

/* Helper function to make random strings for robust testing. Strings will be
 * a minimum of 3 characters up to MAX_RANDOM_STR characters in length.
 */
static char* randomString()
{
   char *str;
   int i;
   int length = (rand() % MAX_RANDOM_STR - 2) + 3;

   if (NULL == (str = malloc(length + 1)))
   {
      perror("randomString()");
      exit(EXIT_FAILURE);
   }

   /* Put random but visible char values in the string */
   for (i = 0; i < length; i++)
      str[i] = (rand() % (' ' - '~' + 1)) + ' ';

   str[length] = 0; /* nul-terminate it! */

   return str; 
}

/* PROVIDED TEST (do not modify)
 *
 * This is a provided sample test that matches the Evaluation System's English
 * test description for the Hash Table project's core test01. I hope it is
 * obvious to you that this is but one of many many variations that one could
 * write that would satisfy the English test description. Recall that one of
 * the important goals of the Evaluation System is to provide you with guidance
 * on how to test you solution but still allow you the opportunity develop your
 * own tests in the hopes that doing so will create an efficient and fruitful
 * learning experience!
 */
static void core01()
{
   unsigned sizes[] = {7};
   HTFunctions funcs = {hashString, compareString, NULL};

   TEST_ERROR(htCreate(&funcs, sizes, -987, 0.73));
}

static void core02()
{
   unsigned sizes[] = {13};
   HTFunctions funcs = {hashString, compareString, NULL};

   TEST_ERROR(htCreate(&funcs, sizes, 0, 0.73));
}

static void core03()
{
   unsigned sizes[] = {0};
   HTFunctions funcs = {hashString, compareString, NULL};

   TEST_ERROR(htCreate(&funcs, sizes, 1, 0.73));
}

static void core04()
{
   unsigned sizes[] = {13, 13};
   HTFunctions funcs = {hashString, compareString, NULL};

   TEST_ERROR(htCreate(&funcs, sizes, 2, 0.73));
}

static void core05()
{
   unsigned sizes[] = {13, 5};
   HTFunctions funcs = {hashString, compareString, NULL};

   TEST_ERROR(htCreate(&funcs, sizes, 2, 0.73));
}

static void core06()
{
   unsigned sizes[] = {13};
   HTFunctions funcs = {hashString, compareString, NULL};

   TEST_ERROR(htCreate(&funcs, sizes, 1, -5));
}

static void core07()
{
   unsigned sizes[] = {13};
   HTFunctions funcs = {hashString, compareString, NULL};

   TEST_ERROR(htCreate(&funcs, sizes, 1, 0));
}

static void core08()
{
   unsigned sizes[] = {13};
   HTFunctions funcs = {hashString, compareString, NULL};

   TEST_ERROR(htCreate(&funcs, sizes, 1, 2));
}

static void core09()
{
   void *ht;
   unsigned sizes[] = {13};
   HTFunctions funcs = {hashString, compareString, NULL};

   ht = htCreate(&funcs, sizes, 1, 0.63);
   TEST_ERROR(htAdd(ht, NULL));
}

static void core10()
{
   void *ht;
   unsigned sizes[] = {13};
   HTFunctions funcs = {hashString, compareString, NULL};

   ht = htCreate(&funcs, sizes, 1, -5);
   htLookUp(ht, NULL);
}

/* PROVIDED TEST (do not modify)
 *
 * This is a provided sample test that matches the Evaluation System's English
 * test description for the Hash Table project's core test11. I hope it is
 * obvious to you that this is but one of many many variations that one could
 * write that would satisfy the English test description. Recall that one of
 * the important goals of the Evaluation System is to provide you with guidance
 * on how to test you solution but still allow you the opportunity develop your
 * own tests in the hopes that doing so will create an efficient and fruitful
 * learning experience!
 */ 
static void core11()
{
   unsigned sizes[] = {31};
   HTFunctions funcs = {hashString, compareString, NULL};
   void *ht = htCreate(&funcs, sizes, 1, 0.73);

   /* Do the test(s) */
   TEST_UNSIGNED(htCapacity(ht), 31);
   TEST_UNSIGNED(htUniqueEntries(ht), 0);
   TEST_UNSIGNED(htTotalEntries(ht), 0);

   htDestroy(ht);
}

/* PROVIDED TEST (do not modify)
 *
 * This is a provided sample test that matches the Evaluation System's English
 * test description for the Hash Table project's core test12. I hope it is
 * obvious to you that this is but one of many many variations that one could
 * write that would satisfy the English test description. Recall that one of
 * the important goals of the Evaluation System is to provide you with guidance
 * on how to test you solution but still allow you the opportunity develop your
 * own tests in the hopes that doing so will create an efficient and fruitful
 * learning experience!
 */ 
static void core12()
{
   int freq = 0;
   unsigned sizes[] = {11, 23};
   HTFunctions funcs = {hashString, compareString, NULL};
   void *ht = htCreate(&funcs, sizes, 2, 0.73);

   /* Single entry */
   freq = htAdd(ht, randomString());

   TEST_UNSIGNED(freq, 1);
   TEST_UNSIGNED(htCapacity(ht), 11);
   TEST_UNSIGNED(htUniqueEntries(ht), 1);
   TEST_UNSIGNED(htTotalEntries(ht), 1);
   
   htDestroy(ht);
}

static void core13()
{
   int freq = 0;
   unsigned sizes[] = {11, 23};
   HTFunctions funcs = {hashString, compareString, NULL};
   void *ht = htCreate(&funcs, sizes, 2, 0.73);

   /* Single entry */
   freq = htAdd(ht, randomString());
   freq = htAdd(ht, randomString());

   TEST_UNSIGNED(freq, 1);
   TEST_UNSIGNED(htCapacity(ht), 11);
   TEST_UNSIGNED(htUniqueEntries(ht), 2);
   TEST_UNSIGNED(htTotalEntries(ht), 2);
   
   htDestroy(ht);
}

static void core14()
{
   int freq = 0;
   unsigned sizes[] = {11, 23};
   HTFunctions funcs = {hashString, compareString, NULL};
   void *ht = htCreate(&funcs, sizes, 2, 0.73);

   /* Single entry */
   char *str;
   char *str2;
   str = malloc(5);
   str2 = malloc(5);
   strcpy(str, "test");
   strcpy(str2, "test");

   TEST_BOOLEAN(str == NULL, 0);
   TEST_BOOLEAN(str == NULL, 0);
   freq = htAdd(ht, str);
   freq = htAdd(ht, str2);

   TEST_UNSIGNED(freq, 2);
   TEST_UNSIGNED(htCapacity(ht), 11);
   TEST_UNSIGNED(htUniqueEntries(ht), 1);
   TEST_UNSIGNED(htTotalEntries(ht), 2);
  
   free(str2);
   htDestroy(ht);
}

static void core15()
{
   int freq = 0;
   unsigned sizes[] = {11, 23};
   HTFunctions funcs = {simpleHash, compareString, NULL};
   void *ht = htCreate(&funcs, sizes, 2, 0.73);

   /* Single entry */
   freq = htAdd(ht, randomString());
   freq = htAdd(ht, randomString());

   TEST_UNSIGNED(freq, 1);
   TEST_UNSIGNED(htCapacity(ht), 11);
   TEST_UNSIGNED(htUniqueEntries(ht), 2);
   TEST_UNSIGNED(htTotalEntries(ht), 2);
   
   htDestroy(ht);
}

static void core16()
{
   HTEntry entry;
   unsigned sizes[] = {11, 23};
   HTFunctions funcs = {hashString, compareString, NULL};
   void *ht = htCreate(&funcs, sizes, 2, 0.73);
   char *str = randomString();
   entry = htLookUp(ht, str);
   TEST_UNSIGNED(entry.frequency, 0);
   TEST_BOOLEAN(entry.data == NULL, 1);
   free(str);
   htDestroy(ht);
}

static void core17()
{
   HTEntry entry;
   unsigned sizes[] = {11, 23};
   HTFunctions funcs = {hashString, compareString, NULL};
   void *ht = htCreate(&funcs, sizes, 2, 0.73);
   char *str = randomString();

   htAdd(ht, randomString());
   htAdd(ht, randomString());
   htAdd(ht, randomString());
   htAdd(ht, randomString());

   entry = htLookUp(ht, str);
   TEST_UNSIGNED(entry.frequency, 0);
   TEST_BOOLEAN(entry.data == NULL, 1);
   free(str);
   htDestroy(ht);
}

static void core18()
{
   HTEntry entry;
   unsigned sizes[] = {11, 23};
   HTFunctions funcs = {hashString, compareString, NULL};
   void *ht = htCreate(&funcs, sizes, 2, 0.73);
   char *str = randomString();
   char *str2 = randomString();

   htAdd(ht, str);
   htAdd(ht, randomString());
   htAdd(ht, str2);
   htAdd(ht, randomString());

   entry = htLookUp(ht, str);
   TEST_UNSIGNED(entry.frequency, 1);
   TEST_BOOLEAN(entry.data == NULL, 0);
   entry = htLookUp(ht, str2);
   TEST_UNSIGNED(entry.frequency, 1);
   TEST_BOOLEAN(entry.data == NULL, 0);
   htAdd(ht, str2);
   entry = htLookUp(ht, str2);
   TEST_UNSIGNED(entry.frequency, 2);
   TEST_BOOLEAN(entry.data == NULL, 0);
   htDestroy(ht);
}

static void core18a()
{
   HTEntry entry;
   unsigned sizes[] = {11, 23};
   HTFunctions funcs = {simpleHash, compareString, NULL};
   void *ht = htCreate(&funcs, sizes, 2, 0.73);
   char *str = randomString();
   char *str2 = randomString();

   htAdd(ht, str);
   htAdd(ht, randomString());
   htAdd(ht, str2);
   htAdd(ht, randomString());
   htAdd(ht, str2);

   entry = htLookUp(ht, str);
   TEST_UNSIGNED(entry.frequency, 1);
   TEST_BOOLEAN(entry.data == NULL, 0);
   entry = htLookUp(ht, str2);
   TEST_UNSIGNED(entry.frequency, 2);
   TEST_BOOLEAN(entry.data == NULL, 0);
   htAdd(ht, str2);
   entry = htLookUp(ht, str2);
   TEST_UNSIGNED(entry.frequency, 3);
   TEST_BOOLEAN(entry.data == NULL, 0);
   htDestroy(ht);
}

static void core19()
{
   HTEntry entry;
   unsigned sizes[] = {11, 23};
   HTFunctions funcs = {hashString, compareString, NULL};
   void *ht = htCreate(&funcs, sizes, 2, 0.73);
   
   char *str;
   char *str2;
   str = malloc(5);
   str2 = malloc(5);
   strcpy(str, "test");
   strcpy(str2, "test");

   htAdd(ht, str);
   htAdd(ht, str2);

   entry = htLookUp(ht, str);
   TEST_UNSIGNED(entry.frequency, 2);
   TEST_BOOLEAN(entry.data == NULL, 0);
   TEST_BOOLEAN(compareString(entry.data, str), 0);
   free(str2);
   htDestroy(ht);
}

static void feature02()
{
   HTEntry entry;
   HashTable* ht;
   void *hasht;
   char *str;
   char *str2;
   char *str3;
   char *str4;
   char *str5;
   HTFunctions funcs = {hashString, compareString, NULL};
   unsigned sizes[] = {5, 11};
   str = malloc(5);
   str2 = malloc(5);
   str3 = malloc(5);
   str4 = malloc(5);
   str5 = malloc(5);
   strcpy(str, "tots");
   strcpy(str2, "test");
   strcpy(str3, "yesz");
   strcpy(str4, "hotw");
   strcpy(str5, "ypae");
   hasht = htCreate(&funcs, sizes, 2, 0.5);
   ht = (HashTable*) hasht;
   
   htAdd(hasht, str5);
   htAdd(hasht, str4);
   htAdd(hasht, str3);
   htAdd(hasht, str2);
   htAdd(hasht, str);

   TEST_UNSIGNED(ht->capacity, 11);
   TEST_UNSIGNED(ht->uEntrys, 5);
   TEST_UNSIGNED(ht->totEntrys, 5);

   entry = htLookUp(ht, str5);
   TEST_UNSIGNED(entry.frequency, 1);
   TEST_BOOLEAN(entry.data == NULL, 0);

   entry = htLookUp(ht, str4);
   TEST_UNSIGNED(entry.frequency, 1);
   TEST_BOOLEAN(entry.data == NULL, 0);

   entry = htLookUp(ht, str3);
   TEST_UNSIGNED(entry.frequency, 1);
   TEST_BOOLEAN(entry.data == NULL, 0);

   entry = htLookUp(ht, str2);
   TEST_UNSIGNED(entry.frequency, 1);
   TEST_BOOLEAN(entry.data == NULL, 0);

   entry = htLookUp(ht, str);
   TEST_UNSIGNED(entry.frequency, 1);
   TEST_BOOLEAN(entry.data == NULL, 0);

   htDestroy(ht);
}

static void feature06()
{
   HTEntry* entry;
   void *ht;
   unsigned sizes[] = {11, 23};
   unsigned size;
   unsigned *sizePt;
   HTFunctions funcs = {hashString, compareString, NULL};
   ht = htCreate(&funcs, sizes, 2, 0.5);
   sizePt = &size;

   entry = htToArray(ht, sizePt);
   TEST_BOOLEAN(entry != NULL, 0);
   htDestroy(ht);
}

static void feature07()
{
   HTEntry* entry;
   void *ht;
   int i;
   unsigned sizes[] = {5};
   unsigned size;
   unsigned *sizePt;
   HTFunctions funcs = {hashString, compareString, NULL};
   ht = htCreate(&funcs, sizes, 2, 1.0);
   sizePt = &size;

   htAdd(ht, randomString());
   htAdd(ht, randomString());
   htAdd(ht, randomString());
   htAdd(ht, randomString());
   htAdd(ht, randomString());

   entry = htToArray(ht, sizePt);
   TEST_BOOLEAN(entry == NULL, 0);
   for(i = 0; i < 5; i++)
      printf("Array Value = %s\n", (char *) entry[i].data);
   htDestroy(ht);
}
static void testAll(Test* tests)
{
   int i;

   for (i = 0; tests[i].fn != NULL; i++)
   {
      printf("Running regular %s\n", tests[i].fnName);

      /* Call the test function via function pointer in the array */
      tests[i].fn();
   }
}

static void findAndCall(Test* tests, const char *type, char *fnName)
{
   int i;

   for (i = 0; tests[i].fn != NULL; i++)
   {
      if (0 == strcmp(tests[i].fnName, fnName))
      {
         /* Found it, call the function via function pointer... */
         printf("Running %s %s\n", type, fnName);
         tests[i].fn();
         return;
      }
   }

   fprintf(stderr, "ERROR %s %s: Function not found\n", type, fnName);
   exit(EXIT_FAILURE);
}

static void runTests(Test *tests, const char *type, char *fnName)
{
   if (fnName == NULL)
      testAll(tests);
   else
      findAndCall(tests, type, fnName);

   /* Free the tests (allocated in initTests) */
   free(tests);
}

static char* checkArgs(int argc, char *argv[], int *testType)
{
   char *testName;

   if (argc == 1)
   {
      *testType = REGULAR;      
      testName = NULL;
   }
   else if (argc == 2)
   {
      *testType = REGULAR; 
      testName = argv[1];
   }
   else if (argc == 3)
   {
      if (0 != strcmp(argv[1], "-special"))
      {
         fprintf(stderr, "Invalid option '%s'\n", argv[1]);
         exit(EXIT_FAILURE);
      }
      
      *testType = SPECIAL;
      testName = argv[2];
   }
   else
   {
      fprintf(stderr, "Usage: %s [testName | -special testName]\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   return testName;
}

Test* initTests(Test tests[], int size)
{
   Test *dynamicMemory = malloc(size);

   if (dynamicMemory == NULL)
   {
      fprintf(stderr, "FAILURE in %s at %d: ", __FILE__, __LINE__);
      perror(NULL);
      exit(EXIT_FAILURE);
   }

   return memcpy(dynamicMemory, tests, size);
}

/* Allocates, initializes, and returns the array of regular test functions.
 * Regular test functions are those that are expected to pass or report failure
 * BUT NOT terminate the test driver.
 *
 * By default, the test driver runs ALL of the regular tests. Alternatively, you
 * can run one at a time by simply specifying its name when you invoke the
 * test driver.
 *
 * See initSpecialTests for tests that you always want to run individually.
 *
 * NOTE: The last structure in the array must have NULL values as this indicates
 *    the array's end.
 *
 * IMPORTANT SUBTLETY: You can only use sizeof to obtain an array's size in
 *    the scope where the array is declared, otherwise you will just get
 *    the size of the pointer to the array.
 */
Test* initRegularTests()
{
   Test local[] = {
      {core11, "core11"},
      {core12, "core12"},
      {core13, "core13"},
      {core14, "core14"},
      {core15, "core15"},
      {core16, "core16"},
      {core17, "core17"},
      {core18, "core18"},
      {core18a, "core18a"},
      {core19, "core19"},
      {feature02, "feature02"},
      {feature06, "feature06"},
      {feature07, "feature07"},
      {NULL, NULL}
   };

   /* See IMPORTANT SUBTLETY above regarding the use of sizeof on arrays */
   return initTests(local, sizeof(local));
}

/* Allocates, initializes, and returns the array of special test functions.
 * Special test functions are those that you want to run individually for one
 * reason or another. For example, a test to see if a function asserts failure
 * when it is supposed to. Or, a test intended to measure CPU or memory
 * performance using tools like the Unix time command or Valgrind - for
 * tests like these you only want to run the single test and then observe the
 * results.
 *
 * See initRegularTests for tests that can together.
 *
 * NOTE: The last structure in the array must have NULL values as this indicates
 *    the array's end.
 *
 * IMPORTANT SUBTLETY: You can only use sizeof to obtain an array's size in
 *    the scope where the array is declared, otherwise you will just get
 *    the size of the pointer to the array.
 */
Test* initSpecialTests()
{
   Test local[] = {
      {core01, "core01"},
      {core02, "core02"},
      {core03, "core03"},
      {core04, "core04"},
      {core05, "core05"},
      {core06, "core06"},
      {core07, "core07"},
      {core08, "core08"},
      {core09, "core09"},
      {core10, "core10"},
      {NULL, NULL}
   };

   /* See IMPORTANT SUBTLETY above regarding the use of sizeof on arrays */
   return initTests(local, sizeof(local)); 
}

/* Test driver for Hash Table Project.
 *
 * Usage: testHashTable [functionName | -special functionName]
 *
 * When no option is specified ALL of the regular tests are run.
 * When "functionName" is specified that single regular test is run.
 * When "-special functionName" is specified that single special test is run.
 *
 * What TODO: As provided with the Unit Test Hash Table exercise, this unit test
 * driver provides several working examples of actual Evaluation System tests
 * for the Hash Table project. The exercise asks you to develop a few more so
 * that you can experience how easy it is to develop unit tests BEFORE you even
 * implement any actual code for the solution being tested. The hope is that
 * this will encourage you to develop tests early and often as you develop your
 * Hash Table project solution thereby adding unit testing and Test Driven
 * Development (TDD) to your personal repertoire of programming skills! 
 *    
 * To add a new test you must:
 * 
 *    1) Write a test function.
 *    2) Add its name to the appropriate array of test functions, see
 *       initRegularTests and initSpecialTests for details.
 *    3) Compile and test your test by verifying:
 *          1. It report failure when run on code known to be incorrect.
 *          2. It passes (does NOT report failure) when run on code believed to
 *             be correct.
 *    4) Be sure use Valgrind, where appropriate, to check for memory errors
 *       and/or leaks too!
 */
int main(int argc, char *argv[])
{
   char *testName;
   int testType;

   /* Random numbers used to produce "interesting" strings for testing */
   srand(182955);

   /* Make stdout unbuffered so that test output is synchronous on signals */
   setbuf(stdout, NULL);

   /* Get the test name type */
   testName = checkArgs(argc, argv, &testType);
 
   /* Run the test(s)... */
   if (testType == REGULAR)
      runTests(initRegularTests(), "regular", testName);
   else
      runTests(initSpecialTests(), "special", testName);
   
   return 0;
}
