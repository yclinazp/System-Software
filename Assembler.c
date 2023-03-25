#include "Assembler.h"

void assemble(char *asmFile, char *objFile) {                   
  printf("Assembler:asmFile=%s objFile=%s\n", asmFile,objFile); 
  printf("===============Assemble=============\n");
  char *text = newFileStr(asmFile);                             // 讀取檔案到text字串中
  Assembler *a = AsmNew();                                      // allocate Assembler type memory
  AsmPass1(a, text);                                            // 編譯第一階段 
  printf("===============SYMBOL TABLE=========\n");             
  HashTableEach(a->symTable, (FuncPtr1) AsmCodePrintln);        // print symboltable
  AsmPass2(a);                                                  // 編譯第二階段 
  AsmSaveObjFile(a, objFile);                                   // 將objCode(hexa)輸出objfile(binary)  
  AsmFree(a);                                                   // free a  
  freeMemory(text);                                             // free text   
}                  

//allocate Assembler type memory
Assembler* AsmNew() {
  Assembler *a=ObjNew(Assembler, 1);//allocate Assembler type*1 count memory
  a->codes = ArrayNew(10); //allocate Array (item = 10) memory
  a->symTable = HashTableNew(127); //allocate (item = 127) hashtable (2D Array) memory
  a->opTable = OpTableNew(); //將opList轉換成Op type(*name, code, type)放入hashtable(op->name, op)
  return a;
}

void AsmFree(Assembler *a) {
  ArrayFree(a->codes, (FuncPtr1) AsmCodeFree);
  HashTableFree(a->symTable);
  OpTableFree();
  ObjFree(a);
}

//解析每一行opcode, type, args, label,計算每一行address, 將label放入symboltable
void AsmPass1(Assembler *a, char *text) {                     
  int i, address = 0, number;                                                 
  Array* lines = split(text, "\r\n", REMOVE_SPLITER); // 將assembly code切成一行一行 (換行char \r\n)
  ArrayEach(lines, strPrintln);                       // print每一行string且換行           
  printf("=================PASS1================\n");               
  for (i=0; i<lines->count; i++) {                    // 對每一行                        
      strReplace(lines->item[i], SPACE, ' ');         // 將\t\n\r換成空char        
      AsmCode *code = AsmCodeNew(lines->item[i]);     // 解析line將label, op, args放到Asmcode type
      code->address = address;                        // 設定該行的address
      Op *op = HashTableGet(opTable, code->op);       // 利用code->op(key) 到hashtable get opcode and type(L,A,J)      
      if (op != NULL) {                               // if key exist
        code->opCode = op->code;                      // setting opCode (00[LD], 01[ST])
        code->type = op->type;                        // setting type(L,A,J)   
      } 
      // 將label放入symboltable                                                 
      if (strlen(code->label)>0)                      // if含有symbol
        HashTablePut(a->symTable, code->label, code); // put into symbol table
      ArrayAdd(a->codes, code);                       // 將Asmcode code(item) 放入Assenbler codes(Array)
      AsmCodePrintln(code);                           // print Asmcode code
      code->size = AsmCodeSize(code);                 // 計算指令所佔bytes大小
      address += code->size;                          // 計算出下一個instruction address
  }                                                                           
  ArrayFree(lines, strFree);                          // free Array lines
}

