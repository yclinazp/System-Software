#include "Assembler.h"

void assemble(char *asmFile, char *objFile) {                   // ��Ķ�����D�n���
  printf("Assembler:asmFile=%s objFile=%s\n", asmFile,objFile); // ��J�զX�y���B��X�ت���
  printf("===============Assemble=============\n");
  char *text = newFileStr(asmFile);                             // Ū���ɮר� text �r�ꤤ
  Assembler *a = AsmNew();                                      
  AsmPass1(a, text);                                            // �Ĥ@���q�G�p���} 
  printf("===============SYMBOL TABLE=========\n");             
  HashTableEach(a->symTable, (FuncPtr1) AsmCodePrintln);        // �L�X�Ÿ���   
  AsmPass2(a);                                                  // �ĤG���q�G�غc�ت��X 
  AsmSaveObjFile(a, objFile);                                      
  AsmFree(a);                                                   // ��X�ت���   
  freeMemory(text);                                             // ����O����   
}                                                               

Assembler* AsmNew() {
  Assembler *a=ObjNew(Assembler, 1);
  a->codes = ArrayNew(10);
  a->symTable = HashTableNew(127);
  a->opTable = OpTableNew();
  return a;
}

void AsmFree(Assembler *a) {
  ArrayFree(a->codes, (FuncPtr1) AsmCodeFree);
  HashTableFree(a->symTable);
  OpTableFree();
  ObjFree(a);
}

void AsmPass1(Assembler *a, char *text) {             // �Ĥ@���q����Ķ           
  int i, address = 0, number;                                                 
  Array* lines = split(text, "\r\n", REMOVE_SPLITER); // �N�զX�y�����Φ��@��@��
  ArrayEach(lines, strPrintln);                       // �L�X�H�K�[��           
  printf("=================PASS1================\n");               
  for (i=0; i<lines->count; i++) {                    // ���C�@��                        
      strReplace(lines->item[i], SPACE, ' ');                   
      AsmCode *code = AsmCodeNew(lines->item[i]);     // �إ߫��O����
      code->address = address;                        // �]�w�Ӧ檺��}      
      Op *op = HashTableGet(opTable, code->op);       // �d�߹B��X            
      if (op != NULL) {                               // �p�G�d��
        code->opCode = op->code;                      //    �]�w�B��X
        code->type = op->type;                        //    �]�w���A
      }                                                  
      if (strlen(code->label)>0)                      // �p�G���аO�Ÿ�
        HashTablePut(a->symTable, code->label, code); //    �[�J�Ÿ���
      ArrayAdd(a->codes, code);                       //  �غc���O����}�C
      AsmCodePrintln(code);                           //    �L�X�[��
      code->size = AsmCodeSize(code);                 //  �p����O�j�p
      address += code->size;                          //  �p��U�@�ӫ��O��}
  }                                                                           
  ArrayFree(lines, strFree);                          // ����O����
}

void AsmPass2(Assembler *a) {                         // ��Ķ�����ĤG���q
  printf("=============PASS2==============\n");                       
  int i;                                                              
  for (i=0; i<a->codes->count; i++) {                 // ��C�@�ӫ��O    
    AsmCode *code = a->codes->item[i];                                
    AsmTranslateCode(a, code);                        //   �i��s�X�ʧ@  
    AsmCodePrintln(code);
  }
}

void AsmTranslateCode(Assembler *a, AsmCode *code) {                       // ���O���s�X���           
  char p1[100], p2[100], p3[100], pt[100];                                                       
  int ra=0, rb=0, rc=0, cx=0;                                                  
  char cxCode[9]="00000000", objCode[100]="", args[100]="";                    
  strcpy(args, code->args);                                                    
  strReplace(args, ",", ' ');                                                  
  int pc = code->address + 4;                                              // ������PC����}+4         
  switch (code->type) {                                                    // �ھګ��O���A             
    case 'J' :                                                             // �B�z J �����O              
      if (!strEqual(args, "")) {                                                                    
        AsmCode *labelCode = HashTableGet(a->symTable,args);               //   ���o�Ÿ���}            
        cx = labelCode->address - pc;                                      //   �p�� cx ���            
        sprintf(cxCode, "%8x", cx);                                                                 
      }                                                                                             
      sprintf(objCode, "%2x%s", code->opCode, &cxCode[2]);                 //   �s�X�ت��X(16�i��)      
      break;                                                                                        
    case 'L' :                                                                                      
      sscanf(args, "R%d %s", &ra, p2);                                        
      if (strHead(p2, "[")) {                                                  
        sscanf(p2, "[R%d+%s]", &rb, pt);                                       
        if (sscanf(pt, "R%d", &rc)<=0)                                         
          sscanf(pt, "%d", &cx);                                               
      } else if (sscanf(p2, "%d", &cx)>0) {                                    
      } else {                                                             
        AsmCode *labelCode = HashTableGet(a->symTable, p2);                    
        cx = labelCode->address - pc;                                      
        rb = 15; // R[15] is PC                                            
      }                                                                         
      sprintf(cxCode, "%8x", cx);                                              
      sprintf(objCode, "%2x%x%x%s", code->opCode, ra, rb, &cxCode[4]);         
      break;                                                               
    case 'A' :                                                             // �B�z A �����O         
      sscanf(args, "%s %s %s", p1, p2, p3);                                //   ���o�Ѽ�            
      sscanf(p1, "R%d", &ra);                                              //   ���ora�Ȧs���N��    
      sscanf(p2, "R%d", &rb);                                              //   ���orb�Ȧs���N��    
      if (sscanf(p3, "R%d", &rc)<=0)                                       //   ���orc�Ȧs���N��    
        sscanf(p3, "%d", &cx);                                             //   �Ϊ̬O cx �Ѽ�      
      sprintf(cxCode, "%8x", cx);                                                                   
      sprintf(objCode, "%2x%x%x%x%s", code->opCode,ra,rb,rc,&cxCode[5]);   //   �s�X�ت��X(16�i��)  
      break;                                                                                        
    case 'D' : {                                                           // �B�z�O��ƫŧi                             
      // �ڭ̱N��ƫŧi  RESW, RESB, WORD, BYTE �]�����@�ث��O�A��κA�� D
      char format4[]="%8x", format1[]="%2x", *format = format1;            
      switch (code->opCode) {                                              // �p�G�O RESW    
        case OP_RESW:                                                      //       �� RESB 
        case OP_RESB:                                                      //         
          memset(objCode, '0', code->size*2);                              // �ت��X�� 0000�K.
          objCode[code->size*2] = '\0';
          break;                                                           // �p�G�O WORD:            
        case OP_WORD:                                                                
          format = format4;                                                // �]�w��X�榡�� %8x 
        case OP_BYTE: {                                                    // �p�G�O BYTE : ��X�榡�� %2x
          Array *array = split(args, " ", REMOVE_SPLITER);                 //   ��ت��X���C�ӼƦr�ର16�i�쪺���G
          char *objPtr = objCode;
          int i=0;
          for (i=0; i<array->count; i++) {                                 
              char *item = array->item[i];
              if (isdigit(item[0]))
                sprintf(objPtr, format, atoi(item));
              else {
                AsmCode *itemCode = HashTableGet(a->symTable, item);
                sprintf(objPtr, format, itemCode->address);
              }
              objPtr += strlen(objPtr);
          }
          ArrayFree(array, strFree);
          break;
        } // case OP_BYTE:
      } // switch
      break;
    } // case 'D'
    default: 
      strcpy(objCode, "");
      break;
  }
  strReplace(objCode, " ", '0');
  strToUpper(objCode);
  code->objCode = newStr(objCode);
}

