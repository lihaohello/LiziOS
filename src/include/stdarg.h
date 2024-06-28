#ifndef __STDARG_H
#define __STDARG_H

// 定义可变参数的类型
typedef char* va_list;

// 定义可变参数的处理逻辑
#define stack_size(t) (sizeof(t) <= sizeof(char*) ? sizeof(char*) : sizeof(t))
#define va_start(ap, v) (ap = (va_list) & v + sizeof(char*))
#define va_arg(ap, t) (*(t*)((ap += stack_size(t)) - stack_size(t)))
#define va_end(ap) (ap = (va_list)0)

#endif