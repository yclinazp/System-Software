#include "Assembler.h"                                          // �ޥβ�Ķ�����Y                
#include "Compiler.h"                                           // �ޥνsĶ�����Y                

#define TEST      1                                             // �sĶ�ؼ� 1: test               
#define C0C       2                                             // �sĶ�ؼ� 2: c0c                
#define AS0       3                                             // �sĶ�ؼ� 3: as0                
#define VM0       4                                             // �sĶ�ؼ� 4: vm0                
                                                                                              
void argError(char *msg) {                                      // �B�z�Ѽƿ��~�����p            
  printf("%s\n", msg);                                                                        
  exit(1);                                                                                    
}                                                                                             
                                                                                              
int main(int argc, char *argv[]) {                              // �D�{���}�l                    
  char cFile0[]="test.c0", *cFile=cFile0;                       //  �w�]�{���ɬ� test.c0         
  char asmFile0[]="test.asm0", *asmFile=asmFile0;               //  �w�]�զX�y����test.asm0      
  char objFile0[]="test.obj0", *objFile=objFile0;               //  �w�]�ت��ɬ� test.obj0       
#if TARGET==TEST                                                // �p�G�sĶ�ؼЬ� TEST           
  ArrayTest();                                                  //   ���հ}�C����                
  HashTableTest();                                              //   �������������              
  OpTableTest();                                                //   ���ի��O������              
  compile(cFile, asmFile);                                      //   ���սsĶ��                  
  assemble(asmFile, objFile);                                   //   ���ղ�Ķ��                  
  runObjFile(objFile);                                          //   ���յ�������                
  checkMemory();                                                //   �ˬd�O����ϥΪ��p          
#elif TARGET==C0C                                               // �p�G�sĶ�ؼЬ� C0C            
  if (argc == 3) {                                              //  �p�G�� 3 �ӰѼ�             
    cFile=argv[1]; asmFile=argv[2];                             //    �]�w�Ѽ�                  
  } else                                                        //  �_�h                        
    argError("c0c <c0File> <asmFile>");                         //    ���ܵ{�������k          
  compile(cFile, asmFile);                                      //  �}�l�sĶ                    
#elif TARGET==AS0                                               // �p�G�sĶ�ؼЬ� AS0            
  if (argc == 3) {                                              //  �p�G�� 3 �ӰѼ�             
    asmFile=argv[1]; objFile=argv[2];                           //    �]�w�Ѽ�                  
  } else                                                        //  �_�h                        
    argError("as0 <asmFile> <objFile>");                        //    ���ܵ{�������k          
  assemble(asmFile, objFile);                                   //  �}�l��Ķ                    
#elif TARGET==VM0                                               // �p�G�sĶ�ؼЬ� VM0            
  if (argc == 2)                                                //  �p�G�� 2 �ӰѼ�             
    objFile=argv[1];                                            //    �]�w�Ѽ�                  
  else                                                          //  �_�h                        
    argError("vm0 <objFile>");                                  //    ���ܵ{�������k          
  runObjFile(objFile);                                          //  �}�l���� (������)           
#endif                                                                                        
  //system("pause");                                              // �Ȱ� (�� Dev C++ �ϥΪ�)      
  return 0;
}
