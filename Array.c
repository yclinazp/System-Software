#include <stdlib.h>
#include <string.h>
#include "Array.h"

void ArrayTest() {
  char *names[] = { "John", "Mary", "George", "Bob" };
  Array *array = ArrayNew(1);//allocate Array*1 memory
  int i;
  for (int i=0; i<4; i++)
    ArrayAdd(array, names[i]); // add 4 string in Array
  ArrayEach(array, strPrintln);// print each element
  printf("ArrayPop()=%s\n", ArrayPop(array));// pop Bob
  printf("ArrayLast()=%s\n", ArrayLast(array));
  for (i=0; i<4; i++) {// pop Bob
    int arrayIdx = ArrayFind(array, names[i], (FuncPtr2)strcmp);
    printf("ArrayFind(%s)=%d\n", names[i], arrayIdx);
  }
  ArrayEach(array, strPrintln);
  ArrayFree(array, NULL);// free Array
}
// allocate Array type memory (item = size)
Array* ArrayNew(int size) {
  Array *array = ObjNew(Array, 1); //allocate "Array type*1 count" byte
  array->count = 0;
  array->size = size;
  array->item = ObjNew(void*, array->size); //allocate "void pointer type*size count"  byte
  return array;
}

void ArrayFree(Array *array, FuncPtr1 freeFuncPtr) {
  if (array == NULL) return;
  if (freeFuncPtr != NULL)
    ArrayEach(array, freeFuncPtr);
  ObjFree(array->item);
  ObjFree(array);
}
//put a item into array (dynamically extending array)
void ArrayAdd(Array *array, void *item) {
  ASSERT(array->count <= array->size);
  if (array->count == array->size) { 
    int newSize = array->size*2; //extend to size*2
    void **newItems = ObjNew(void*, newSize); //allocate "type*newSize" byte
    memcpy(newItems, array->item, array->size*sizeof(void*)); //copy old item to newItems
//  printf("array grow from %d to %d\n", array->count, newSize);
    ObjFree(array->item); //free old item
    array->item = newItems;
    array->size = newSize;
  }
  array->item[array->count++] = item; //put a new element in item
//printf("add item = %s\n", item);
}
// push one element into array
void ArrayPush(Array *array, void *item) {
  ArrayAdd(array, item);
}

void* ArrayPop(Array *array) {
  ASSERT(array->count>0);
  return array->item[--(array->count)];//count-1 then return item[last]
}
// get the top element 
void* ArrayPeek(Array *array) {
  return ArrayLast(array);
}
// get the top element 
void* ArrayLast(Array *array) {
  ASSERT(array->count > 0);
  return array->item[array->count-1];//return item[last]
}
//將str依分隔字符拆解成一行行token
Array* split(char *str, char *spliter, SplitMode mode) {
  Array *tokens = ArrayNew(10);//init tokens (item=10)
  int si, tokenCount=0;
  int begin=0, ti = 0;
  for (si=0; si<=strlen(str); si++) {
      if (str[si]=='\0' || strMember(str[si], spliter)) { // If encounting '\0' OR 分隔字符
        int len = si-begin; //遇到分隔字符時去計算前面的string length
        if (len > 0)
          ArrayAdd(tokens, newSubstr(str, begin, len));//將Substr(從begin開始,長度=len) 加入tokens
        if (mode == KEEP_SPLITER) //if 保留分隔字符mode
          ArrayAdd(tokens, newSubstr(str, si, 1)); //將分隔字符加入tokens
//      printf("token1=%s token2=%s\n", tokens->item[ti-2], tokens->item[ti-1]);
        begin = si+1; //更新begin到下一個Substr起始位置
      }
  }
  return tokens;//str go through完成
}

int ArrayFind(Array *array, void *data, FuncPtr2 fcmp) {
  int i;
  for (i=0; i<array->count; i++)
    if (fcmp(array->item[i], data)==0) //check data existing in each of item then return index
      return i;
  return -1;
}

void ArrayEach(Array *array, FuncPtr1 f) {
  int i;
  for (i=0; i<array->count; i++)
    f(array->item[i]); // for each element, do f
}
