#ifndef SCANNER_H
#define SCANNER_H

#include "Array.h"

typedef struct {        // ���y�������󵲺c              
  char *text;           //   ��J���{�� (text) 
  int textLen;          //   �ثe���y�쪺��m  
  int textIdx;          //   �{�����`����      
  char token[MAX_LEN];  //   �ثe���y�쪺���J  
} Scanner;

Scanner* ScannerNew(char *pText);
void ScannerFree(Scanner *scanner);
char* ScannerScan(Scanner *scanner);
Array* tokenize(char *text);
char *tokenToType(char *token);
void printTokens(Array *tokens);

extern char STRING[];
extern char NUMBER[];
extern char ID[];
extern char KEYWORDS[];
extern char OP1[];
extern char OP2[];
extern char COND_OP[];
extern char ITEM[];

#endif
