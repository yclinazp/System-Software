#ifndef GENERATOR_H
#define GENERATOR_H

#include "Tree.h"
#include "HashTable.h"
//程式產生器Generator
//1.GenCode()  將語意樹轉換為組合語言檔案
//2.GenData()  將符號表中所有符號轉換為資料宣告的RESW

typedef struct {                                                                                     
  HashTable *symTable;                                                                    // 符號表                       
  Tree *tree;                                                                             // 剖析樹                       
  FILE *asmFile;                                                                          // FILE 欲讀寫的文件,組語檔         
  int forCount, varCount;                                                                 // For迴圈與臨時變數的數量   
} Generator;                                                                              // 組語程式碼產生器                               
                                                                                                                          
void generate(Tree *tree, char *asmFile);                                                 // 主函數            
                                                                                                                          
Generator *GenNew();                                                                      // Generator建構             
void GenFree(Generator *g);                                                               // Generator解構            
Tree* GenCode(Generator *g, Tree *node, char *rzVar);                                     // 程式轉換,遞迴方式訪問每一個節點, 取出對應子節點, 以子節點傳回內容進行轉換              
void GenData(Generator *g);                                                               // 將符號表的符號轉換組語最下方的資料宣告, ex: sum: RESW 1 i: RESW 1 T0: RESW 1                   
void GenPcode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo);        // 將語法樹轉換成pcode虛擬碼再轉為組合語言             
void GenPcodeToAsm(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo);   // 將中間碼轉成組合語言             
void GenAsmCode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo);      // 輸出組合語言指令到asmFile           
void GenTempVar(Generator *g, char *tempVar);                                             // 產生一個臨時變量T0 T1...         
void negateOp(char *condOp, char *negOp);                                                 // 對condOp做互補運算
// label、操作符 op、參數 p1、參數 p2、目標參數 pTo
#endif


















