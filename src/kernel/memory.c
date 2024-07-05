#include "../include/memory.h"
#include "../include/assert.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/types.h"

#define BOCHS_BREAK asm volatile("xchg %bx,%bx");

#define PG_SIZE 4096
#define MEM_BITMAP_BASE 0xc009a000
#define K_HEAP_START 0xc0100000

#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22)
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12)

/// @brief 物理内存池
struct pool {
    /// @brief 管理内存池的位图（位图起始地址、位图字节大小）
    struct bitmap pool_bitmap;
    /// @brief 物理内存的起始地址
    u32 phy_addr_start;
    /// @brief 内存池字节数量
    u32 pool_size;
};

/// @brief 声明内核物理内存池、用户物理内存池
struct pool kernel_pool, user_pool;

/// @brief 虚拟内存管理
struct virtual_addr kernel_vaddr;

/// @brief 内存池初始化：1M为内核代码，1M为页表，其余30M内核、用户平分各占15M
/// @param all_mem：总内存大小32M，在loader代码中读取内存大小后写入0xd00内存处
static void mem_pool_init(u32 all_mem) {
    printf("   mem_pool_init start\n");
    // 256各页表：1页目录+1+254
    u32 page_table_size = PG_SIZE * 256;

    // 内核1M+页表1M已用
    u32 used_mem = page_table_size + 0x100000;
    // 可用32M-2M=30M
    u32 free_mem = all_mem - used_mem;
    u16 all_free_pages = free_mem / PG_SIZE;
    u16 kernel_free_pages = all_free_pages / 2;
    u16 user_free_pages = all_free_pages - kernel_free_pages;

    u32 kbm_length = kernel_free_pages / 8;
    u32 ubm_length = user_free_pages / 8;

    u32 kp_start = used_mem;
    u32 up_start = kp_start + kernel_free_pages * PG_SIZE;

    kernel_pool.phy_addr_start = kp_start;
    user_pool.phy_addr_start = up_start;
    kernel_pool.pool_size = kernel_free_pages * PG_SIZE;
    user_pool.pool_size = user_free_pages * PG_SIZE;
    kernel_pool.pool_bitmap.length = kbm_length;
    user_pool.pool_bitmap.length = ubm_length;
    kernel_pool.pool_bitmap.bits = (void*)MEM_BITMAP_BASE;
    user_pool.pool_bitmap.bits = (void*)(MEM_BITMAP_BASE + kbm_length);

    printf("kernel_pool: bitmap_start: %x, phy_addr_start=%x\n",
           (u32)kernel_pool.pool_bitmap.bits, kernel_pool.phy_addr_start);
    printf("user_pool: bitmap_start: %x, phy_addr_start=%x\n",
           (u32)user_pool.pool_bitmap.bits, user_pool.phy_addr_start);

    // 清空两个位图
    bitmap_init(&kernel_pool.pool_bitmap);
    bitmap_init(&user_pool.pool_bitmap);

    // 内核空间的虚拟地址管理
    kernel_vaddr.addr_bitmap.length = kbm_length;
    kernel_vaddr.addr_bitmap.bits =
        (void*)(MEM_BITMAP_BASE + kbm_length + ubm_length);
    kernel_vaddr.addr_start = K_HEAP_START;
    bitmap_init(&kernel_vaddr.addr_bitmap);
    printf("mem_pool_init done\n");
}

/// @brief 在内存池中申请虚拟地址
/// @param
/// @param count
/// @return 虚拟地址
static void* vaddr_get(enum pool_type type, u32 count) {
    u32 vaddr_start = 0, bit_index_start = -1;
    if (type == KERNEL_POOL) {
        // 在内核虚拟位图中分配虚拟内存
        bit_index_start = bitmap_request_bits(&kernel_vaddr.addr_bitmap, count);
        if (bit_index_start == -1)
            return NULL;
        // 分配完成之后，将相应位置为1
        for (u32 i = bit_index_start; i < bit_index_start + count; i++)
            bitmap_set_bit(&kernel_vaddr.addr_bitmap, i, 1);
        vaddr_start = kernel_vaddr.addr_start + bit_index_start * PG_SIZE;
    } else {
        // 在用户空间分配虚拟内存...
    }
    return (void*)vaddr_start;
}

