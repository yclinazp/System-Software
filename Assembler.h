#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Cpu0.h"
#include <ctype.h>
#include "Lib.h"

typedef struct {                                    // ��Ķ������                     
  Array *codes;                                     //   ���O�����C                 
  HashTable *symTable;                              //   �Ÿ���                       
  HashTable *opTable;                               //   ���O��                       
} Assembler;                                                                       
                                                                                   
typedef struct {                                    // ���O����                       
  int address, opCode, size;                        //   �]�t��}�B�B��X�B           
  char *label, *op, *args, type;                    //   �Ŷ��j�p�Bop, �B�аO�B       
  char *objCode;                                    //   �ѼơB���A�B�ت��X           
} AsmCode;                                          //   �����                       
                                                                                   
void assemble(char *asmFile, char *objFile);        // ��Ķ�����D�{��              
                                                                                   
Assembler* AsmNew();                                // ��Ķ�����غc���               
void AsmFree(Assembler *a);                         // ��Ķ�����Ѻc���               
                                                                                   
void AsmPass1(Assembler *a, char *text);            // ��Ķ�����Ĥ@���q               
void AsmPass2(Assembler *a);                        // ��Ķ�����ĤG���q               
void AsmSaveObjFile(Assembler *a, char *objFile);   // �x�s�ت���                     
void AsmTranslateCode(Assembler *a, AsmCode *code); // �N���O�ର�ت��X               
int AsmCodeSize(AsmCode *code);                                                                                   
AsmCode* AsmCodeNew(char *line);                                                   
void AsmCodeFree(AsmCode *code);                    // ���O���󪺫غc���             
int AsmCodePrintln(AsmCode *code);                  // ���O���󪺸Ѻc���             
                                                    // ���O���󪺦C�L���             
#endif



























