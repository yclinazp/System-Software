#ifndef CPU0_H
#define CPU0_H

#include "OpTable.h"

typedef struct {
  BYTE *m;                      // memory array
  int mSize;                    // memory size
  int R[16], IR;                // R0-R15, Instruction register
} Cpu0;                         

void runObjFile(char *objFile);  

Cpu0* Cpu0New(char *objFile);   // create Cpu0 object,load objFile
void Cpu0Free(Cpu0 *cpu0);      // free Cpu0 object
void Cpu0Run(Cpu0 *cpu0, int startPC); // 從startPC address開始run
void Cpu0Dump(Cpu0 *cpu0);      // print all register

#endif
