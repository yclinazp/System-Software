#include "Cpu0.h"

void runObjFile(char *objFile) {                 // ���������D���
  printf("===VM0:run %s on CPU0===\n", objFile);               
  Cpu0 *cpu0 = Cpu0New(objFile);                 // �إ�CPU0����
  Cpu0Run(cpu0, 0);                              // �}�l����
  Cpu0Dump(cpu0);                                // �ɦL�Ȧs��
  Cpu0Free(cpu0);                                // ����O���� 
}

Cpu0* Cpu0New(char *objFile) {                   // �إ� CPU0 ����                 
  Cpu0 *cpu0=ObjNew(Cpu0, 1);                    //  ���t CPU0 ����Ŷ� 
  cpu0->m = newFileBytes(objFile, &cpu0->mSize); //  ���J�M���� objFile ��O���� m ��
  return cpu0;                                                                
}                                                                             
                                                                              
void Cpu0Free(Cpu0 *cpu0) {                      // �R�� CPU0 ����                 
  freeMemory(cpu0->m);                           //  ���� CPU0 �� memory 
  ObjFree(cpu0);                                 //  ���� CPU0 ����                                         
}                                                                                         
                                                                                                                                                            
#define bits(i, from, to) ((UINT32) i << (31-to) >> (31-to+from)) // ���o from �� to �������줸                   
#define ROR(i, k) (((UINT32)i>>k)|(bits(i,32-k, 31)<<(32-k)))// �V�k����k�줸                                           
#define ROL(i, k) (((UINT32)i<<k)|(bits(i,0,k-1)<<(32-k)))   // �V������k�줸                                             
#define SHR(i, k) ((UINT32)i>>k)                             // �V�k����k�줸                
#define SHL(i, k) ((UINT32)i<<k)                             // �V������k�줸                
#define bytesToInt32(p) (INT32)(p[0]<<24|p[1]<<16|p[2]<<8|p[3])// 4 byte�� int
#define bytesToInt16(p) (INT16)(p[0]<<8|p[1])                // 2 byte�� INT16               
#define int32ToBytes(i, bp) { bp[0]=i>>24; bp[1]=i>>16; bp[2]=i>>8; bp[3]=i;} // int�ର4 byte                                             
#define StoreInt32(i, m, addr) { BYTE *p=&m[addr]; int32ToBytes(i, p); } // i=m[addr�Kaddr+3]                                         
#define LoadInt32(i, m, addr) { BYTE *p=&m[addr]; i=bytesToInt32(p); } // m[addr..addr+3]=i                                         
#define StoreByte(b, m, addr) { m[addr] = (BYTE) b; }        // m[addr]=b                                                 
#define LoadByte(b, m, addr) { b = m[addr]; }                // b=m[addr]                    
                                                                                          
#define PC R[15]                                             // PC is R[15]                  
#define LR R[14]                                             // LR is R[14]                  
#define SP R[13]                                             // SP is R[13]                  
#define SW R[12]                                             // SW is R[12]                  

