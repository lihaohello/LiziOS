#ifndef __BITMAP_H
#define __BITMAP_H

#include "types.h"
#include <stdint.h>

struct bitmap
{
    u32 length;
    u8 *bits;
};

void bitmap_init(struct bitmap *bitmap);
bool bitmap_scan_test(struct bitmap *bitmap, u32 index);
int bitmap_scan(struct bitmap *bitmap, u32 count);
void bitmap_set(struct bitmap *bitmap, u32 index, i8 value);

#endif
