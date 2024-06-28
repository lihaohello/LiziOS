#ifndef __TYPES_H
#define __TYPES_H

// 定义基本数据类型（有符号、无符号）
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

// 定义数组长度类型
typedef unsigned int size_t;

// 定义逻辑真、逻辑假常量
#define true 1
#define false 0

// 定义字符串结束符
#define EOS '\0'

// 定义空指针
#define NULL ((void*)0)

#endif