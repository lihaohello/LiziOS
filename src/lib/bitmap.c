#include "../include/bitmap.h"
#include "../include/string.h"

/// @brief 初始化位图
/// @param bitmap
void bitmap_init(struct bitmap* bitmap) {
    memset(bitmap->bits, 0, bitmap->length);
}

/// @brief 获取位图中的某一位
/// @param bitmap
/// @param index
/// @return
bool bitmap_get_bit(struct bitmap* bitmap, u32 bit_index) {
    u32 index0 = bit_index / 8;
    u32 index1 = bit_index % 8;
    return (bitmap->bits[index0] & (1 << index1));
}

/// @brief 设置位图中的某一位
/// @param bitmap
/// @param index
/// @param value: 为0或1
void bitmap_set_bit(struct bitmap* bitmap, u32 bit_index, i8 value) {
    u32 index0 = bit_index / 8;
    u32 index1 = bit_index % 8;

    if (value)
        bitmap->bits[index0] |= (1 << index1);
    else
        bitmap->bits[index0] &= ~(1 << index1);
}

/// @brief 在位图中申请连续的一定数量的位
/// @param bitmap
/// @param count
/// @return
int bitmap_request_bits(struct bitmap* bitmap, u32 bit_count) {
    u32 left = 0;
    u32 right = 0;
    u32 n = 0;
    while (right < bitmap->length && n < bit_count) {
        if (bitmap_get_bit(bitmap, left) == 1) {
            left++;
            right = left;
            continue;
        }
        if (bitmap_get_bit(bitmap, ++right) == 0)
            n = right - left + 1;
        else
            left = right;
    }

    if (right == bitmap->length)
        return -1;
    return left;
}
