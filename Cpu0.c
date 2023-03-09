#include "Cpu0.h"

void runObjFile(char *objFile) {                 
  printf("===VM0:run %s on CPU0===\n", objFile);               
  Cpu0 *cpu0 = Cpu0New(objFile);                 // create Cpu0 object,load objFile
  Cpu0Run(cpu0, 0);                              // 從0 address開始run
  Cpu0Dump(cpu0);                                // print all register
  Cpu0Free(cpu0);                                // free Cpu0 object 
}

Cpu0* Cpu0New(char *objFile) {                                
  Cpu0 *cpu0=ObjNew(Cpu0, 1);                    // allocate Cpu0 type*1 count memory
  cpu0->m = newFileBytes(objFile, &cpu0->mSize); // load objFile 放到m (memory array)
  return cpu0;                                                                
}                                                                             
                                                                              
void Cpu0Free(Cpu0 *cpu0) {                                        
  freeMemory(cpu0->m);                           
  ObjFree(cpu0);                                                                        
}                                                                                         
//#define UINT32  unsigned int                                                                                                                                                      
#define bits(i, from, to) ((UINT32) i << (31-to) >> (31-to+from)) // 取得from到to之間的位元 (31,30,...1,0)               
#define ROR(i, k) (((UINT32)i>>k)|(bits(i,32-k, 31)<<(32-k)))// 向右旋轉k位元                                         
#define ROL(i, k) (((UINT32)i<<k)|(bits(i,0,k-1)<<(32-k)))   // 向左旋轉k位元                                        
#define SHR(i, k) ((UINT32)i>>k)                             // 向右移位k           
#define SHL(i, k) ((UINT32)i<<k)                             // 向左移位k       
#define bytesToInt32(p) (INT32)(p[0]<<24|p[1]<<16|p[2]<<8|p[3])// 4byte轉(int) 32bits
#define bytesToInt16(p) (INT16)(p[0]<<8|p[1])                // 2byte轉(int) 16bits       
#define int32ToBytes(i, bp) { bp[0]=i>>24; bp[1]=i>>16; bp[2]=i>>8; bp[3]=i;} // int 32bits轉為4byte                                           
#define StoreInt32(i, m, addr) { BYTE *p=&m[addr]; int32ToBytes(i, p); } // instruction=m[addr...addr+3]                                      
#define LoadInt32(i, m, addr) { BYTE *p=&m[addr]; i=bytesToInt32(p); } // m[addr...addr+3]=instruction                                       
#define StoreByte(b, m, addr) { m[addr] = (BYTE) b; }        // m[addr]=b                                             
#define LoadByte(b, m, addr) { b = m[addr]; }                // b=m[addr]                 
                                                                                          
#define PC R[15]                                             // PC is R[15]                  
#define LR R[14]                                             //                    
#define SP R[13]                                             // Sack Pointer Register is R[13]                  
#define SW R[12]                                             // Satus Words is R[12]                  

