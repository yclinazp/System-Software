#include "HashTable.h"

void HashTableTest() {
  printf("\n=======HashTableTest()==========\n");
  char *names[] = { "John", "Mary", "George", "Mickey", "Snoopy", "Bob", "Jack" };
  char *ids[]    = { "1",    "2",    "3",      "4",      "5",      "6",   "7" };
  HashTable* table = HashTableNew(3);
  int i;
  for (i=0; i<5; i++){
    HashTablePut(table, names[i], ids[i]); //put (names, ids)配對 into hashtable
    }
//  for (i=0; i<7; i++)
//    printf("id=%s\n", HashTableGet(table, names[i])); //將hashtable中names對應slot對到的ids傳回
//  HashTableEach(table, strPrintln);
  HashTableFree(table);
}

int hash(char *key, int range) {
  int i, hashCode=0;
  for (i=0; i<strlen(key); i++) { //each of string
    BYTE value = (BYTE) key[i];//BYTE=unsigned char, 轉為無號數
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
  ObjFree(e); //free(e)
}

int EntryCompare(Entry *e1, Entry *e2) {
  return strcmp(e1->key, e2->key);
}

//init (2D Arrray) hashtable memory (item = size)
HashTable* HashTableNew(int size) {
  Array *table = ArrayNew(size);//allocate Array type memory(item=size)
  int i;
  for (i=0; i<table->size; i++)
    ArrayAdd(table, ArrayNew(1));//allocate size of "Array type(item=1)" byte for table->item // add 2D Array
  return table;
}


void HashTableFree(HashTable *table) {
  int ti, hi;
  for (ti=0; ti<table->size; ti++) {
    Array *hitArray = table->item[ti]; //指定每個hitArray
    ArrayFree(hitArray, (FuncPtr1) EntryFree); //執行free(hitArray)
  }
  ArrayFree(table, NULL);//將每個hitArray free之後,執行free(table)
}

Entry keyEntry;
// 將hashtable中key對應index對到的data傳回 
void *HashTableGet(HashTable *table, char *key) { 
  int slot = hash(key, table->size);            // get the index(slot) from key
  Array *hitArray = (Array*) table->item[slot]; // use index(slot) to get 對應到的列hitArray
  keyEntry.key = key;
  int keyIdx = ArrayFind(hitArray, &keyEntry, (FuncPtr2)EntryCompare);//check key existing in each of element in hitarray 
  if (keyIdx >= 0) { // key existing
    Entry *e = hitArray->item[keyIdx]; //將key對應到的data放入Entry e
    return e->data;//return 對應到的data
  }
  return NULL; // key no existing
}

// put (key, new data) into hashtable
void HashTablePut(HashTable *table, char *key, void *data) { //new data (void *data)
  Entry *e;
  int slot = hash(key, table->size);            
  Array *hitArray = (Array*) table->item[slot]; 
  keyEntry.key = key;
  int keyIdx = ArrayFind(hitArray, &keyEntry, (FuncPtr2)EntryCompare);
  if (keyIdx >= 0) { // key existing
    e = hitArray->item[keyIdx];
    e->data = data; // replace old data with new data 
  } else { // key no existing
    e = EntryNew(key, data);// 建立新配對
    ArrayAdd(hitArray, e); // 將Entry放入index(slot)對應到的列hitArray中
  }
}

// for each element(data) in Hashtable, doing f
void HashTableEach(HashTable *table, FuncPtr1 f) {
  int i, j;
  for (i=0; i<table->count; i++) {
    Array *hits = table->item[i]; //for all hitArrays
    for (j=0; j<hits->count; j++) {
      Entry *e = hits->item[j]; //Entry e point to each element in one hitArray
      f(e->data);//doing f for data
    }
  }
}

// transfer HashTable (2D) to Array (1D)
Array* HashTableToArray(HashTable *table) {
  Array *array = ArrayNew(table->count);
  int i, j;
  for (i=0; i<table->count; i++) {
    Array *hits = table->item[i];
    for (j=0; j<hits->count; j++) {
      Entry *e = hits->item[j];
      ArrayAdd(array, e->data);//將hashtable的data放到array
    }
  }
  return array;
}