void AsmPass2(Assembler *a) {                          
  printf("=============PASS2==============\n");                       
  int i;                                                              
  for (i=0; i<a->codes->count; i++) {                 // 對每一指令 (Array數量)   
    AsmCode *code = a->codes->item[i];                                
    AsmTranslateCode(a, code);                        // 編成object code  
    AsmCodePrintln(code);                  
  }
}
//轉換成objCode(hexa)
void AsmTranslateCode(Assembler *a, AsmCode *code) {                                  
  char p1[100], p2[100], p3[100], pt[100];                                                       
  int ra=0, rb=0, rc=0, cx=0;                                                  
  char cxCode[9]="00000000", objCode[100]="", args[100]="";                    
  strcpy(args, code->args);                                                    
  strReplace(args, ",", ' ');                                                  
  int pc = code->address + 4;                                              // fecth instruction之後PC+4         
  switch (code->type) {                                                                 
    case 'J' :                                                             // 處理J type           
      if (!strEqual(args, "")) {                                           // if args不為空字串(長度為0)                         
        AsmCode *labelCode = HashTableGet(a->symTable,args);               // 取得symbol address        
        cx = labelCode->address - pc;                                      // 計算cx (PC相對定址法)           
        sprintf(cxCode, "%8x", cx);                                                                 
      }                                                                                             
      sprintf(objCode, "%2x%s", code->opCode, &cxCode[2]);                 // opcode(2碼)+從cx第2到第8個元素(共六碼)     
      break;                                                                                        
    case 'L' :                                                             // 處理L type                         
      sscanf(args, "R%d %s", &ra, p2);                                     // sscanf 把args字串解析成格式化輸入
      if (strHead(p2, "[")) {                                              // if p2以"["開頭 (表示是memory location [Ra])
        sscanf(p2, "[R%d+%s]", &rb, pt);                                   // 再取出Rb pt(Cx)  
        if (sscanf(pt, "R%d", &rc)<=0)                                         
          sscanf(pt, "%d", &cx);                                           // 將pt assign給cx    
      } else if (sscanf(p2, "%d", &cx)>0) {                                // if p2可以轉換成十進位, 將p2給cx  
      } else {                                                             
        AsmCode *labelCode = HashTableGet(a->symTable, p2);                // find symbol p2 in symboltable 
        cx = labelCode->address - pc;                                      // 計算cx (PC相對定址法)  
        rb = 15; // R[15] is PC                                            // Rb = PC
      }                                                                         
      sprintf(cxCode, "%8x", cx);                                          // sprintf將cx string轉換成八位hexo   
      sprintf(objCode, "%2x%x%x%s", code->opCode, ra, rb, &cxCode[4]);     // 取cx後四碼
      break;                                                               
    case 'A' :                                                             // 處理A type      
      sscanf(args, "%s %s %s", p1, p2, p3);                                         
      sscanf(p1, "R%d", &ra);                                              // 取得Ra     
      sscanf(p2, "R%d", &rb);                                              // 取得Rb         
      if (sscanf(p3, "R%d", &rc)<=0)                                              
        sscanf(p3, "%d", &cx);                                             // 取得Rb,不是R開頭則轉換成cx        
      sprintf(cxCode, "%8x", cx);                                                                   
      sprintf(objCode, "%2x%x%x%x%s", code->opCode,ra,rb,rc,&cxCode[5]);   // 取cx後3碼    
      break;                                                                                        
    case 'D' : {                                                           // 處理D type (資料宣告)                                
      char format4[]="%8x", format1[]="%2x", *format = format1;            
      switch (code->opCode) {                                                  
        case OP_RESW:                                                              
        case OP_RESB:                                                      // opCode=RESB+RESW        
          memset(objCode, '0', code->size*2);                              // set objCode all to 0
          objCode[code->size*2] = '\0';
          break;                                                                       
        case OP_WORD:                                                      // opCode= WORD and BYTE      
          format = format4;                                                // 輸出%8x
        case OP_BYTE: {                                                   
          Array *array = split(args, " ", REMOVE_SPLITER);                 // 以空格為分隔符號拆解args成tokens  
          char *objPtr = objCode;
          int i=0;
          for (i=0; i<array->count; i++) {                                 // 對每一行tokens
              char *item = array->item[i];
              if (isdigit(item[0]))                                        // if item第一個為數字
                sprintf(objPtr, format, atoi(item));                       // 轉換成整數, 輸出成%8x形式至objPtr
              else {                                                       // if item為symbol
                AsmCode *itemCode = HashTableGet(a->symTable, item);       // 到symboltable尋找
                sprintf(objPtr, format, itemCode->address);                // 將symbol對應的address, 輸出成%8x形式至objPtr
              }
              objPtr += strlen(objPtr);                                    // 將objPtr移動到後面char位置, 以便下一行tokens儲存char
          }
          ArrayFree(array, strFree);
          break;//for opCode= WORD and BYTE
        } 
      } 
      break;//for case 'D'
    } 
    default: 
      strcpy(objCode, "");                                                 // 非以上type,objcode=空字串
      break;//for switch code->type
  }
  strReplace(objCode, " ", '0');                                           // replace 空格 with 0 (objCode固定長度為8)
  strToUpper(objCode);                                                     // 轉大寫
  code->objCode = newStr(objCode);                                         // store at AsmCode objCode
}
//將objCode(hexa)輸出objfile(binary)
void AsmSaveObjFile(Assembler *a, char *objFile) {
  printf("==========Save to ObjFile:%s==========\n", objFile);
  FILE *file = fopen(objFile, "wb");                                       // open with binary mode
  int i;
  for (i=0; i<a->codes->count; i++) {                                      // 對每一行AsmCode code
    AsmCode *code = a->codes->item[i];
    char *objPtr = code->objCode;
    while (*objPtr != '\0') {                                              // 用objPtr go through 整個objCode
      int x; 
      sscanf(objPtr, "%2x", &x);                                           // 從objPtr讀取2位hex(=8位binary,= 1byte) 放到x
      assert(x >= 0 && x < 256);                                           // 確保轉換後的x在一個byte範圍內
      BYTE b = (BYTE) x;                                                   // 將x轉換成1 byte b
      fwrite(&b, sizeof(BYTE), 1, file);                                   // write b in file
      objPtr += 2;                                                         // objPtr move to next byte
      char bstr[3];                                                        
      sprintf(bstr, "%2x", b);                                             // 將b=15轉換成string bstr="0F"
      strReplace(bstr, " ", '0');                                          // 空格轉成0       
      strToUpper(bstr);                                                    // 轉大寫  
      printf("%s", bstr);                                                  // 印出string bstr
    }
  }
  printf("\n");
  fclose(file);
}

