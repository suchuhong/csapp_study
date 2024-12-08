## 使用

make main

make run

## 获取 add.txt

1、gcc add.c -o add

2、objdump add >> add.txt

3、删除部分信息


## 注意
1、 使用 extern 修饰全局变量，防止重复定义。

2、仅在一个 .c 文件中定义全局变量。

3、确保头文件有 include guard 或 #pragma once。