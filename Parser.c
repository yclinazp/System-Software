#include "Parser.h"

Parser *parse(char *text) {        // ��R�����D�n���          
  Parser *p=ParserNew();           // �إ߭�R��       
  ParserParse(p, text);            // �}�l��R         
  return p;                        // �Ǧ^��R��       
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
  Parser *parser = ObjNew(Parser, 1);
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

void ParserParse(Parser *p, char *text) {                 // ��R���󪺥D���            
  printf("======= tokenize =======\n");                   //   �����I�s���y�����D��� tokenize() �N�{���ഫ�����J��C
  p->tokens = tokenize(text);                             
  printTokens(p->tokens);                                                     
  p->tokenIdx = 0;                                                                    
  printf("======= parsing ========\n");                                       
  p->tree = parseProg(p);                                 // �}�l��R PROG = BaseList
  if (p->stack->count != 0) {                             // �p�G��R��������|�O�Ū��A���N�O��R���\ 
    printf("parse fail:stack.count=%d", p->stack->count); //   �_�h�N���ܿ��~�T��
    error();                                                               
  }
}

void error() {
  printf("error()!\n");
  exit(1);
}

// PROG = BaseList
Tree *parseProg(Parser *p) {                // ��R PROG=BaseList �W�h    
  push(p, "PROG");                                                      
  parseBaseList(p);                         // �إ� PROG �����          
  return pop(p, "PROG");                    //  ��R BaseList�A            
}                                           // ���X PROG ����R��        
                                                                        
// BaseList= (BASE)*                        // ��R BaseList =(BASE)* �W�h  
void parseBaseList(Parser *p) {                                         
  push(p, "BaseList");                      // �إ� BaseList �����      
  while (!isEnd(p) && !isNext(p, "}"))      //  ��R BASE�A����{�������θI�� } ����
      parseBase(p);                                        
  pop(p, "BaseList");                       // ���X BaseList ����R��    
}

// BASE = FOR | STMT ';'
void parseBase(Parser *p) {                 // ��R BASE = FOR|STMT �W�h                           
  push(p, "BASE");                                                                                
  if (isNext(p, "for"))                     // �إ� BASE �����                                     
      parseFor(p);                          // �p�G�U�@�ӵ��J�O for                                  
  else {                                    //  �ھ� FOR �W�h�i���R                                
      parseStmt(p);                         // �_�h                                                  
      next(p, ";");                         //  �ھ� STMT �W�h�i���R                               
  }                                                                                               
  pop(p, "BASE");                           // ���X BASE ����R��                                  
}                                                                                                 
                                                                                                  
// FOR = for (STMT; COND; STMT) BLOCK       // ��R FOR = for ( STMT ; COND ; STMT ) BLOCK 
void parseFor(Parser *p) {                  // �إ� FOR �����                                     
  push(p, "FOR");                           // ���o for                                             
  next(p, "for");                           // ���o (                                               
  next(p, "(");                             // ��R STMT                                          
  parseStmt(p);                             // ���o ;                                               
  next(p, ";");                             // ��R COND                                          
  parseCond(p);                             // ���o ;                                               
  next(p, ";");                             // ��R STMT                                          
  parseStmt(p);                             // ���o )                                               
  next(p, ")");                             // ��R BLOCK                                         
  parseBlock(p);                            // ���X FOR ����R��                                   
  pop(p, "FOR");
}

// BLOCK = '{' BaseList '}'
void parseBlock(Parser *p) {
  push(p, "BLOCK");
  next(p, "{");
  parseBaseList(p);
  next(p, "}");
  pop(p, "BLOCK");
}

// STMT = return id | id '=' EXP  | id OP1
void parseStmt(Parser *p) {
  push(p, "STMT");
  if (isNext(p, "return")) {
    next(p, "return");
    next(p, "id");
  } else {
    next(p, "id");
    if (isNext(p, "="))  { // id '=' EXP   --> ASSIGN
      next(p, "=");
      parseExp(p);
    } else              // id OP1
      next(p, OP1);
  }
  pop(p, "STMT");
}

// EXP = ITEM [+-*/] ITEM | ITEM
void parseExp(Parser *p) {
  push(p, "EXP");
  next(p, ITEM);
  if (isNext(p, OP2)) {
      next(p, OP2);
      next(p, ITEM);
  }
  pop(p, "EXP");
}

// COND = EXP COND_OP EXP
void parseCond(Parser *p) {
  push(p, "COND");
  parseExp(p);
  next(p, COND_OP);
  parseExp(p);
  pop(p, "COND");
}

char* level(Parser *p) {
  return strSpaces(p->stack->count);
}

char* nextToken(Parser *p) {
  return (char*) p->tokens->item[p->tokenIdx];
}

BOOL isEnd(Parser *p) {
  return (p->tokenIdx >= p->tokens->count);
}

BOOL isNext(Parser *p, char *pTypes) {
  char *token = nextToken(p); 
  if (token == NULL) return FALSE;
  char *type = tokenToType(token);
  char tTypes[MAX_LEN+1];
  sprintf(tTypes, "|%s|", pTypes);
  if (strPartOf(type, tTypes))
    return TRUE;
  else
    return FALSE;
}
        
char *next(Parser *p, char *pTypes) {                         // �ˬd�U�@�ӵ��J�����A                                          
  char *token = nextToken(p);                                 // ���o�U�@�ӵ��J                                               
  if (isNext(p, pTypes)) {                                    // �p�G�OpTypes���A���@                                         
    char *type = tokenToType(token);                          //   ���o���A                                                    
    Tree *child = TreeNew(type, token);                       //   �إߵ��J�`�I(token,type)                                    
    Tree *parentTree = ArrayPeek(p->stack);                   //   ���o���`�I�A                                                
    TreeAddChild(parentTree, child);                          //   �[�J���`�I�����l��                                          
    printf("%s idx=%d, token=%s, type=%s\n",                  //   �L�X���J�H�K�[��                                            
      level(p),p->tokenIdx,token,type);                                                                                     
    p->tokenIdx++;                                            //   �e�i��U�@�Ӹ`�I                                            
    return token;                                             //   �Ǧ^�ӵ��J                                                  
  } else {                                                    // �_�h(�U�@�Ӹ`�I���A���~)                                     
    printf("next():%s is not type(%s)\n", token, pTypes);     //   �L�X���~�T��                                                
    error();                                                                                                                
    p->tokenIdx++;                                            //  �e�i��U�@�Ӹ`�I                                                                                                          
    return NULL;                                              
  }                                                                                                                         
}                                                                                                                           
                                                                                                                            
Tree* push(Parser *p, char* pType) {                          // �إ� pType ���A���l��A���J���|��                                                              
  printf("%s+%s\n", level(p), pType);                                                      
  Tree* tree = TreeNew(pType, "");                                                                                          
  ArrayPush(p->stack, tree);                                                                                                
  return tree;                                                                                                              
}                                                                                                                           
                                                                                                                            
Tree* pop(Parser *p, char* pType) {                           // ���X pType���A���l��                                          
  Tree *tree = ArrayPop(p->stack);                            // ���o���|�̤W�h���l��                                         
  printf("%s-%s\n", level(p), tree->type);                    // �L�X�H�K�[��                                                 
  if (strcmp(tree->type, pType)!=0) {                         // �p�G���A���ŦX                                               
    printf("pop(%s):should be %s\n",tree->type,pType);        //  �L�X���~�T��                                                
	error();                                                                                                                   
  }                                                                                                                         
  if (p->stack->count > 0) {                                  // �p�G���|���O�Ū�                                                                       
    Tree *parentTree = ArrayPeek(p->stack);                   //  ���X�W�@�h��R��                                             
    TreeAddChild(parentTree, tree);                           //  �N�غc��������R��[�J�W�@�h�`�I���A�����l��
  }
  return tree;                                                                                     
}                                                                                                                           
                                                                                                                            
                                                                                                                            
