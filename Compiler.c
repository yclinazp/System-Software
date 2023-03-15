#include "Parser.h"
#include "Generator.h"

void compile(char *cFile, char *asmFile) {                      
  printf("compile file:%s\n", cFile, asmFile);                               
  char *cText = newFileStr(cFile);             //   讀取檔案到cText string
  Parser *parser = parse(cText);               //   剖析cText轉為語法樹
  generate(parser->tree, asmFile);             //   將語法樹轉為目的碼                  
  ParserFree(parser);                                           
  freeMemory(cText);
}





   
