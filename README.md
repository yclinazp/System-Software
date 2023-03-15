# Table of the contents
- [Background](#Background)

+ [Install](#Install)

* [Usage](#Usage)

* [Reference](#Reference)

# Background
參考系統程式 - 陳鍾誠(https://www.flag.com.tw/books/product/F7501)
使用C語言實作一簡單系統軟體C0語言(包含組譯器、虛擬機器、剖析氣、編譯器),C0語言包含for迴圈和基本運算式功能

# Install

1. Clone the repo :
```
git clone https://github.com/yclinazp/System-Software.git
```

# Usage
- **開始建置專案**

0. Type the "make" command.
```
make
```
- **For compiler**

1. 編譯test.c0, 輸出test.asm0組合語言.
```
c0c test.c0 test.asm0
```
- **For assembler**

2. 組譯test.asm0, 輸出test.obj0目的碼
```
as0 test.asm0 test.obj0
```
- **For executing object code**

3. 使用vm0虛擬機器執行目的碼
```
vm0 test.obj0
```
- **清除檔案**

4. 清除上次make產生的檔案.
```
make clean
```

# Reference

1.[系統程式 - 陳鍾誠 電子書](https://github.com/cccbook/sp/wiki/spbook?fbclid=IwAR27KGcfHevXaAFR2uoxb8QY3nzTwHnC_WAwdiVFmAdDX0tjSswElonU_7U)

2.[系統程式 - 陳鍾誠 code](http://sp1.wikidot.com/main)