void Cpu0Run(Cpu0 *cpu0, int start) {                        // �����������D�n������                   
  char buffer[200];
  unsigned int IR, op, ra, rb, rc, cc;                                                                         
  int c5, c12, c16, c24, caddr, raddr;                                                                
  unsigned int N, Z;
  BYTE *m=cpu0->m;                                                                                    
  int  *R=cpu0->R;                                           
  PC = start;                                                // �]�w�_�l��}�A�ǳƶ}�l����                             
  LR = -1;                                                                                
  BOOL stop = FALSE;                                                                                  
  while (!stop) {                                            // �p�G�|������                             
    R[0] = 0;                                                // R[0] �û��� 0                            
    LoadInt32(IR, m, PC);                                    // ���O�^���AIR=[PC..PC+3]                                 
    cpu0->IR = IR;                                           // ���o���O�Ȧs��                           
    PC += 4;                                                 // �^�����N PC �[ 4�A���V�U�@�ӫ��O
    op = bits(IR, 24, 31);                                   // ���o op ���AIR[24..31]
    ra = bits(IR, 20, 23);                                   // ���o ra ���AIR[20..23]    
    rb = bits(IR, 16, 19);                                   // ���o rb ���AIR[16..19]
    rc = bits(IR, 12, 15);                                   // ���o rc ���AIR[12..15]
    c5 = bits(IR,  0, 4);                                    // ���o 5  �줸�� cx 
    c12= bits(IR, 0, 11);                                    // ���o 12 �줸�� cx 
    c16= bits(IR, 0, 15);                                    // ���o 16 �줸�� cx                         
    c24= bits(IR, 0, 23);                                    // ���o 24 �줸�� cx                                         
    N  = bits(SW, 31, 31);
    Z  = bits(SW, 30, 30);
    if (bits(IR, 11, 11)!=0) c12 |= 0xFFFFF000;              // �Y cx ���t�ơA�h�վ㬰2�ɼƮ榡                                         
    if (bits(IR, 15, 15)!=0) c16 |= 0xFFFF0000;              // �Y cx ���t�ơA�h�վ㬰2�ɼƮ榡                                        
    if (bits(IR, 23, 23)!=0) c24 |= 0xFF000000;              // �Y cx ���t�ơA�h�վ㬰2�ɼƮ榡                                                                                 
    caddr = R[rb]+c16;                                       // ���o��}[Rb+cx]                          
    raddr = R[rb]+R[rc];                                     // ���o��}[Rb+Rc]                          
    switch (op) {                                            // �ھ�op����ʧ@                           
      case OP_LD : LoadInt32(R[ra], m, caddr); break;        // �B�z LD ���O                            
      case OP_ST : StoreInt32(R[ra], m, caddr); break;       // �B�z ST ���O                            
      case OP_LDB: LoadByte(R[ra], m, caddr); break;         // �B�z LDB ���O                            
      case OP_STB: StoreByte(R[ra], m, caddr); break;        // �B�z STB ���O                            
      case OP_LDR: LoadInt32(R[ra], m, raddr); break;        // �B�z LDR ���O                           
      case OP_STR: StoreInt32(R[ra], m, raddr); break;       // �B�z STR ���O                           
      case OP_LBR: LoadByte(R[ra], m, raddr); break;         // �B�z LBR ���O                           
      case OP_SBR: StoreByte(R[ra], m, raddr); break;        // �B�z SBR ���O                           
      case OP_LDI: R[ra] = c16; break;                       // �B�z LDI ���O                           
      case OP_CMP: {                                         // �B�zCMP���O�A�ھڤ�����G�A�]�w N,Z �X�� 
        if (R[ra] > R[rb]) {                                 // > : SW(N=0, Z=0)
          SW &= 0x3FFFFFFF;                                  // N=0, Z=0
        } else if (R[ra] < R[rb]) {                          // < : SW(N=1, Z=0, ....)                                                
          SW |= 0x80000000;                                  // N=1;
          SW &= 0xBFFFFFFF;                                  // Z=0;
        } else {                                             // = : SW(N=0, Z=1)                      
          SW &= 0x7FFFFFFF;                                  // N=0;
          SW |= 0x40000000;                                  // Z=1;
        }
        ra = 12;
        break;                                                                                        
      } case OP_MOV: R[ra] = R[rb]; break;                   // �B�zMOV���O                             
      case OP_ADD: R[ra] = R[rb] + R[rc]; break;             // �B�zADD���O                             
      case OP_SUB: R[ra] = R[rb] - R[rc]; break;             // �B�zSUB���O                             
      case OP_MUL: R[ra] = R[rb] * R[rc]; break;             // �B�zMUL���O                             
      case OP_DIV: R[ra] = R[rb] / R[rc]; break;             // �B�zDIV���O                             
      case OP_AND: R[ra] = R[rb] & R[rc]; break;             // �B�zAND���O                             
      case OP_OR:  R[ra] = R[rb] | R[rc]; break;             // �B�zOR���O                              
      case OP_XOR: R[ra] = R[rb] ^ R[rc]; break;             // �B�zXOR���O                             
      case OP_ROL: R[ra] = ROL(R[rb],c5); break;             // �B�zROL���O                             
      case OP_ROR: R[ra] = ROR(R[rb],c5); break;             // �B�zROR���O                             
      case OP_SHL: R[ra] = SHL(R[rb],c5); break;             // �B�zSHL���O                             
      case OP_SHR: R[ra] = SHR(R[rb],c5); break;             // �B�zSHR���O                             
      case OP_JEQ: if (Z==1) PC += c24; break;               // �B�zJEQ���O Z=1
      case OP_JNE: if (Z==0) PC += c24; break;               // �B�zJNE���O Z=0 
      case OP_JLT: if (N==1&&Z==0) PC += c24; break;         // �B�zJLT���O NZ=10 
      case OP_JGT: if (N==0&&Z==0) PC += c24; break;         // �B�zJGT���O NZ=00
      case OP_JLE:                                           // �B�zJLE���O NZ=10 or 01
           if ((N==1&&Z==0)||(N==0&&Z==1)) PC+=c24; break;
      case OP_JGE:                                           // �B�zJGE���O NZ=00 or 01
           if ((N==0&&Z==0)||(N==0&&Z==1)) PC+=c24; break;
      case OP_JMP: PC+=c24; break;                           // �B�zJMP���O                             
      case OP_SWI: LR = PC; PC=c24; break;                   // �B�zSWI���O                             
      case OP_JSUB:LR = PC; PC+=c24; break;                  // �B�zJSUB���O                            
      case OP_RET: if (LR<0) stop=TRUE; else PC=LR; break;   // �B�zRET���O                             
      case OP_PUSH:SP-=4; StoreInt32(R[ra], m, SP); break;   // �B�zPUSH���O                            
      case OP_POP: LoadInt32(R[ra], m, SP); SP+=4; break;    // �B�zPOP���O                             
      case OP_PUSHB:SP--; StoreByte(R[ra], m, SP); break;    // �B�zPUSH���O                            
      case OP_POPB:LoadByte(R[ra], m, SP); SP++; break;      // �B�zPOPB���O                            
      default: printf("Error:invalid op (%02x) ", op);                                                
    }                                                                                                 
    sprintf(buffer, "PC=%08x IR=%08x SW=%08x R[%02d]=0x%08x=%d\n", // �L�X PC, IR, R[ra]�Ȧs�����ȡA�H�Q�[��
                         PC,      IR,     SW,  ra,    R[ra],R[ra]);
    strToUpper(buffer);
    printf(buffer);
  }                                                                                                   
}                                                                                       
                                                                                        
void Cpu0Dump(Cpu0 *cpu0) {                                  // �L�X�Ҧ��Ȧs��                           
  printf("\n===CPU0 dump registers===\n");                                                            
  printf("IR =0x%08x=%d\n", cpu0->IR, cpu0->IR);             // �L�X IR                                                                           
  int i;                                                                   
  for (i=0; i<16; i++)                                       // �L�X R0..R15  
    printf("R[%02d]=0x%08x=%d\n",i,cpu0->R[i],cpu0->R[i]);
}
