#include "Generator.h"
#include <ctype.h>
// �{�����;����D�n��ơC
void generate(Tree *tree, char *asmFile) {      // �N��R�� tree �ର�զX�y���� asmFile
  char nullVar[100]="";                                                             
  Generator *g = GenNew();                      // �}�ҲզX�y���ɥH�K��X              
  g->asmFile = fopen(asmFile, "w");                                                 
  printf("=====PCODE=====\n");                  // ���͵{���X                          
  GenCode(g, tree, nullVar);                    // ���͸�ƫŧi                        
  GenData(g);                                   // �����զX�y����                      
  fclose(g->asmFile);                           // ����O����                          
  GenFree(g);                                   // Ū�J�զX�y���ɨæL�X                
  char *asmText = newFileStr(asmFile);                                              
  printf("=====AsmFile:%s======\n", asmFile);                                       
  printf("%s\n", asmText);                      // ����O����                          
  freeMemory(asmText);
}

Generator *GenNew() {
  Generator *g = ObjNew(Generator, 1);
  g->symTable = HashTableNew(127);
  return g;
}

void GenFree(Generator *g) {
  HashTableFree(g->symTable);
  ObjFree(g);
}

char nullVar[100]="";

Tree* GenCode(Generator *g, Tree *node, char *rzVar) {                          // ���j���͸`�I node ���{���X         
  strcpy(nullVar, "");                                                                                             
  strcpy(rzVar, "");                                                                                               
  if (node == NULL) return NULL;                                                // ���j�פ����C                     
                                                                                                                   
  if (strEqual(node->type, "FOR")) {                                            // �B�z FOR �`�I                      
    // FOR ::= 'for' '(' STMT ';' COND ';' STMT ')' BLOCK                                                
    char forBeginLabel[100], forEndLabel[100], condOp[100];                     
    Tree *stmt1 = node->childs->item[2],                                        // ���o�l�`�I                         
         *cond  = node->childs->item[4],                                                                           
         *stmt2 = node->childs->item[6],                                                                           
         *block = node->childs->item[8];                                                                           
    GenCode(g, stmt1, nullVar);                                                 // ���j���� STMT                             
    int tempForCount = g->forCount++;                                           // �]�wFOR�j�骺                 
    sprintf(forBeginLabel, "FOR%d", tempForCount);                              //   �i�J�аO                                
    sprintf(forEndLabel, "_FOR%d", tempForCount);                               //   ���}�аO            
    GenPcode(g, forBeginLabel, "", "", "", "");                                 // �����X�G�Ҧp FOR1:    
    GenCode(g, cond, condOp);                                                   // ���j���� COND        
    char negOp[100];                                                                                  
    negateOp(condOp, negOp);                                                    // ���ɹB��negOp         
    GenPcode(g, "", "J", negOp, "", forEndLabel);                               // �����X�G�ҦpJ > _FOR1 
    GenCode(g, block, nullVar);                                                 // ���j���� BLOCK       
    GenCode(g, stmt2, nullVar);                                                 // ���j���� STMT        
    GenPcode(g, "", "J", "", "", forBeginLabel);                                // �����X�G�ҦpJ FOR1    
    GenPcode(g, forEndLabel, "", "", "", "");                                   // �����X�G�Ҧp _FOR1    
    return NULL;                                                                                      
  } else if (strEqual(node->type, "STMT")) {                                    // �B�z STMT �`�I          
    //   STMT = return id | id '=' EXP | id ('++'|'--')                                              
    Tree *c1 = node->childs->item[0];                                           //   ���o�l�`�I              
    if (strEqual(c1->type, "return")) {                                         // �B�z return ���O                                
      Tree *id = node->childs->item[1];                                                                   
      GenPcode(g, "", "RET", "", "", id->value);                                // �����X�G �Ҧp RET sum      
    } else {                                                                                              
      Tree *id = node->childs->item[0];                                         //   ���o�l�`�I              
      Tree *op = node->childs->item[1];                                                                   
      if (strEqual(op->type, "=")) {                                            // �B�z id= EXP             
        // STMT �O id '=' EXP                                                   //  ���o�l�`�I               
        Tree *exp = node->childs->item[2];                                                                
        char expVar[100];                                                          
        GenCode(g, exp, expVar);                                                //  ���j���� EXP            
        GenPcode(g, "", "=", expVar, "", id->value);                            //  �����X�G�Ҧp = 0 sum     
        HashTablePut(g->symTable, id->value, id->value);                        //  �N id �[�J��Ÿ�����                              
        strcpy(rzVar, expVar);                                                  //  �Ǧ^ EXP ���ܼơA�Ҧp T0  
      } else { // STMT �O id++ �� id--�A--> id OP1                              // �B�z id++ �� id--         
        char addsub[100];                                                                                 
        if (strEqual(op->value, "++"))                                          //  �p�G�O id++              
          strcpy(addsub, "+");                                                  //   �]�w�B�⬰ + �k         
        else                                                                    //  �_�h                     
          strcpy(addsub, "-");                                                  //   �]�w�B�⬰ - �k         
        GenPcode(g, "", addsub, id->value, "1", id->value);                     //  �����X�G�Ҧp ADD i, 1, i 
        strcpy(rzVar, id->value);                                               //  �Ǧ^id�A�Ҧp i           
      }                                                                         
    }                                                                           
  } else if (strEqual(node->type, "COND")) {                                    // �B�z COND �`�I      
    // �B�z�P�_�� COND = EXP ('=='|'!='|'<='|'>='|'<'|'>') EXP                                
    Tree* op = node->childs->item[1];                                           // ���o�l�`�I            
    char expVar1[100], expVar2[100];                                                                  
    GenCode(g, node->childs->item[0], expVar1);                                 //  ���j���� EXP       
    GenCode(g, node->childs->item[2], expVar2);                                 //  ���j���� EXP       
    GenPcode(g, "", "CMP", expVar1, expVar2, nullVar);                          //  �����X�G�Ҧp CMP i,10 
    strcpy(rzVar, op->value); // �Ǧ^���L�B��l                                 //  �Ǧ^op�A�Ҧp >       
  } else if (strPartOf(node->type, "|EXP|")) {                                  // �B�z EXP
    // �B�z�B�⦡ EXP = ITEM ([+-*/] ITEM)*                                               
    Tree *item1 = node->childs->item[0];                                        // ���o�l�`�I            
    char var1[100], var2[100], tempVar[100];                                                          
    GenCode(g, item1, var1);                                                    // ���j���� ITEM
    if (node->childs->count > 1) {
      Tree* op = node->childs->item[1];                                        // �s����o (op ITEM)? 
      Tree* item2 = node->childs->item[2];                                                         
      GenCode(g, item2, var2);                                                  // ���j���� ITEM        
      GenTempVar(g, tempVar);                                                   // ���o�{���ܼơA�ҦpT0  
      GenPcode(g, "", op->value, var1, var2, tempVar);                          // �����X�G�Ҧp + sum i T0
      strcpy(var1, tempVar);                                                    // �Ǧ^�{���ܼơA�Ҧp T0  
    }                                                                                                 
    strcpy(rzVar, var1);                                                        // �Ǧ^�{���ܼơA�Ҧp T0  
  } else if (strPartOf(node->type, "|number|id|")) {                            // �B�z number, id �`�I
    // �J���ܼƩα`�ơA�Ǧ^�� value �W�١C                                                         
    strcpy(rzVar, node->value);                                                 // �����Ǧ^ id �� number
  } else if (node->childs != NULL) {                                            // ��L���p           
    // ��L���p�A�Y���l�N�h���^�B�z                                                                
    int i;                                                                                         
    for (i=0; i<node->childs->count; i++)                                       // ���j�B�z�Ҧ��l�`�I
      GenCode(g, node->childs->item[i], nullVar);
  }
  return NULL;
}

