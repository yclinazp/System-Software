#include "HashTable.h"

void HashTableTest() {
  printf("\n=======HashTableTest()==========\n");
  char *names[] = { "John", "Mary", "George", "Mickey", "Snoopy", "Bob", "Jack" };
  char *ids[]    = { "1",    "2",    "3",      "4",      "5",      "6",   "7" };
  HashTable* table = HashTableNew(3);
  int i;
  for (i=0; i<5; i++)
    HashTablePut(table, names[i], ids[i]);
//  for (i=0; i<7; i++)
//    printf("id=%s\n", HashTableGet(table, names[i]));
//  HashTableEach(table, strPrintln);
  HashTableFree(table);
}

int hash(char *key, int range) {
  int i, hashCode=0;
  for (i=0; i<strlen(key); i++) {
    BYTE value = (BYTE) key[i];
    hashCode += value;
    hashCode %= range;
  }
  return hashCode;
}

Entry* EntryNew(char *key, void *data) {
  Entry* e = ObjNew(Entry, 1);
  e->key = key;
  e->data = data;
  return e;
}

void EntryFree(Entry *e) {
  ObjFree(e);
}

int EntryCompare(Entry *e1, Entry *e2) {
  return strcmp(e1->key, e2->key);
}

HashTable* HashTableNew(int size) {
  Array *table = ArrayNew(size);
  int i;
  for (i=0; i<table->size; i++)
    ArrayAdd(table, ArrayNew(1));
  return table;
}

void HashTableFree(HashTable *table) {
  int ti, hi;
  for (ti=0; ti<table->size; ti++) {
    Array *hitArray = table->item[ti];
    ArrayFree(hitArray, (FuncPtr1) EntryFree);
  }
  ArrayFree(table, NULL);
}

Entry keyEntry;
// �M��������� key �ҹ����������öǦ^
void *HashTableGet(HashTable *table, char *key) { 
  int slot = hash(key, table->size);            // ���o����� (�C��) 
  Array *hitArray = (Array*) table->item[slot]; // ���o�ӦC
  // ��X�ӦC���O�_���]�t key ���t��
  keyEntry.key = key;
  int keyIdx = ArrayFind(hitArray, &keyEntry, (FuncPtr2)EntryCompare);
  if (keyIdx >= 0) { // �Y���A�h�Ǧ^�Ӱt�諸��Ƥ��� 
    Entry *e = hitArray->item[keyIdx];
    return e->data;
  }
  return NULL; // �_�h�Ǧ^ NULL 
}

// �N (key, data) �t���J������� 
void HashTablePut(HashTable *table, char *key, void *data) {
  Entry *e;
  int slot = hash(key, table->size);            // ���o����� (�C��) 
  Array *hitArray = (Array*) table->item[slot]; // ���o�ӦC
  keyEntry.key = key;
  int keyIdx = ArrayFind(hitArray, &keyEntry, (FuncPtr2)EntryCompare);
  if (keyIdx >= 0) { // �Y���A�h�Ǧ^�Ӱt�諸��Ƥ��� 
    e = hitArray->item[keyIdx];
    e->data = data;
  } else {
    e= EntryNew(key, data);// �إ߰t�� 
    ArrayAdd(hitArray, e); // ��J�������C�� 
  }
}

void HashTableEach(HashTable *table, FuncPtr1 f) {
  int i, j;
  for (i=0; i<table->count; i++) {
    Array *hits = table->item[i];
    for (j=0; j<hits->count; j++) {
      Entry *e = hits->item[j];
      f(e->data);
    }
  }
}

Array* HashTableToArray(HashTable *table) {
  Array *array = ArrayNew(table->count);
  int i, j;
  for (i=0; i<table->count; i++) {
    Array *hits = table->item[i];
    for (j=0; j<hits->count; j++) {
      Entry *e = hits->item[j];
      ArrayAdd(array, e->data);
    }
  }
  return array;
}
