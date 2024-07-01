#ifndef __BITMAP_H
#define __BITMAP_H

#include <stdint.h>
#include "types.h"

/// @brief 位图数据结构
struct bitmap {
    /// @brief 总长度（单位：字节）
    u32 length;
    /// @brief 起始字节指针
    u8* bits;
};

void bitmap_init(struct bitmap*);
bool bitmap_get_bit(struct bitmap*, u32);
void bitmap_set_bit(struct bitmap*, u32, i8);
int bitmap_request_bits(struct bitmap*, u32);

#endif