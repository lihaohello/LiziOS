#ifndef __MEMORY_H
#define __MEMORY_H
#include "bitmap.h"
#include "types.h"

/// @brief 内存池种类
enum pool_type {
    /// @brief 内核内存池
    KERNEL_POOL = 1,
    /// @brief 用户内存池
    USER_POOL = 2
};

/// @brief 虚拟地址空间管理
struct virtual_addr {
    struct bitmap addr_bitmap;
    u32 addr_start;
};

extern struct pool kernel_pool, user_pool;

void* malloc_kernel_pages(u32);
void* malloc_user_pages(u32);
void* get_a_page(enum pool_type, u32);
u32 vaddr_to_paddr(u32);
void memory_init();

#endif