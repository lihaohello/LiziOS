#ifndef __CONSOLE_H
#define __CONSOLE_H
#include "types.h"

void console_init();
void console_print_str(char* str);
void console_print_num(u32 num);
void console_print_char(char c);

#endif