int AsmCodePrintln(AsmCode *code) {
  char label[100] = "", address[100], buffer[200];
  if (strlen(code->label)>0) 
    sprintf(label, "%s:", code->label); //輸出%s,將格式化輸出的結果字串到label
  sprintf(address, "%4x", code->address);//輸出佔4位hexo,將格式化輸出的結果字串到address
  strReplace(address, " ", '0'); //將空格換成0
  //args (%-14s)會輸出佔14位string 存在buffer
  sprintf(buffer, "%s %-8s %-4s %-14s %c %2x %s\n", address, label, code->op, code->args, code->type, code->opCode, code->objCode);
  strToUpper(buffer);//將buffer轉成大寫字母
  printf(buffer);
}

AsmCode* AsmCodeNew(char *line) {
  AsmCode* code = ObjNew(AsmCode,1);//allocate AsmCode type*1 count memory
  char label[100]="", op[100]="", args[100]="", temp[100];
  int count = sscanf(line, "%s %s %[^;]", label, op, args);//將line解析成label, op, args(args:非分號的字元)
  if (strTail(label, ":")) { //label的最後等於":"
    strTrim(temp, label, ":"); //remove label的":",放在temp
    strcpy(label, temp); //放回label
  } else {
    strcpy(label, ""); //label = NULL
    sscanf(line, "%s %[^;]", op, args); //label已為NULL,重新解析 line 字串，將 op、args 存到對line
  }
//  printf("label=%s op=%s args=%s\n", code->label, op, args);
  code->label = newStr(label); // 將 label、op、args 存到 AsmCode
  code->op = newStr(op);
  strTrim(temp, args, SPACE);//remove args的"\t\n\r",放在temp
  code->args = newStr(temp);
  code->type = ' ';//設為空char
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

// 計算AsmCode指令大小, CPU0每個指令佔4 bytes
int AsmCodeSize(AsmCode *code) {                        
  switch (code->opCode) {    //code->opCode = (LD=00, ST=01)                                          
    case OP_RESW :                                    
      return 4 * atoi(code->args);                  // 將args整數string轉成整數*4 (byte)
    case OP_RESB :                                          
      return atoi(code->args);                      // 將args整數string轉成整數*1 (byte)
    case OP_WORD :                                        
      return 4 * (strCountChar(code->args, ",")+1); // 紀錄","出現次數, args個數*4  (byte)
    case OP_BYTE :                                      
      return strCountChar(code->args, ",")+1;       // args個數*1  (byte)              
    case OP_NULL :                                  // OP_NULL = 0xFF           
      return 0;                                              
    default :                                      
      return 4;                                     // 4 byte
  }                                                   
}
