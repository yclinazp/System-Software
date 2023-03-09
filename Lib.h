#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 提高可讀性 
#define FALSE   0
#define TRUE    1
#define BYTE    unsigned char
#define BOOL    unsigned char
#define INT32   int
#define INT16   short
#define INT8    char
#define UINT32  unsigned int   //32bit無號整數
#define UINT16  unsigned short //16bit無號整數
#define UINT8  unsigned char   //8bit無號整數
#define MAX_LEN 512

// �򥻨�� 
#define min(x,y)         (x < y?x:y)
#define max(x,y)         (x > y?x:y)
#define ASSERT(cond)     assert(cond)
#define ObjNew(type, count) newMemory(count*sizeof(type)) //allocate "type*count" byte
#define ObjFree freeMemory
#define strFree freeMemory

void* newMemory(int size);
void freeMemory(void *ptr);
void checkMemory();
char *newStr(char *str);
char *newSubstr(char *str, int i, int len);
BYTE* newFileBytes(char *fileName, int *sizePtr);
char* newFileStr(char *fileName);


#define strEqual(str1, str2) (strcmp(str1, str2)==0) //if str1=str2 then strcmp=0
#define strMember(ch, set) (strchr(set, ch) != NULL) //strchr check if ch is a member of set, if not found return null 
void strSubstr(char *substr, char *str, int i, int len);
void strReplace(char *str, char *from, char to);
void strTrim(char *trimStr, char *str, char *set);
char *strSpaces(int len);
void strToUpper(char *str);
BOOL strPartOf(char *token, char *set);
void strPrint(void *data);
void strPrintln(void *data);

BOOL strHead(char *str, char *head);
BOOL strTail(char *str, char *tail);
int strCountChar(char *str, char *charSet);

// function pointer used in ArrayEach(), HashTableEach
typedef void (*FuncPtr1) (void *);
typedef int (*FuncPtr2) (void *, void *);

extern char SPLITER[];
extern char SPACE[];
extern char ALPHA[];
extern char DIGIT[];

#endif
