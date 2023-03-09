#include <string.h>
#include "Scanner.h"

char STRING[] = "string";
char NUMBER[] = "number";
char ID[]  = "id";
char KEYWORDS[] = "|if|for|while|return|";
char OP1[] = "|++|--|";
char OP2[] = "|+|-|*|/|";
char COND_OP[] = "|==|!=|<=|>=|<|>|";
char ITEM[]= "|id|number|string|";
char OP[]  = "+-*/<=>!";

#define ch() (scanner->text[scanner->textIdx])                                  // 指向目前掃描字串的字元位置
#define next() (scanner->textIdx++)                                             // 指向目前掃描位置的下一個Char 

Scanner* ScannerNew(char *pText) {
  Scanner *scanner = ObjNew(Scanner, 1);                                        // allocate Scanner type* 1 count memory
  scanner->text = pText;
  scanner->textLen = strlen(pText);
  scanner->textIdx = 0;
  return scanner;
}
//free Scanner
void ScannerFree(Scanner *scanner) {
  ObjFree(scanner);
}
//掃描下一個token
char *ScannerScan(Scanner *scanner) {                                                                  
  while (strMember(ch(), SPACE))                                                                            
    next();                                                                     // if目前掃描位置=空白, Idx++                                      
  if (scanner->textIdx >= scanner->textLen)                                     // 檢查是否超過範圍                       
    return NULL;                                                                                                         
  char c = ch();                                                                // 取得下一個字元                         
  int begin = scanner->textIdx;                                                 // 紀錄tokens開始位置                        
  if (c == '\"') { // string = ".."                                             // 如果是" 代表string開頭               
    next(); // skip begin quote "                                               // 一直讀到下一個" 符號為止
    while (ch() != '\"') next();                                                // 跳過"                                 
    next(); // skip end quote "                                                                                          
  } else if (strMember(c, OP)) { // OP , ex : ++, --, <=, >=, ...               // 如果是OP (+-*/<=>!)              
    while (strMember(ch(), OP)) next();                                         // 一直讀到不是OP符號為止                
  } else if (strMember(c, DIGIT)) { // number, ex : 312, 77568, ...             // 如果是數字                          
    while (strMember(ch(), DIGIT)) next();                                      // 一直讀到不是數字號為止                                
  } else if (strMember(c, ALPHA)) { // name, ex : int, sum, i, for, if, ....    // 如果是英文字母                      
    while (strMember(ch(), ALPHA) || strMember(ch(), DIGIT)) next();            // 一直讀到不是英文字母(或數字)為止 ex:x1y2z
  } else // some other symbol, such as #                                                         
    next();                                                                     // 目前掃描位置+1                                                             
  strSubstr(scanner->token, scanner->text, begin, scanner->textIdx-begin);      // 設定token為(begin到textIdx) 
  return scanner->token;                                                         
}              

// 將程式轉換成一個一個token                                                                                                            
Array* tokenize(char *text) {                                                                                
  Array *tokens = ArrayNew(10);                                                 // allocate Array tokens memory (item=10)                                                                 
  Scanner *scanner = ScannerNew(text);                                                                                                              
  char *token = NULL;                                                                                                    
  while ((token = ScannerScan(scanner))!= NULL) {                               // 不斷的取出下一個token,直到程式字串結束為止
    ArrayAdd(tokens, newStr(token));                                            // 將掃描好的token 放入Array tokens
    printf("token=%s\n", token);                                                              
  }                                                                                           
  ScannerFree(scanner);                                                                       
  return tokens;                                                                                                         
}                                                                                                                        
// 判斷token的type                                                                                                                       
char *tokenToType(char *token) {                                                                                               
  if (strPartOf(token, KEYWORDS))                                               // 如果是|if|for|while|return|, type為該關鍵字自己                                            
    return token;                                                                                                                    
  else if (token[0] == '\"')                                                    // 如果以"開頭 type=string               
    return STRING;                                                                                         
  else if (strMember(token[0], DIGIT))                                          // 如果以數字開頭 type=number                     
    return NUMBER;                                                                                         
  else if (strMember(token[0], ALPHA))                                          // 如果以英文字母開頭 type=ID                  
    return ID;                                                                                                 
  else                                                                          // 否則就是單一字元 (像是+ - * / > < { }) type=該token            
    return token;                                                                                       
}                                                                                           
//print 所有tokens的string和type                                                                                          
void printTokens(Array *tokens) {                                                           
  printf("tokens->count = %d\n", tokens->count);
  int i;
  for (i=0; i<tokens->count; i++) {
    char *token = tokens->item[i];
    printf("token=%s , type=%s\n", token, tokenToType(token));
  }
}
