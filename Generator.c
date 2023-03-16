#include "Generator.h"
#include <ctype.h>


//將語法樹tree轉為組合語言檔asmFile
void generate(Tree *tree, char *asmFile) {      
  char nullVar[100]="";                                                             
  Generator *g = GenNew();                                   
  g->asmFile = fopen(asmFile, "w");             // 以寫入模式開啟組語檔                                 
  printf("=====PCODE=====\n");                                             
  GenCode(g, tree, nullVar);                    // 產生程式碼                       
  GenData(g);                                   // 將符號表的符號轉換組語最下方的資料宣告, ex: sum: RESW 1 i: RESW 1 T0: RESW 1                     
  fclose(g->asmFile);                           // 關閉組語檔                       
  GenFree(g);                                                  
  char *asmText = newFileStr(asmFile);          // 讀入組合語言檔並印出                                  
  printf("=====AsmFile:%s======\n", asmFile);                                       
  printf("%s\n", asmText);                                              
  freeMemory(asmText);
}

Generator *GenNew() {
  Generator *g = ObjNew(Generator, 1);          // allocate Generator object*1 count memory
  g->symTable = HashTableNew(127);              // allocate (item = 127) hashtable (2D Array) memory
  return g;
}

void GenFree(Generator *g) {
  HashTableFree(g->symTable);
  ObjFree(g);
}

