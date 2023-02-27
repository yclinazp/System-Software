#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include "Tree.h"

typedef struct {                           // ��R�������󵲺c      
  Array *tokens;                           //   ���J��C         
  Tree *tree;                              //   ��R�� (���)    
  Array* stack;                            //   ��R�L�{�Ϊ����| 
  int tokenIdx;                            //   ���J����         
} Parser;                                                     
                                                              
Parser *parse(char *text);                 // ��R�����D�{��     
                                                              
Parser *ParserNew();                       // ��R�����غc���   
void ParserParse(Parser *p, char *text);   // ��R������R���   
void ParserFree(Parser *parser);           // ����O����         

#endif












       
