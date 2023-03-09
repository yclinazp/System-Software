#ifndef ARRAY_H
#define ARRAY_H

#include "Lib.h"

typedef struct {
  int size;    // array大小
  int count;   // array數量
  void **item; // 指向指標陣列的指標
} Array;        

typedef enum { KEEP_SPLITER, REMOVE_SPLITER } SplitMode; //保留和不保留分隔符號

extern void ArrayTest();

extern Array* ArrayNew(int size);// create new Array 
extern void ArrayFree(Array *array, FuncPtr1 freeFuncPtr); //free Array memory
extern void ArrayAdd(Array *array, void *item); // add one element into Array
extern void ArrayPush(Array *array,void *item); // push one element into Array
extern void* ArrayPop(Array *array);  // pop one element 
extern void* ArrayPeek(Array *array); // get the top element  
extern void* ArrayLast(Array *array); // get the last element 
extern void ArrayEach(Array *array, FuncPtr1 f); //for each element in Array, execute f
extern int ArrayFind(Array *array, void *data, FuncPtr2 fcmp);//Find *data in given array then return index
extern Array* split(char *str, char *spliter, SplitMode mode);//將str依分隔字符拆解成一行行token

#endif