void GenData(Generator *g) { // ���ͲզX�y�����ܼƫŧi 
  Array *symArray = HashTableToArray(g->symTable);
  int i;
  for (i=0; i<symArray->count; i++) { // ���ͲŸ��� 
    char *varName = symArray->item[i];
    char varLabel[100];
    sprintf(varLabel, "%s:", varName);
    GenAsmCode(g, varLabel, "RESW", "1", "", "");
  }
  for (i=0; i<g->varCount; i++) { // �����{���ܼ� 
    char tVarLabel[100];
    sprintf(tVarLabel, "T%d:", i);
    GenAsmCode(g, tVarLabel, "RESW", "1", "", "");
  }
  ArrayFree(symArray, NULL);
}

void GenPcode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo) {        // ��Xpcode��A�ର�զX�y��        
  char labelTemp[100];                                                                                                      
  if (strlen(label)>0)                                                                                                      
    sprintf(labelTemp, "%s:", label);                                                                                       
  else                                                                                                                      
    strcpy(labelTemp, "");                                                                                                  
  printf("%-8s %-4s %-4s %-4s %-4s\n", labelTemp, op, p1, p2, pTo);                        // �L�X pcode                       
  GenPcodeToAsm(g, labelTemp, op, p1, p2, pTo);                                            // �N pcode �ର�զX�y��                                 
}                                                                                                        
                                                                                                                            
