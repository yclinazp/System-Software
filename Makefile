CC   = gcc
OBJ  = Parser.o Tree.o Lib.o Scanner.o Array.o Compiler.o HashTable.o Generator.o Assembler.o Cpu0.o OpTable.o
#所有object file
LINKOBJ = $(OBJ)
#連結用的目的檔
LIBS = 
#無額外函式庫
INCS =
#無額外連結目錄
BIN  = test.exe c0c.exe as0.exe vm0.exe
#可執行檔列表
CFLAGS = $(INCS) -g3
#顯示編譯訊息
RM = del #移除指令for window

#RM = rm -f #移除指令for linux

.PHONY: all clean
#利用.PHONY 來指定all clean 為 fake 項目, 非真正文件

all: $(OBJ) test c0c as0 vm0
#make會先編譯完所有OBJ, 再創建test、c0c等可執行檔
test: $(OBJ)
	$(CC) main.c $(LINKOBJ) -DTARGET=TEST -o test $(LIBS)
#先編譯完所有OBJ 使用gcc編譯main.c 連接所有目標文件，並生成test可執行文件
#-D define TARGET=TEST
c0c: $(OBJ)
	$(CC) main.c $(LINKOBJ) -DTARGET=C0C -o c0c $(LIBS)

as0: $(OBJ)
	$(CC) main.c $(LINKOBJ) -DTARGET=AS0 -o as0 $(LIBS)

vm0: $(OBJ)
	$(CC) main.c $(LINKOBJ) -DTARGET=VM0 -o vm0 $(LIBS)

clean: 
	${RM} $(OBJ) $(BIN)

Parser.o: Parser.c
	$(CC) -c Parser.c -o Parser.o $(CFLAGS)
#如何編譯Parser.o, depend Parser.c, -c表示只編譯不連結
Tree.o: Tree.c
	$(CC) -c Tree.c -o Tree.o $(CFLAGS)

Lib.o: Lib.c
	$(CC) -c Lib.c -o Lib.o $(CFLAGS)

Scanner.o: Scanner.c
	$(CC) -c Scanner.c -o Scanner.o $(CFLAGS)

Array.o: Array.c
	$(CC) -c Array.c -o Array.o $(CFLAGS)

Compiler.o: Compiler.c
	$(CC) -c Compiler.c -o Compiler.o $(CFLAGS)

HashTable.o: HashTable.c
	$(CC) -c HashTable.c -o HashTable.o $(CFLAGS)

Generator.o: Generator.c
	$(CC) -c Generator.c -o Generator.o $(CFLAGS)

Assembler.o: Assembler.c
	$(CC) -c Assembler.c -o Assembler.o $(CFLAGS)

Cpu0.o: Cpu0.c
	$(CC) -c Cpu0.c -o Cpu0.o $(CFLAGS)

OpTable.o: OpTable.c
	$(CC) -c OpTable.c -o OpTable.o $(CFLAGS)
