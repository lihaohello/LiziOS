#include "../include/bitmap.h"
#include "../include/assert.h"
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

/*
/// @brief 在位图中申请连续的一定数量的位
/// @param bitmap
/// @param count
/// @return
int bitmap_request_bits(struct bitmap* bitmap, u32 bit_count) {
    // 本算法有待改进和测试
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
*/

int bitmap_request_bits(struct bitmap* bitmap, u32 cnt) {
    u32 idx_byte = 0;
    while ((0xff == bitmap->bits[idx_byte]) && (idx_byte < bitmap->length)) {
        idx_byte++;
    }

    ASSERT(idx_byte < bitmap->length);
    if (idx_byte == bitmap->length) {
        return -1;
    }

    int idx_bit = 0;
    while ((uint8_t)(1 << idx_bit) & bitmap->bits[idx_byte]) {
        idx_bit++;
    }

    int bit_idx_start = idx_byte * 8 + idx_bit;
    if (cnt == 1) {
        return bit_idx_start;
    }

    u32 bit_left = (bitmap->length * 8 - bit_idx_start);
    u32 next_bit = bit_idx_start + 1;
    u32 count = 1;

    bit_idx_start = -1;
    while (bit_left-- > 0) {
        if (!(bitmap_get_bit(bitmap, next_bit))) {
            count++;
        } else {
            count = 0;
        }
        if (count == cnt) {
            bit_idx_start = next_bit - cnt + 1;
            break;
        }
        next_bit++;
    }
    return bit_idx_start;
}
