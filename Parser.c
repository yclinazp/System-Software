#include "Parser.h"

Parser *parse(char *text) {                 
  Parser *p=ParserNew();           // 建立剖析器    
  ParserParse(p, text);            // 開始剖析,將text轉換成剖析樹(p->tree)      
  return p;                               
}

char* nextToken(Parser *p);
char *tokenToType(char *token);
Tree* push(Parser *p, char* term);
Tree* pop(Parser *p, char* term);
Tree* parseProg(Parser *p);
void parseBlock(Parser *p);
void parseFor(Parser *p);
void parseBaseList(Parser *p);
void parseBase(Parser *p);
void parseStmt(Parser *p);
void parseExp(Parser *p);
void parseCond(Parser *p);
void error();
BOOL isEnd(Parser *p);
BOOL isNext(Parser *p, char *pTypes);
char *next(Parser *p, char *pTypes);

Parser *ParserNew() {
  Parser *parser = ObjNew(Parser, 1);                     // allocate Parser object*1 count memory
  parser->tokens = NULL;                                  
  parser->tree = NULL;                                    
  parser->stack = ArrayNew(10);
  return parser;
}

void ParserFree(Parser *parser) {
  ArrayFree(parser->tokens, strFree);
  ArrayFree(parser->stack, NULL);
  TreeFree(parser->tree);
  ObjFree(parser);
}

void ParserParse(Parser *p, char *text) {                            
  printf("======= tokenize =======\n");                   
  p->tokens = tokenize(text);                             // 將text轉成token串列
  printTokens(p->tokens);                                                     
  p->tokenIdx = 0;                                                                   
  printf("======= parsing ========\n");                   // 開始剖析                    
  p->tree = parseProg(p);                                 // 從PROG = baselist規則開始,如果剖析完成後stack為空,為成功
  if (p->stack->count != 0) {                             // 否則印出錯誤訊息 
    printf("parse fail:stack.count=%d", p->stack->count);  
    error();                                                              
  }
}

void error() {
  printf("error()!\n");
  exit(1);
}

Tree *parseProg(Parser *p) {                // 剖析PROG = BaseList 規則   
  push(p, "PROG");                          // 建立PROG的樹根                           
  parseBaseList(p);                         // 剖析Baselist          
  return pop(p, "PROG");                    // 取出PROG的剖析樹(下方很多子樹), 此時stack為空           
}                                                    
                                                                           
void parseBaseList(Parser *p) {             // 剖析BaseList= (BASE)* 規則                        
  push(p, "BaseList");                      // 建立BaseList的樹根     
  while (!isEnd(p) && !isNext(p, "}"))      // 剖析Base, 直到程式結束or碰到 "}"
      parseBase(p);                                        
  pop(p, "BaseList");                       // 取出BaseList剖析樹,掛在PROG之下 
}

void parseBase(Parser *p) {                 // 剖析BASE = FOR | STMT ';' 規則                           
  push(p, "BASE");                          // 建立BASE的樹根                                                     
  if (isNext(p, "for"))                     // 如果下一個token是for                                     
      parseFor(p);                          // 根據for規則剖析                                
  else {                                    // 否則                               
      parseStmt(p);                         // 根據STMT規則剖析                                                   
      next(p, ";");                         // 取得分號(建立新的節點,掛在BASE之下)                            
  }                                                                                               
  pop(p, "BASE");                           // 取出Base的剖析樹,掛在BaseList之下                                 
}                                                                                                 
                                                                                                                                      
void parseFor(Parser *p) {                  // 剖析FOR = 'for' '(' STMT ';' 'COND' ';' STMT ')' BLOCK 規則                                    
  push(p, "FOR");                           // 建立FOR的樹根                                              
  next(p, "for");                           // 取得FOR(建立新的節點,掛在FOR之下)                                                
  next(p, "(");                             // 取得(                                         
  parseStmt(p);                             // 取得STMT                                           
  next(p, ";");                             // 取得;                                        
  parseCond(p);                             // 剖析COND                                             
  next(p, ";");                             // 取得;                                       
  parseStmt(p);                             // 取得STMT                                             
  next(p, ")");                             // 取得)                                         
  parseBlock(p);                            // 剖析Block                                   
  pop(p, "FOR");                            // 取出FOR的剖析樹,掛在BASE之下
}


void parseBlock(Parser *p) {                // 剖析BLOCK = '{' BaseList '}' 規則
  push(p, "BLOCK");                         // 建立BLOCK的樹根  
  next(p, "{");
  parseBaseList(p);                         // 剖析BaseList                  
  next(p, "}");
  pop(p, "BLOCK");                          // 取出BLOCK的剖析樹,掛在FOR之下
}


