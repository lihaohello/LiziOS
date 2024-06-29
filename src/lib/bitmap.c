#include "bitmap.h"
#include "string.h"

/// @brief 初始化位图
/// @param bitmap
void bitmap_init(struct bitmap *bitmap)
{
    memset(bitmap->bits, 0, bitmap->length);
}

/// @brief 获取位图中的某一位
/// @param bitmap
/// @param index
/// @return
bool bitmap_get_bit(struct bitmap *bitmap, u32 index)
{
    u32 byte_index = index / 8;
    u32 bit_index = index % 8;
    return (bitmap->bits[byte_index] & (1 << bit_index));
}

/// @brief 设置位图中的某一位
/// @param bitmap
/// @param index
/// @param value: 为0或1
void bitmap_set_bit(struct bitmap *bitmap, u32 index, i8 value)
{
    u32 byte_index = index / 8;
    u32 bit_index = index % 8;

    if (value)
        bitmap->bits[byte_index] |= (1 << bit_index);
    else
        bitmap->bits[byte_index] &= ~(1 << bit_index);
}

/// @brief 在位图中申请连续的一定数量的位
/// @param bitmap
/// @param count
/// @return
int bitmap_request_bits(struct bitmap *bitmap, u32 count)
{
    u32 left = 0;
    u32 right = 0;
    u32 n = 0;
    while (right < bitmap->length && n < count)
    {
        if (bitmap->bits[left] == 1)
        {
            left++;
            right = left;
            continue;
        }
        if (bitmap->bits[++right] == 0)
            n = right - left + 1;
        else
            left = right;
    }

    if (right == bitmap->length)
        return -1;
    return left;
}
