#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Cpu0.h"
#include <ctype.h>
#include "Lib.h"

typedef struct {                                                      
  Array *codes;                                     // 指令物件陣列              
  HashTable *symTable;                              // 符號表                    
  HashTable *opTable;                               // 指令表                   
} Assembler;                                                                       
                                                                                   
typedef struct {                                    // 指令物件                    
  int address, opCode, size;                        // 記憶體位址 運算碼(LD=00, ST=01) code指令大小  
  char *label, *op, *args, type;                    // 標記 op 參數 (解析一行line得到三者) 型態(L,A,Ｊ)
  char *objCode;                                    // 目的碼      
} AsmCode;                                                               
                                                                                   
void assemble(char *asmFile, char *objFile);        //               
                                                                                   
Assembler* AsmNew();                                //             
void AsmFree(Assembler *a);                         //             
                                                                                   
void AsmPass1(Assembler *a, char *text);            //           
void AsmPass2(Assembler *a);                        //               
void AsmSaveObjFile(Assembler *a, char *objFile);   //                     
void AsmTranslateCode(Assembler *a, AsmCode *code); //             
int AsmCodeSize(AsmCode *code);                                                                                   
AsmCode* AsmCodeNew(char *line);                                                   
void AsmCodeFree(AsmCode *code);                    //              
int AsmCodePrintln(AsmCode *code);                  //            
                                                    //             
#endif



























