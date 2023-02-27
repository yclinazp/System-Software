#include "Parser.h"
#include "Generator.h"

void compile(char *cFile, char *asmFile) {     // �sĶ���D�{��                  
  printf("compile file:%s\n", cFile, asmFile);                               
  char *cText = newFileStr(cFile);             //   Ū���ɮר� cText �r�ꤤ�C   
  Parser *parser = parse(cText);               //   ��R�{�� (cText) �ର�y�k�� 
  generate(parser->tree, asmFile);             //   �{���X����                  
  ParserFree(parser);                          //   ����O����                  
  freeMemory(cText);
}





   
