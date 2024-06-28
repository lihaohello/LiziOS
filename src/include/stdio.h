#ifndef __STDIO_H
#define __STDIO_H

#include <../include/stdarg.h>

// 汇编实现的基本打印函数（print.asm）,print_str借助于print_char
void print_char(char char_asci);
void print_str(char* str);

// C语言实现的格式化打印函数（printf.c & vsprintf.c），借助于print_str
int vsprintf(char* buf, const char* fmt, va_list args);
int sprintf(char* buf, const char* fmt, ...);
int printf(const char* fmt, ...);

#endif