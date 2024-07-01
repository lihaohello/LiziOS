#ifndef __MEMORY_H
#define __MEMORY_H
#include "bitmap.h"
#include "types.h"

enum pool_type { KERNEL_POOL, USER_POOL };

struct virtual_addr {
    struct bitmap addr_bitmap;
    u32 addr_start;
};

extern struct pool kernel_pool, user_pool;
void mem_init();

#endif