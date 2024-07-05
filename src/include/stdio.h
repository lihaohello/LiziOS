#ifndef __STDIO_H
#define __STDIO_H

#include <stdarg.h>
#include "types.h"

// prints.asm: 汇编实现的字符串打印函数（
void print_str(char* str);

// vsprintf.c: 借用Linux代码，格式化字符串到缓存
int vsprintf(char* buf, const char* fmt, va_list args);
int sprintf(char* buf, const char* fmt, ...);

// printf.c: 格式化打印函数，借助vsprintf()、print_str()
int printf(const char* fmt, ...);

/// @brief 设置光标位置
/// @param cursor_pos
void set_cursor(u32 cursor_pos);

#endif