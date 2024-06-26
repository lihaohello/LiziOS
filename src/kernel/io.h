#ifndef __IO_H
#define __IO_H
#include "types.h"

static inline void outb(u16 port, u8 data) {
    asm volatile("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

static inline void outsw(u16 port, const void* addr, u32 word_cnt) {
    asm volatile("cld; rep outsw" : "+S"(addr), "+c"(word_cnt) : "d"(port));
}

static inline u8 inb(u16 port) {
    u8 data;
    asm volatile("inb %w1, %b0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void insw(u16 port, void* addr, u32 word_cnt) {
    asm volatile("cld; rep insw"
                 : "+D"(addr), "+c"(word_cnt)
                 : "d"(port)
                 : "memory");
}
#endif
