#include "Lib.h"
#include <ctype.h>

char SPLITER[] = " \t\n\r~!@#$%^&*()_+{}:\"<>?-=[]|\\;',./";
char SPACE[] = " \t\n\r";
char ALPHA[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char DIGIT[] = "0123456789";
char NAME_CHAR[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";


int newMemoryCount = 0;
void* newMemory(int size) { //size count*sizeof(type)
  void *ptr=malloc(size); //dynamically allocate "size bytes" memory
  assert(ptr != NULL);
  memset(ptr, 0, size); //sets the value to 0 of each byte
//printf("memGet:%p\n", ptr);
  newMemoryCount++; //track the times of this function be called
  return ptr;
}

int freeMemoryCount=0;
void freeMemory(void *ptr) {
//  printf("memFree:%p\n", ptr);
  free(ptr);
  freeMemoryCount++;
}

void checkMemory() {
  printf("newMemoryCount=%d freeMemoryCount=%d\n", newMemoryCount, freeMemoryCount);
}

// 讀取file中的內容且copy, return copy內容 (define BYTE = unsigned char) 
BYTE* newFileBytes(char *fileName, int *sizePtr) {
  FILE *file = fopen(fileName, "rb");       //mode : "rb" ,開檔且讀取二進制檔
  fseek(file, 0 , SEEK_END);                //移動檔案指標到某個位置, offest=0, offset is relative to end of file
  long size = ftell(file);                  //獲取文件大小size,offset= 文件的當前位置-開頭位置
  rewind(file);                             //移動檔案指標到開頭
  BYTE *buffer = (char*) newMemory(size+1); //allocate a char array buffer
  fread (buffer,size,1,file);               //讀取file中一個size byte大小的element(也就是all file),放入buffer
  fclose(file); 
  *sizePtr = size;                          //儲存檔案大小
  return buffer;                            //return buffer(已copy完all file)
}

// add \0 to end of file
char* newFileStr(char *fileName) {
  int size; 
  BYTE *buffer = newFileBytes(fileName, &size);
  buffer[size] = '\0';
  return (char*) buffer;
}

//allocate (str length + 1) memory
char *newStr(char *str) {
  char *rzStr = newMemory(strlen(str)+1); //allocate (string length + 1) memory
  strcpy(rzStr, str); //將str copy至rzStr
  return rzStr;
}
// 
char *newSubstr(char *str, int i, int len) {
  char *rzStr = newMemory(len+1);
  strSubstr(rzStr, str, i, len);
  return rzStr;
}
 
void strPrint(void *data) { //print string
  printf("%s ", data);
}

void strPrintln(void *data) { //print string then 換行
  printf("%s\n", data);
}
//比較str是否以head作為開頭
BOOL strHead(char *str, char *head) { 
  return (strncmp(str, head, strlen(head))==0);//相同return 0
}
//比較str是否以tail作為結尾
BOOL strTail(char *str, char *tail) {
  int strLen = strlen(str), tailLen = strlen(tail);
  if (strLen < tailLen) return FALSE;//str無法以tail結尾
  return (strcmp(str+strLen-tailLen, tail)==0);//比較str最後長度是否=tail
}

int strCountChar(char *str, char *charSet) {
  int i, count=0;
  for (i=0; i<strlen(str); i++)
    if (strMember(str[i], charSet))
      count++;
  return count;
}
//copy str的第i位置,長度len 成為substring
void strSubstr(char *substr, char *str, int i, int len) { 
  strncpy(substr, &str[i], len);//copy a substring to substr
  substr[len]='\0';//add NULL char at end
}
//在string set中查詢是否存在 string token
BOOL strPartOf(char *token, char *set) {
  ASSERT(token != NULL && set != NULL);
  ASSERT(strlen(token) < 100);
  char ttoken[100];
  sprintf(ttoken, "|%s|", token);    //在token 前後加"|"
  return (strstr(set, ttoken)!=NULL);//判斷set中是否包含ttoken
}
//trimStr = remove str中的set
void strTrim(char *trimStr, char *str, char *set) {
  char *start, *stop;
  for (start = str; strMember(*start, set); start++);//從str開頭開始, 找出不在set裡的第一個char
  for (stop = str+strlen(str)-1; stop > str && strMember(*stop, set); stop--);//從str結尾開始, 找出不在set裡的最後一個char
  if (start <= stop) { //去除str頭尾含有set的char,放到trimStr
    strncpy(trimStr, start, stop-start+1); //copy from start, length = stop-start+1
    trimStr[stop-start+1]='\0'; //make sure trimStr end with \0
  } else // str裡不存在含有set的char
    strcpy(trimStr, "");//trimStr = NULL
}
//replace "from" to "to"
void strReplace(char *str, char *from, char to) {
  int i;
  for (i=0; i<strlen(str); i++)
    if (strMember(str[i], from))
      str[i] = to;
}

char tspaces[MAX_LEN];
char* strSpaces(int level) {
  assert(level < MAX_LEN);
  memset(tspaces, ' ', MAX_LEN);
  tspaces[level] = '\0';
  return tspaces;
}
// 將str轉成大寫字母
void strToUpper(char *str) {
  int i;
  for (i = 0; i<strlen(str); i++)
    str[i] = toupper(str[i]);
}
