#ifndef GENERATOR_H
#define GENERATOR_H

#include "Tree.h"
#include "HashTable.h"

typedef struct {                                                                          // �{���X���;�����                
  HashTable *symTable;                                                                    //   �Ÿ���                        
  Tree *tree;                                                                             //   ��R��                        
  FILE *asmFile;                                                                          //   ��X��CPU0�զX�y����          
  int forCount, varCount;                                                                 //   For�j��P�{���ܼƪ��ƶq       
} Generator;                                                                                                              
                                                                                                                          
void generate(Tree *tree, char *asmFile);                                                 // �{���X���;����D���            
                                                                                                                          
Generator *GenNew();                                                                      // Generator ���غc���            
void GenFree(Generator *g);                                                               // Generator ���Ѻc���            
Tree* GenCode(Generator *g, Tree *node, char *rzVar);                                     // ���ͲզX�y���{���X              
void GenData(Generator *g);                                                               // ���͸�ƫŧi                    
void GenPcode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo);        // ��X�����X pcode                
void GenPcodeToAsm(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo);   // �N�����X�ର�զX�y��            
void GenAsmCode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo);      // ��X�զX�y�����O                
void GenTempVar(Generator *g, char *tempVar);                                             // ���o�U�@���{���ܼƦW��          
void negateOp(char *condOp, char *negOp);                                                 // ������B�⪺���ɹB��,ex: < �� >=

#endif


