void AsmSaveObjFile(Assembler *a, char *objFile) {
  printf("==========Save to ObjFile:%s==========\n", objFile);
  FILE *file = fopen(objFile, "wb");
  int i;
  for (i=0; i<a->codes->count; i++) {
    AsmCode *code = a->codes->item[i];
    char *objPtr = code->objCode;
    while (*objPtr != '\0') {
      int x;
      sscanf(objPtr, "%2x", &x);
      assert(x >= 0 && x < 256);
      BYTE b = (BYTE) x;
      fwrite(&b, sizeof(BYTE), 1, file);
      objPtr += 2;
      char bstr[3];
      sprintf(bstr, "%2x", b);
      strReplace(bstr, " ", '0');
      strToUpper(bstr);
      printf("%s", bstr);
    }
  }
  printf("\n");
  fclose(file);
}

int AsmCodePrintln(AsmCode *code) {
  char label[100] = "", address[100], buffer[200];
  if (strlen(code->label)>0)
    sprintf(label, "%s:", code->label);
  sprintf(address, "%4x", code->address);
  strReplace(address, " ", '0');
  sprintf(buffer, "%s %-8s %-4s %-14s %c %2x %s\n", address, label, code->op, code->args, code->type, code->opCode, code->objCode);
  strToUpper(buffer);
  printf(buffer);
}

AsmCode* AsmCodeNew(char *line) {
  AsmCode* code = ObjNew(AsmCode,1);
  char label[100]="", op[100]="", args[100]="", temp[100];
  int count = sscanf(line, "%s %s %[^;]", label, op, args);
  if (strTail(label, ":")) {
    strTrim(temp, label, ":");
    strcpy(label, temp);
  } else {
    strcpy(label, "");
    sscanf(line, "%s %[^;]", op, args);
  }
//  printf("label=%s op=%s args=%s\n", code->label, op, args);
  code->label = newStr(label);
  code->op = newStr(op);
  strTrim(temp, args, SPACE);
  code->args = newStr(temp);
  code->type = ' ';
  code->opCode = OP_NULL;
//  AsmCodePrintln(code);
  return code;
}

void AsmCodeFree(AsmCode *code) {
  freeMemory(code->label);
  freeMemory(code->op);
  freeMemory(code->args);
  freeMemory(code->objCode);
  freeMemory(code);
}

int AsmCodeSize(AsmCode *code) {                    // �p����O���j�p     
  switch (code->opCode) {                           // �ھڹB��X op                       
    case OP_RESW :                                  //  �p�G�ORESW       
      return 4 * atoi(code->args);                  //   �j�p�� 4*�O�d�q  
    case OP_RESB :                                  // �p�G�ORESB        
      return atoi(code->args);                      //   �j�p�� 1*�O�d�q  
    case OP_WORD :                                  // �p�G�OWORD        
      return 4 * (strCountChar(code->args, ",")+1); //   �j�p�� 4*�ѼƭӼ�
    case OP_BYTE :                                  // �p�G�OBYTE        
      return strCountChar(code->args, ",")+1;       //   �j�p��1*�ѼƭӼ�                  
    case OP_NULL :                                  // �p�G�u�O�аO                      
      return 0;                                     //   �j�p�� 0        
    default :                                       // ��L���� (���O)   
      return 4;                                     //   �j�p�� 4
  }                                                   
}