char nullVar[100]="";
//程式轉換,遞迴方式訪問每一個節點, 取出對應子節點, 以子節點傳回內容進行轉換 (char *rzVar: 記錄要返回的變數名稱)
//子節點儲存方式為Array
Tree* GenCode(Generator *g, Tree *node, char *rzVar) {                                   
  strcpy(nullVar, "");                                                                                          
  strcpy(rzVar, "");                                                            // rzVar、nullVar設為空string                                  
  if (node == NULL) return NULL;                                                // 沒有語法樹節點就中止遞迴                     
                                                                                                                   
  if (strEqual(node->type, "FOR")) {                                            // [處理FOR節點]                     
    // (四個子節點) FOR ::= 'for' '(' STMT ';' COND ';' STMT ')' BLOCK                                                
    char forBeginLabel[100], forEndLabel[100], condOp[100];                     
    Tree *stmt1 = node->childs->item[2],                                        // 取得子節點                         
         *cond  = node->childs->item[4],                                                                           
         *stmt2 = node->childs->item[6],                                                                           
         *block = node->childs->item[8];                                                                           
    GenCode(g, stmt1, nullVar);                                                 // 遞迴產生STMT                             
    int tempForCount = g->forCount++;                                           // 設定FOR迴圈的標記               
    sprintf(forBeginLabel, "FOR%d", tempForCount);                              // 進入標記                              
    sprintf(forEndLabel, "_FOR%d", tempForCount);                               // 離開標記           
    GenPcode(g, forBeginLabel, "", "", "", "");                                 // 輸出中間碼:例如 FOR0:
    GenCode(g, cond, condOp);                                                   // 遞迴產生COND,比較運算變數, 例如 <=    
    char negOp[100];                                                                                  
    negateOp(condOp, negOp);                                                    // 互補運算negOp (<=變成>)      
    GenPcode(g, "", "J", negOp, "", forEndLabel);                               // 中間碼:例如 J >_FOR0 
    GenCode(g, block, nullVar);                                                 // 遞迴產生BLOCK     
    GenCode(g, stmt2, nullVar);                                                 // 遞迴產生STMT      
    GenPcode(g, "", "J", "", "", forBeginLabel);                                // 中間碼:例如J FOR0   
    GenPcode(g, forEndLabel, "", "", "", "");                                   // 中間碼:例如 _FOR0  
    return NULL;                                                                                      
  } else if (strEqual(node->type, "STMT")) {                                    // [處理STMT節點]          
    // (處理三個子節點) STMT = return id | id '=' EXP | id ('++'|'--')                                          
    Tree *c1 = node->childs->item[0];                                           // 取得子節點             
    if (strEqual(c1->type, "return")) {                                         // 處理return 
      // return id                              
      Tree *id = node->childs->item[1];                                                                   
      GenPcode(g, "", "RET", "", "", id->value);                                // 中間碼:例如 RET sum   
    } else {                                                                                              
      Tree *id = node->childs->item[0];                                         // 取得子節點              
      Tree *op = node->childs->item[1];                                                                   
      if (strEqual(op->type, "=")) {                                            // 處理id = EXP         
        // STMT  id '=' EXP                                                                    
        Tree *exp = node->childs->item[2];                                      // 取得子節點EXP                            
        char expVar[100];                                                          
        GenCode(g, exp, expVar);                                                // 遞迴產生EXP,將運算結果存在expVar             
        GenPcode(g, "", "=", expVar, "", id->value);                            // 中間碼: 例如 = 0 sum 
        HashTablePut(g->symTable, id->value, id->value);                        // 將id加入到符號表中                             
        strcpy(rzVar, expVar);                                                  // 傳回EXP的運算結果, 例如 T0   
      } else {  
        // id ('++'|'--')                                                                
        char addsub[100];                                                                                 
        if (strEqual(op->value, "++"))                                          // 處理id++               
          strcpy(addsub, "+");                                                  // 設為+法       
        else                                                                                          
          strcpy(addsub, "-");                                                  // 處理id--,設為-法         
        GenPcode(g, "", addsub, id->value, "1", id->value);                     // 產生中間碼: 例如 + i, 1, i 
        strcpy(rzVar, id->value);                                               // 傳回id名稱, 例如 i          
      }                                                                         
    }                                                                           
  } else if (strEqual(node->type, "COND")) {                                    // [處理COND節點]  
    // (三個子節點)  COND = EXP ('=='|'!='|'<='|'>='|'<'|'>') EXP                               
    Tree* op = node->childs->item[1];                                           // 取得子節點op       
    char expVar1[100], expVar2[100];                                                                  
    GenCode(g, node->childs->item[0], expVar1);                                 // 遞迴產生EXP,存在expVar1     
    GenCode(g, node->childs->item[2], expVar2);                                 // 遞迴產生EXP,存在expVar2    
    GenPcode(g, "", "CMP", expVar1, expVar2, nullVar);                          // 產生中間碼: 例如 CMP i, 10 
    strcpy(rzVar, op->value);                                                   // 傳回比較運算變數, 例如 >

  } else if (strPartOf(node->type, "|EXP|")) {                                  // [處理EXP節點]
    //  EXP = ITEM ([+-*/] ITEM)*                                               
    Tree *item1 = node->childs->item[0];                                        // 取得子節點 ITEM1      
    char var1[100], var2[100], tempVar[100];                                                          
    GenCode(g, item1, var1);                                                    // 遞迴產生ITEM,存在var1
    if (node->childs->count > 1) {             
      Tree* op = node->childs->item[1];                                         // 取得子節點 op和ITEM2
      Tree* item2 = node->childs->item[2];                                                         
      GenCode(g, item2, var2);                                                  // 遞迴產生ITEM,存在var2       
      GenTempVar(g, tempVar);                                                   // 取得臨時變數,例如 T0
      GenPcode(g, "", op->value, var1, var2, tempVar);                          // 產生中間碼: 例如 + sum i T0
      strcpy(var1, tempVar);                                                    // 傳回臨時變數, 例如T0 (=sum+i)   
    }                                                                                                 
    strcpy(rzVar, var1);                                                        // 傳回臨時變數給rzVar, 例如 T0 (=sum+i)  

  } else if (strPartOf(node->type, "|number|id|")) {                            // [處理|number|id|節點]                                                        
    strcpy(rzVar, node->value);                                                 // 遇到常數或變數 直接傳回常數值or變數名稱

  } else if (node->childs != NULL) {                                            // [其他情況]                                                                                                                                                              
    for (int i=0; i<node->childs->count; i++)                                   // 遞迴處理所有子節點 (PROG BaseList BASE BLOCK )
      GenCode(g, node->childs->item[i], nullVar);                               // 不需傳回值
  }
  return NULL;
}
//將符號表的符號轉換組語最下方的資料宣告, ex: sum: RESW 1 i: RESW 1 T0: RESW 1
void GenData(Generator *g) { 
  Array *symArray = HashTableToArray(g->symTable);
  int i;
  for (i=0; i<symArray->count; i++) {                                           // gothrough每一個符號(sum,i等等)
    char *varName = symArray->item[i];                                          // 將每一個符號存在varName 
    char varLabel[100];
    sprintf(varLabel, "%s:", varName);                                          // 將varName+:存到varLabel
    GenAsmCode(g, varLabel, "RESW", "1", "", "");                               // 輸出組語到asmFile
  }
  for (i=0; i<g->varCount; i++) {                                               // gothrough每一個臨時變數(T0,T1)
    char tVarLabel[100];
    sprintf(tVarLabel, "T%d:", i);                                              // 組成T0:放到tVarLabel
    GenAsmCode(g, tVarLabel, "RESW", "1", "", "");                              // 輸出組語到asmFile
  }
  ArrayFree(symArray, NULL);
}
//將語法樹轉換成pcode虛擬碼,再呼叫GenPcodeToAsm轉為組合語言
void GenPcode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo) {                
  char labelTemp[100];                                                                                                      
  if (strlen(label)>0)                                                                     // 有label                                                                                      
    sprintf(labelTemp, "%s:", label);                                                      // 將label加上冒號寫入labelTemp,ex: LOOP:                                
  else                                                                                                                      
    strcpy(labelTemp, "");                                                                 // 沒有label,確保labelTemp為空                                 
  printf("%-8s %-4s %-4s %-4s %-4s\n", labelTemp, op, p1, p2, pTo);                        // 將pcode印出                       
  GenPcodeToAsm(g, labelTemp, op, p1, p2, pTo);                                            // 將pcode轉為組語                               
}                                                                                                        
//將中間碼轉成組合語言                                                                                                                           
void GenPcodeToAsm(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo) {   
  if (strlen(label)>0)                                                                     // 如果有標記                                      
    GenAsmCode(g, label, "", "", "", "");                                                  // 輸出標記 ex: FOR:                         
  if (strEqual(op, "=")) {                 // pTo = p1                                     // [處理等號] (= 0 sum)               
    GenAsmCode(g, "", "LD", "R1", p1, "");                                                 // 例如LDI R1, 0                  
    GenAsmCode(g, "", "ST", "R1", pTo, "");                                                //    ST, R1, sum                      
  } else if (strPartOf(op, "|+|-|*|/|")) { // pTo = p1 op p2                               // [處理運算] (+ sum i sum)             
    char asmOp[100];                                                                                                       
    if (strEqual(op, "+")) strcpy(asmOp, "ADD");                                           // 根據op設定運算           
    else if (strEqual(op, "-")) strcpy(asmOp, "SUB");                                                                       
    else if (strEqual(op, "*")) strcpy(asmOp, "MUL");                                                                       
    else if (strEqual(op, "/")) strcpy(asmOp, "DIV");                                                                       
    GenAsmCode(g, "", "LD", "R1", p1, "");                                                 // 例如LD R1, sum                  
    GenAsmCode(g, "", "LD", "R2", p2, "");                                                 //    LD R2, i                    
    GenAsmCode(g, "", asmOp,"R3", "R1", "R2");                                             //    ADD R3, R1, R2              
    GenAsmCode(g, "", "ST", "R3", pTo, "");                                                //    ST R3, sum                  
  } else if (strEqual(op, "CMP")) {        // CMP p1, p2                                   // [處理CMP] (cmp i 10)              
    GenAsmCode(g, "", "LD", "R1", p1, "");                                                 // 例如LD R1, i                    
    GenAsmCode(g, "", "LD", "R2", p2, "");                                                 //    LDI R2, 10                  
    GenAsmCode(g, "", "CMP", "R1", "R2", "");                                              //    CMP R1, R2                  
  } else if (strEqual(op, "J")) {          // J op label                                   // [處理J] (J > _FOR)            
    char asmOp[100];                                                                                     
    if (strEqual(p1, "=")) strcpy(asmOp, "JEQ");                                           // 根據op設定跳躍              
    else if (strEqual(p1, "!=")) strcpy(asmOp, "JNE");                                                                      
    else if (strEqual(p1, "<")) strcpy(asmOp, "JLT");                                                                       
    else if (strEqual(p1, ">")) strcpy(asmOp, "JGT");                                                                       
    else if (strEqual(p1, "<=")) strcpy(asmOp, "JLE");                                                                      
    else if (strEqual(p1, ">=")) strcpy(asmOp, "JGE");                                                                      
    else strcpy(asmOp, "JMP");                                                                                              
    GenAsmCode(g, "", asmOp, pTo, "", "");                                                 // 例如 JGT _FOR0                                 
  } else if (strEqual(op, "RET")) {                                                        // [處理RET] 例如 RET sum                    
    GenAsmCode(g, "", "LD", "R1", pTo, "");                                                // 轉成 LD R1, sum               
    GenAsmCode(g, "", "RET", "", "", "");                                                  //     RET                
  }                                                                                                         
}                                                                                                  
// 輸出組合語言指令到asmFile                                                                                                                         
void GenAsmCode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo) {      // label、操作符 op、參數 p1、參數 p2、目標參數 pTo                                     
  char *realOp = op;                                                                                                                            
  if (strEqual(op, "LD"))                                                                  // 如果指令是LD,而且p2為常數                                 
    if (isdigit(p2[0]))                                                                                  
      realOp = "LDI";                                                                      // 改使用LDI指令                   
  fprintf(g->asmFile, "%-8s %-4s %-4s %-4s %-4s\n", label, realOp, p1, p2, pTo);           // 輸出組合語言指令到asmFile中                  
}                                                                                                          
//產生一個臨時變量T0 T1...                                                                                                        
void GenTempVar(Generator *g, char *tempVar) {                                                          
  sprintf(tempVar, "T%d", g->varCount++);
}
//對condOp做互補運算
void negateOp(char *condOp, char *negOp) {               
  if (strEqual(condOp, "==")) strcpy(negOp, "!=");                                         //若 condOp 為 "=="，則 negOp 設為 "!="
  if (strEqual(condOp, "!=")) strcpy(negOp, "=="); 
  if (strEqual(condOp, ">=")) strcpy(negOp, "<");
  if (strEqual(condOp, "<=")) strcpy(negOp, ">");
  if (strEqual(condOp, ">")) strcpy(negOp, "<=");
  if (strEqual(condOp, "<")) strcpy(negOp, ">=");
}