/// @brief 获取虚拟地址的pte指针（虚拟地址）
/// @param vaddr 虚拟地址
/// @return
static u32* pte_ptr(u32 vaddr) {
    // 0xffc00000是第1个页表的起始地址
    u32* pte =
        (u32*)(0xffc00000 + ((vaddr & 0xffc00000) >> 10) + PTE_IDX(vaddr) * 4);
    return pte;
}

/// @brief 获取虚拟地址的pde指针（虚拟地址）
/// @param vaddr 虚拟地址
/// @return
static u32* pde_ptr(u32 vaddr) {
    // 0xfffff0000是页目录的虚拟地址
    u32* pde = (u32*)(0xfffff000 + PDE_IDX(vaddr) * 4);
    return pde;
}

/// @brief 从物理内存池中分配一个物理页
/// @param m_pool
/// @return
static void* palloc(struct pool* m_pool) {
    int bit_idx = bitmap_request_bits(&m_pool->pool_bitmap, 1);
    printf("in palloc: bit_idx=%d\n", bit_idx);
    if (bit_idx == -1)
        return NULL;
    bitmap_set_bit(&m_pool->pool_bitmap, bit_idx, 1);
    u32 page_pyaddr = (m_pool->phy_addr_start + PG_SIZE * bit_idx);
    return (void*)page_pyaddr;
}

/// @brief 在页表中添加虚拟地址和物理地址的映射
/// @param _vaddr 虚拟地址
/// @param _page_phyaddr 物理页地址
static void page_table_add(void* _vaddr, void* _page_phyaddr) {
    u32 vaddr = (u32)_vaddr;
    u32 page_phyaddr = (u32)_page_phyaddr;

    u32* pde = pde_ptr(vaddr);
    u32* pte = pte_ptr(vaddr);

    if (*pde & 0x00000001) {
        ASSERT(!(*pte & 0x00000001));
        if (!(*pte & 0x00000001)) {
            *pte = (page_phyaddr | 0b0111);
        } else {
            PANIC("pte repeated!");
            *pte = (page_phyaddr | 0b0111);
        }
    } else {
        u32 pde_phyaddr = (u32)palloc(&kernel_pool);
        *pde = (pde_phyaddr | 0b0111);
        memset((void*)((u32)pte & 0xfffff000), 0, PG_SIZE);
        ASSERT(!(*pte & 0x00000001));
        *pte = (page_phyaddr | 0b0111);
    }
}

/// @brief 分配n个页，并返回其虚拟地址空间
/// @param
/// @param count
/// @return
void* malloc_page(enum pool_type type, u32 count) {
    ASSERT(count > 0 && count < 3840);

    void* vaddr_start = vaddr_get(type, count);
    if (vaddr_start == NULL)
        return NULL;
    u32 vaddr = (u32)vaddr_start;

    struct pool* mem_pool = type & KERNEL_POOL ? &kernel_pool : &user_pool;
    u32 cnt = count;
    while (cnt-- > 0) {
        void* page_phyaddr = palloc(mem_pool);
        if (page_phyaddr == NULL)
            return NULL;
        page_table_add((void*)vaddr, page_phyaddr);

        vaddr += PG_SIZE;
    }
    return vaddr_start;
}

/// @brief 分配内核物理地址，并返回虚拟地址
/// @param count
/// @return
void* get_kernel_pages(u32 count) {
    void* vaddr = malloc_page(KERNEL_POOL, count);
    if (vaddr != NULL)
        memset(vaddr, 0, count * PG_SIZE);
    return vaddr;
}

void memory_init() {
    printf("mem_init start\n");
    u32 mem_bytes_total = (*(u32*)(0xd00));
    mem_pool_init(mem_bytes_total);
    printf("mem_init done\n");
}
