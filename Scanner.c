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

#define ch() (scanner->text[scanner->textIdx])
#define next() (scanner->textIdx++)

Scanner* ScannerNew(char *pText) {
  Scanner *scanner = ObjNew(Scanner, 1);
  scanner->text = pText;
  scanner->textLen = strlen(pText);
  scanner->textIdx = 0;
  return scanner;
}

void ScannerFree(Scanner *scanner) {
  ObjFree(scanner);
}

char *ScannerScan(Scanner *scanner) {                                           // ���y�U�@�ӵ��J                           
  while (strMember(ch(), SPACE))                                                // �����ť�                                 
    next();                                                                                                              
  if (scanner->textIdx >= scanner->textLen)                                     // �ˬd�O�_�W�L�d��                         
    return NULL;                                                                                                         
  char c = ch();                                                                // ���o�U�@�Ӧr��                           
  int begin = scanner->textIdx;                                                 // �O����J�}�l�I                           
  if (c == '\"') { // string = ".."                                             // �p�G�O "�A�N��r��}�Y�A                 
    next(); // skip begin quote "                                                 
    while (ch() != '\"') next();                                                // �@��Ū��U�@�� " �Ÿ�����C                                 
    next(); // skip end quote "                                                                                          
  } else if (strMember(c, OP)) { // OP , ex : ++, --, <=, >=, ...               // �p�G�OOP(+-*/<=>!���Ÿ�)                 
    while (strMember(ch(), OP)) next();                                         // �@��Ū�줣�OOP����                     
  } else if (strMember(c, DIGIT)) { // number, ex : 312, 77568, ...             // �p�G�O�Ʀr                               
    while (strMember(ch(), DIGIT)) next();                                      // �@��Ū�줣�O�Ʀr����                   
  } else if (strMember(c, ALPHA)) { // name, ex : int, sum, i, for, if, ....    // �p�G�O�^��r��                           
    while (strMember(ch(), ALPHA) || strMember(ch(), DIGIT)) next();            // �@��Ū�줣�O�^��r�� (�μƦr)���� (ex: x1y2z)
  } else // some other symbol, such as #                                                         
    next();                                                                     // �_�h�A�Ǧ^��@�r��                                                                
  strSubstr(scanner->token, scanner->text, begin, scanner->textIdx-begin);      // �]�wtoken��(begin�KtextIdx) �������l�r��
  return scanner->token;                                                        // �Ǧ^token���J
}                                                                                             
                                                                                                             
Array* tokenize(char *text) {                                                   // �N�{���ഫ���@�Ӥ@�Ӫ����J                             
  Array *tokens = ArrayNew(10);                                                                                                                     
  Scanner *scanner = ScannerNew(text);                                                                                                              
  char *token = NULL;                                                                                                    
  while ((token = ScannerScan(scanner))!= NULL) {                               // ���_���X�U�@�ӵ��J�A����{���r�굲������
    ArrayAdd(tokens, newStr(token));                                            
    printf("token=%s\n", token);                                                              
  }                                                                                           
  ScannerFree(scanner);                                                                       
  return tokens;                                                                                                         
}                                                                                                                        
                                                                                                                         
char *tokenToType(char *token) {                                                // �P�_�è��o token�����A                                                
  if (strPartOf(token, KEYWORDS))                                               //   �p�G�O����r if, for, �K                                            
    return token;                                                               //   ���A�Y��������r                                                  
  else if (token[0] == '\"')                                                    // �p�G�H�Ÿ� " �}�Y�A�h                  
    return STRING;                                                              //   ���A�� STRING                        
  else if (strMember(token[0], DIGIT))                                          // �p�G�O�Ʀr�}�Y�A�h                     
    return NUMBER;                                                              //   ���A�� NUMBER                        
  else if (strMember(token[0], ALPHA))                                          // �p�G�O�^��r���}�Y�A�h                 
    return ID;                                                                  //   ���A�� ID                            
  else                                                                          // �_�h (���O +,-,*,/,>,<,�K.)            
    return token;                                                               //   ���A�Y���� token                     
}                                                                                           
                                                                                            
void printTokens(Array *tokens) {                                                           
  printf("tokens->count = %d\n", tokens->count);
  int i;
  for (i=0; i<tokens->count; i++) {
    char *token = tokens->item[i];
    printf("token=%s , type=%s\n", token, tokenToType(token));
  }
}
