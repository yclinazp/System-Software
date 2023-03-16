#ifndef COMPILER_H                          //避免多次載入同一個header file到程式中
                                            //如果COMPILER_H已被定義,則忽略後續至#endif
#include "Parser.h"
#include "Generator.h"

void compile(char *cFile, char *asmFile); 

#endif
