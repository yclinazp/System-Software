#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include "Tree.h"

//1.Scanner 將text切成一個個tokens
//2.Parser 語法剖析 比對各個EBNF語法規則,使用stack將tokens轉換成一棵語法樹
//3.Parser 語意分析 確認各個node type是否相容,輸出具有標記的語意樹

typedef struct {                                  
  Array *tokens;                           // 詞彙串列       
  Tree *tree;                              // 剖析樹 (樹根 )     
  Array* stack;                            // 剖析過程用的堆疊 (stack是一種Array type)  
  int tokenIdx;                            // 目前掃描到的詞彙位置         
} Parser;                                                     
                                                              
Parser *parse(char *text);                 // parser主程式     
                                                              
Parser *ParserNew();                          
void ParserParse(Parser *p, char *text);   // 將text轉換成剖析樹(p->tree)     
void ParserFree(Parser *parser);                    

#endif












       