void GenPcodeToAsm(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo) {   // �N pcode �ର�զX�y�������
  if (strlen(label)>0)                                                                     // �p�G���аO                                       
    GenAsmCode(g, label, "", "", "", "");                                                  //  ��X�аO                          
  if (strEqual(op, "=")) { // pTo = p1                                                     // �B�z���� (= 0 sum)                
    GenAsmCode(g, "", "LD", "R1", p1, "");                                                 // �ন LDI R1, 0                   
    GenAsmCode(g, "", "ST", "R1", pTo, "");                                                //      ST R1, sum                  
  } else if (strPartOf(op, "|+|-|*|/|")) { // pTo = p1 op p2                               // �B�z�B��(+ sum i sum)            
    char asmOp[100];                                                                                                        
    if (strEqual(op, "+")) strcpy(asmOp, "ADD");                                           // �ھ� op �]�w�B����O             
    else if (strEqual(op, "-")) strcpy(asmOp, "SUB");                                                                       
    else if (strEqual(op, "*")) strcpy(asmOp, "MUL");                                                                       
    else if (strEqual(op, "/")) strcpy(asmOp, "DIV");                                                                       
    GenAsmCode(g, "", "LD", "R1", p1, "");                                                 // �ন LD R1, sum                  
    GenAsmCode(g, "", "LD", "R2", p2, "");                                                 //      LD R2, i                    
    GenAsmCode(g, "", asmOp,"R3", "R1", "R2");                                             //      ADD R3, R1, R2              
    GenAsmCode(g, "", "ST", "R3", pTo, "");                                                //      ST R3, sum                  
  } else if (strEqual(op, "CMP")) { // CMP p1, p2                                          // �B�z CMP (cmp i 10)              
    GenAsmCode(g, "", "LD", "R1", p1, "");                                                 // �ন LD R1, i                    
    GenAsmCode(g, "", "LD", "R2", p2, "");                                                 //      LDI R2, 10                  
    GenAsmCode(g, "", "CMP", "R1", "R2", "");                                              //      CMP R1, R2                  
  } else if (strEqual(op, "J")) { // J op label                                            // �B�z J (J > _FOR)                
    char asmOp[100];                                                                       // �ھ� op �]�w���D���O             
    if (strEqual(p1, "=")) strcpy(asmOp, "JEQ");                                                   
    else if (strEqual(p1, "!=")) strcpy(asmOp, "JNE");                                                                      
    else if (strEqual(p1, "<")) strcpy(asmOp, "JLT");                                                                       
    else if (strEqual(p1, ">")) strcpy(asmOp, "JGT");                                                                       
    else if (strEqual(p1, "<=")) strcpy(asmOp, "JLE");                                                                      
    else if (strEqual(p1, ">=")) strcpy(asmOp, "JGE");                                                                      
    else strcpy(asmOp, "JMP");                                                                                              
    GenAsmCode(g, "", asmOp, pTo, "", "");                                                                                   
  } else if (strEqual(op, "RET")) {                                                        // �B�z RET sum                     
    GenAsmCode(g, "", "LD", "R1", pTo, "");                                                // �ন LD R1, sum                  
    GenAsmCode(g, "", "RET", "", "", "");                                                  //      RET                         
  }                                                                                                         
}                                                                                                  
                                                                                                                            
void GenAsmCode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo) {      // ��X�զX�y�����O                                     
  char *realOp = op;                                                                                                                            
  if (strEqual(op, "LD"))                                                                                                   
    if (isdigit(p2[0]))                                                                    // �p�G���O�O LD�A�ӥB              
      realOp = "LDI";                                                                      //   p2 ���`��                      
  fprintf(g->asmFile, "%-8s %-4s %-4s %-4s %-4s\n", label, realOp, p1, p2, pTo);           //    ��� LDI ���O                 
}                                                                                          // ��X�զX�y�����O                 
                                                                                                        
void GenTempVar(Generator *g, char *tempVar) {                                                          
  sprintf(tempVar, "T%d", g->varCount++);
}

void negateOp(char *condOp, char *negOp) {
  if (strEqual(condOp, "==")) strcpy(negOp, "!=");
  if (strEqual(condOp, "!=")) strcpy(negOp, "==");
  if (strEqual(condOp, ">=")) strcpy(negOp, "<");
  if (strEqual(condOp, "<=")) strcpy(negOp, ">");
  if (strEqual(condOp, ">")) strcpy(negOp, "<=");
  if (strEqual(condOp, "<")) strcpy(negOp, ">=");
}
