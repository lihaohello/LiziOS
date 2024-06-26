#ifndef __IO_H
#define __IO_H
#include "types.h"

/// @brief 向一个16位端口写入一个8位数据
/// @param port 
/// @param data 
static inline void outb(u16 port, u8 data) {
    asm volatile("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

/// @brief 从一个16位端口读一个8位数据
/// @param port
/// @return 
static inline u8 inb(u16 port) {
    u8 data;
    asm volatile("inb %w1, %b0" : "=a"(data) : "Nd"(port));
    return data;
}

#endif