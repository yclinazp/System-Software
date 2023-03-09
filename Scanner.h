#ifndef SCANNER_H
#define SCANNER_H

#include "Array.h"

typedef struct {                     
  char *text;                           // 輸入的程式 (text) 
  int textLen;                          // 程式的總長度 
  int textIdx;                          // 目前掃描位置 
  char token[MAX_LEN];                  // 目前掃描到的詞彙
} Scanner;

Scanner* ScannerNew(char *pText);    
void ScannerFree(Scanner *scanner);
char* ScannerScan(Scanner *scanner);    // 掃描下一個token
Array* tokenize(char *text);            // 將程式轉換成一個一個token
char *tokenToType(char *token);         // 判斷token的type 
void printTokens(Array *tokens);        // print 所有tokens的string和type

extern char STRING[];
extern char NUMBER[];
extern char ID[];
extern char KEYWORDS[];
extern char OP1[];
extern char OP2[];
extern char COND_OP[];
extern char ITEM[];

#endif
