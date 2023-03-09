#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Array.h"

typedef struct {
  char *key;
  void *data;
} Entry;

Entry* EntryNew(char *key, void *data); // 建立新配對
int EntryCompare(Entry *e1, Entry *e2); // compare two Entrykey

int hash(char *key, int range);

#define HashTable Array //將hashtable作為dynamic array

HashTable* HashTableNew(int size); //init HashTable memory size
void HashTableFree(HashTable *table); //free Hashtable
void HashTablePut(HashTable *table, char *key, void *data);// put (key, new data) into hashtable
void *HashTableGet(HashTable *table, char *key);// 將hashtable中key對應index對到的data傳回 
void HashTableEach(HashTable *table, FuncPtr1 f);// for each element(data) in Hashtable, doing f
Array* HashTableToArray(HashTable *table);// transfer HashTable (2D) to Array (1D)
void HashTableTest();

#endif
