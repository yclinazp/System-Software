#include "Assembler.h"                                                       
#include "Compiler.h"                                                         

#define TEST      1                                             // 編譯目標1       
#define C0C       2                                             // 編譯目標2                
#define AS0       3                                             // 編譯目標3                 
#define VM0       4                                             // 編譯目標4                
                                                                                              
void argError(char *msg) {                                      // 處理command參數錯誤狀況           
  printf("%s\n", msg);                                                                        
  exit(1);                                                      // 結束程式執行,回傳1                             
}                                                                                             
                                                                                              
int main(int argc, char *argv[]) {                                                  
  char cFile0[]="test.c0", *cFile=cFile0;                       // 預設程式檔為test.c0         
  char asmFile0[]="test.asm0", *asmFile=asmFile0;               // 預設組合語言為test.asm0      
  char objFile0[]="test.obj0", *objFile=objFile0;               // 預設目的檔為test.obj0       
#if TARGET==TEST                                                // 如果編譯目標為TEST,則執行#if後續程式            
  ArrayTest();                                                  // 測試陣列               
  HashTableTest();                                              // 測試雜湊表              
  OpTableTest();                                                // 測試指令表              
  compile(cFile, asmFile);                                      // 測試編譯器                
  assemble(asmFile, objFile);                                   // 測試組譯器              
  runObjFile(objFile);                                          // 測試虛擬機器                
  checkMemory();                                                // 檢查記憶體使用狀況         
#elif TARGET==C0C                                               // 如果編譯目標為C0C,則執行#elif後續程式            
  if (argc == 3) {                                                         
    cFile=argv[1]; asmFile=argv[2];                             // 設定檔案為參數                
  } else                                                                           
    argError("c0c <c0File> <asmFile>");                         // 否則 提示正確command方法       
  compile(cFile, asmFile);                                      // 開始編譯                   
#elif TARGET==AS0                                               // 如果編譯目標為AS0,則執行#elif後續程式            
  if (argc == 3) {                                                            
    asmFile=argv[1]; objFile=argv[2];                           // 設定檔案為參數                 
  } else                                                                              
    argError("as0 <asmFile> <objFile>");                        // 否則 提示正確command方法         
  assemble(asmFile, objFile);                                   // 開始組譯                    
#elif TARGET==VM0                                               // 如果編譯目標為VM0,則執行#elif後續程式             
  if (argc == 2)                                                           
    objFile=argv[1];                                            // 設定檔案為參數                   
  else                                                                                
    argError("vm0 <objFile>");                                  // 否則 提示正確command方法            
  runObjFile(objFile);                                          // 開始執行虛擬機          
#endif                                                                                            
  return 0;
}