void Cpu0Run(Cpu0 *cpu0, int start) {                                            
  char buffer[200];
  unsigned int IR, op, ra, rb, rc, cc;                                                                         
  int c5, c12, c16, c24, caddr, raddr;                                                                
  unsigned int N, Z;
  BYTE *m=cpu0->m;                                           // 設定起始address                                     
  int  *R=cpu0->R;                                           
  PC = start;                                                //                              
  LR = -1;                                                                                
  BOOL stop = FALSE;                                         //                                        
  while (!stop) {                                            // 如果尚未結束                          
    R[0] = 0;                                                //                           
    LoadInt32(IR, m, PC);                                    // fetch instruction, m[PC...PC+3]=IR                            
    cpu0->IR = IR;                                           // IR<-m[addr...addr+3]                      
    PC += 4;                                                 // 
    op = bits(IR, 24, 31);                                   // 取得op欄位
    ra = bits(IR, 20, 23);                                   // 取得Ra欄位
    rb = bits(IR, 16, 19);                                   // 取得Rb欄位
    rc = bits(IR, 12, 15);                                   // 取得Rc欄位
    c5 = bits(IR, 0, 4);                                     // 取得5, 12, 16, 24位元的Cx欄位
    c12= bits(IR, 0, 11);                                     
    c16= bits(IR, 0, 15);                                                              
    c24= bits(IR, 0, 23);                                                                              
    N  = bits(SW, 31, 31);                                   // 取得狀態暫存器中N旗標
    Z  = bits(SW, 30, 30);                                   // 取得狀態暫存器中Z旗標
    if (bits(IR, 11, 11)!=0) c12 |= 0xFFFFF000;              // 若為負數 則調整為2's complement                                       
    if (bits(IR, 15, 15)!=0) c16 |= 0xFFFF0000;              // if 第16bit=1, 將前面16bits設為1 保留後面16bits                                         
    if (bits(IR, 23, 23)!=0) c24 |= 0xFF000000;              // if 第23bit=1, 將前面24bits設為1 保留後面8bits                                                                                 
    caddr = R[rb]+c16;                                       // 取得位址[Rb+cx]                         
    raddr = R[rb]+R[rc];                                     // 取得位址[Rb+Rc]                         
    switch (op) {                                            // 根據op執行動作                           
      case OP_LD : LoadInt32(R[ra], m, caddr); break;                                     
      case OP_ST : StoreInt32(R[ra], m, caddr); break;                                 
      case OP_LDB: LoadByte(R[ra], m, caddr); break;                                   
      case OP_STB: StoreByte(R[ra], m, caddr); break;                                   
      case OP_LDR: LoadInt32(R[ra], m, raddr); break;                                  
      case OP_STR: StoreInt32(R[ra], m, raddr); break;                                  
      case OP_LBR: LoadByte(R[ra], m, raddr); break;                                    
      case OP_SBR: StoreByte(R[ra], m, raddr); break;                                   
      case OP_LDI: R[ra] = c16; break;                                                 
      case OP_CMP: {                                           
        if (R[ra] > R[rb]) {                                 // 大於
          SW &= 0x3FFFFFFF;                                  // 設定N=0, Z=0
        } else if (R[ra] < R[rb]) {                          // 小於                                                
          SW |= 0x80000000;                                  // 設定N=1
          SW &= 0xBFFFFFFF;                                  // 設定Z=0
        } else {                                             // 等於                
          SW &= 0x7FFFFFFF;                                  // 設定N=0
          SW |= 0x40000000;                                  // 設定Z=1
        }
        ra = 12;                                             // 指令執行完後 輸出R12
        break;                                                                                        
      } case OP_MOV: R[ra] = R[rb]; break;                                              
      case OP_ADD: R[ra] = R[rb] + R[rc]; break;                                       
      case OP_SUB: R[ra] = R[rb] - R[rc]; break;                                        
      case OP_MUL: R[ra] = R[rb] * R[rc]; break;                                          
      case OP_DIV: R[ra] = R[rb] / R[rc]; break;                                         
      case OP_AND: R[ra] = R[rb] & R[rc]; break;                                          
      case OP_OR:  R[ra] = R[rb] | R[rc]; break;                                          
      case OP_XOR: R[ra] = R[rb] ^ R[rc]; break;                                          
      case OP_ROL: R[ra] = ROL(R[rb],c5); break;                                         
      case OP_ROR: R[ra] = ROR(R[rb],c5); break;                                        
      case OP_SHL: R[ra] = SHL(R[rb],c5); break;                                      
      case OP_SHR: R[ra] = SHR(R[rb],c5); break;                                         
      case OP_JEQ: if (Z==1) PC += c24; break;               // equal 
      case OP_JNE: if (Z==0) PC += c24; break;               // not equal
      case OP_JLT: if (N==1&&Z==0) PC += c24; break;         // <
      case OP_JGT: if (N==0&&Z==0) PC += c24; break;         // >
      case OP_JLE:                                           // <=
           if ((N==1&&Z==0)||(N==0&&Z==1)) PC+=c24; break;
      case OP_JGE:                                           // >=
           if ((N==0&&Z==0)||(N==0&&Z==1)) PC+=c24; break;
      case OP_JMP: PC+=c24; break;                           // 無條件                          
      case OP_SWI: LR = PC; PC=c24; break;                   // SW interrupt                           
      case OP_JSUB:LR = PC; PC+=c24; break;                                               
      case OP_RET: if (LR<0) stop=TRUE; else PC=LR; break;   // 當LR=-1,跳出,else return                            
      case OP_PUSH:SP-=4; StoreInt32(R[ra], m, SP); break;                              
      case OP_POP: LoadInt32(R[ra], m, SP); SP+=4; break;                                 
      case OP_PUSHB:SP--; StoreByte(R[ra], m, SP); break;                                
      case OP_POPB:LoadByte(R[ra], m, SP); SP++; break;                                 
      default: printf("Error:invalid op (%02x) ", op);                                                
    }                                                                                                 
    sprintf(buffer, "PC=%08x IR=%08x SW=%08x R[%02d]=0x%08x=%d\n", //print PC IR SW R[ra]的hexo和整數
                         PC,      IR,     SW,  ra,    R[ra],R[ra]);
    strToUpper(buffer);
    printf(buffer);                                                //print sting buffer
  }                                                                                                   
}                                                                                       
                                                                                        
void Cpu0Dump(Cpu0 *cpu0) {                                  // print all registers (dump傾倒)                        
  printf("\n===CPU0 dump registers===\n");                                                            
  printf("IR =0x%08x=%d\n", cpu0->IR, cpu0->IR);             // print IR                                                                         
  int i;                                                                   
  for (i=0; i<16; i++)                                       // print R0~R15 
    printf("R[%02d]=0x%08x=%d\n",i,cpu0->R[i],cpu0->R[i]);
}