void parseStmt(Parser *p) {                 // 剖析STMT = 'return' id | id '=' EXP  | id OP1 規則
  push(p, "STMT");                          // 建立STMT的樹根 
  if (isNext(p, "return")) {                // 如果下一個token是return
    next(p, "return");                      // 取得return 
    next(p, "id");                          // 取得id
  } else {                         
    next(p, "id");                          // 取得id
    if (isNext(p, "="))  {                  // id '=' EXP   --> ASSIGN
      next(p, "=");                         // 取得=
      parseExp(p);                          // 剖析EXP
    } else                                  // id OP1
      next(p, OP1);
  }
  pop(p, "STMT");                           // 取出STMT的剖析樹,掛在上一層節點之下 
}

void parseExp(Parser *p) {                  // 剖析EXP = ITEM [+-*/] ITEM | ITEM 規則
  push(p, "EXP");                           // 建立EXP的樹根 
  next(p, ITEM);                            // 取得ITEM
  if (isNext(p, OP2)) {                     // 如果下一個token是OP2
      next(p, OP2);
      next(p, ITEM);
  }
  pop(p, "EXP");                            // 取出EXP的剖析樹,掛在上一層節點之下  
}

void parseCond(Parser *p) {                 // 剖析COND = EXP COND_OP EXP 規則
  push(p, "COND");                          // 建立COND的樹根 
  parseExp(p);
  next(p, COND_OP);
  parseExp(p);
  pop(p, "COND");                           // 取出COND的剖析樹,掛在上一層節點之下 
}
//use spaces to show level(counts of stack)
char* level(Parser *p) {
  return strSpaces(p->stack->count);                         // return 由count個空格組成的string 
}
//return 目前char的下一個string(下一個token)
char* nextToken(Parser *p) {
  return (char*) p->tokens->item[p->tokenIdx];
}
//判斷是否到達tokens中最後一個char 
BOOL isEnd(Parser *p) {
  return (p->tokenIdx >= p->tokens->count);
}
//判斷下一個token是否為pTypes 
BOOL isNext(Parser *p, char *pTypes) {
  char *token = nextToken(p);                                 // 取得下一個token
  if (token == NULL) return FALSE;
  char *type = tokenToType(token);                            // 得到token的type, |if|for|while|return|, STRING, NUMBER, ID, 單一char
  char tTypes[MAX_LEN+1];
  sprintf(tTypes, "|%s|", pTypes);                            // 將pTypes前後加上"|,放到tTypes
  if (strPartOf(type, tTypes))                                // 在tTypes中查詢是否存在type
    return TRUE;
  else
    return FALSE;
}
// 建立新的子樹, 並掛到上一層節點之下     
char *next(Parser *p, char *pTypes) {                         // 先檢查下一個token的type                                           
  char *token = nextToken(p);                                 // 取得下一個token                                              
  if (isNext(p, pTypes)) {                                    // 判斷是否屬於pTypes                                        
    char *type = tokenToType(token);                          // 取得下一個token的type                                                      
    Tree *child = TreeNew(type, token);                       // 建立token節點                                      
    Tree *parentTree = ArrayPeek(p->stack);                   // get the top element for stack, 取得父節點                                           
    TreeAddChild(parentTree, child);                          // 將下一個token加入父節點成為子樹                                            
    printf("%s idx=%d, token=%s, type=%s\n",                  // 印出token                                             
      level(p),p->tokenIdx,token,type);                                                                                     
    p->tokenIdx++;                                            // 前進到下一個節點                                              
    return token;                                                                                                 
  } else {                                                    // 否則                                     
    printf("next():%s is not type(%s)\n", token, pTypes);     // 下一個節點type錯誤                                               
    error();                                                  // exit with 1 status                                                             
    p->tokenIdx++;                                            // 前進到下一個節點                                                                                                           
    return NULL;                                              
  }                                                                                                                         
}                                                                                                                           
// 建立pType型態的子樹,push到stack                                                                                                                            
Tree* push(Parser *p, char* pType) {                                                                                      
  printf("%s+%s\n", level(p), pType);                         // 印出 level & type                                   
  Tree* tree = TreeNew(pType, "");                            // 根據pType建立新節點tree                                                       
  ArrayPush(p->stack, tree);                                  // push tree into stack                                                         
  return tree;                                                                                                              
}                                                                                                                           
// 取出stack中的pType型態的子樹,並掛到上一層節點之下                                                                                                                            
Tree* pop(Parser *p, char* pType) {                                                                      
  Tree *tree = ArrayPop(p->stack);                            // 取得stack最上層的子樹                                          
  printf("%s-%s\n", level(p), tree->type);                    // 印出 level & type                                               
  if (strcmp(tree->type, pType)!=0) {                         // 如果與pType型態不符合                                           
    printf("pop(%s):should be %s\n",tree->type,pType);                                                          
	error();                                                    // 印出error                                                                 
  }                                                                                                                         
  if (p->stack->count > 0) {                                  // 如果stack不是空的                                                                       
    Tree *parentTree = ArrayPeek(p->stack);                   // 將建構完成的剖析樹,掛到其上一層的節點之下                                            
    TreeAddChild(parentTree, tree);                           
  }
  return tree;                                                // 成為子樹                                   
}                                                                                                                           
                                                                                                                            
                                                                                                                            
