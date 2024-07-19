#include "memory.h"
#include "assert.h"
#include "bitmap.h"
#include "stdio.h"
#include "string.h"
#include "sync.h"

// 中断
#define BOCHS_BREAK asm volatile("xchg %bx,%bx");

// 一页对应的字节数
#define PG_SIZE 4096
// 位图所在位置的基址
#define MEM_BITMAP_BASE 0xc009a000
// 内核空间对应的虚拟地址起始位置
#define K_HEAP_START 0xc0100000

// 获取该虚拟地址在页目录中的索引号
#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22)
// 获取该虚拟地址在页表中的索引号
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12)

/// @brief 物理内存池
struct pool {
    /// @brief 用户进程申请资源时需要加锁控制
    struct lock lock;
    /// @brief 管理内存池的位图（位图起始地址、位图字节大小）
    struct bitmap pool_bitmap;
    /// @brief 物理内存的起始地址
    u32 phy_addr_start;
    /// @brief 内存池字节数量
    u32 pool_size;
};

/// @brief 内核物理空间、用户物理空间的内存池
struct pool kernel_pool, user_pool;

/// @brief 内核虚拟内存管理
struct virtual_addr kernel_vaddr;

/// @brief 内存池初始化：1M为内核代码，1M为页表，其余30M内核、用户均分各占15M
/// @param all_mem：总内存大小32M，在loader代码中读取内存大小后写入0xd00内存处
static void mem_pool_init(u32 all_mem) {
    // 256各页表：1页目录+1+254
    u32 page_table_size = PG_SIZE * 256;

    // 内核1M+页表1M已用
    u32 used_mem = page_table_size + 0x100000;

    // 可用32M-2M=30M
    u32 free_mem = all_mem - used_mem;
    u16 all_free_pages = free_mem / PG_SIZE;
    u16 kernel_free_pages = all_free_pages / 2;
    u16 user_free_pages = all_free_pages - kernel_free_pages;

    // 初始化内核物理空间内存池
    kernel_pool.phy_addr_start = used_mem;
    kernel_pool.pool_size = kernel_free_pages * PG_SIZE;
    kernel_pool.pool_bitmap.length = kernel_free_pages / 8;
    kernel_pool.pool_bitmap.bits = (void*)MEM_BITMAP_BASE;
    bitmap_init(&kernel_pool.pool_bitmap);

    // 初始化用户物理空间内存池
    user_pool.phy_addr_start = used_mem + kernel_free_pages * PG_SIZE;
    user_pool.pool_size = user_free_pages * PG_SIZE;
    user_pool.pool_bitmap.length = user_free_pages / 8;
    user_pool.pool_bitmap.bits =
        (void*)(MEM_BITMAP_BASE + kernel_free_pages / 8);
    bitmap_init(&user_pool.pool_bitmap);

    printf("-----------------------------\n");
    printf("kernel_pool: bitmap_start: %x, phy_addr_start=%x\n",
           (u32)kernel_pool.pool_bitmap.bits, kernel_pool.phy_addr_start);
    printf("user_pool: bitmap_start: %x, phy_addr_start=%x\n",
           (u32)user_pool.pool_bitmap.bits, user_pool.phy_addr_start);
    printf("-----------------------------\n");

    // 初始化内核物理空间对应的虚拟地址空间
    kernel_vaddr.addr_bitmap.length = kernel_free_pages / 8;
    kernel_vaddr.addr_bitmap.bits =
        (void*)(MEM_BITMAP_BASE + kernel_free_pages / 8 + user_free_pages / 8);
    kernel_vaddr.addr_start = K_HEAP_START;
    bitmap_init(&kernel_vaddr.addr_bitmap);
}

/// @brief 虚拟地址在页表中的地址
/// @param vaddr 虚拟地址
/// @return
static u32* pte_ptr(u32 vaddr) {
    // 0xffc00000是第1个页表的起始地址
    u32* pte =
        (u32*)(0xffc00000 + ((vaddr & 0xffc00000) >> 10) + PTE_IDX(vaddr) * 4);
    return pte;
}

/// @brief 虚拟地址所属页表在页目录表中的地址
/// @param vaddr 虚拟地址
/// @return
static u32* pde_ptr(u32 vaddr) {
    // 0xfffff0000是页目录的虚拟地址
    u32* pde = (u32*)(0xfffff000 + PDE_IDX(vaddr) * 4);
    return pde;
}

/// @brief 申请虚拟地址（内核虚拟地址、用户虚拟地址）
/// @param
/// @param count
/// @return 虚拟地址
static void* vaddr_get_pages(enum pool_type type, u32 count) {
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
        // 在用户空间分配虚拟内存
        struct task_struct* cur = running_thread();
        bit_index_start =
            bitmap_request_bits(&cur->userprog_vaddr.addr_bitmap, count);
        if (bit_index_start == -1)
            return NULL;

        for (u32 i = bit_index_start; i < bit_index_start + count; i++)
            bitmap_set_bit(&cur->userprog_vaddr.addr_bitmap, i, 1);
        vaddr_start =
            cur->userprog_vaddr.addr_start + bit_index_start * PG_SIZE;
        ASSERT((u32)vaddr_start < (0xc0000000 - PG_SIZE));
    }
    return (void*)vaddr_start;
}

/// @brief 从指定物理内存池中分配一个物理页，返回其起始地址
/// @param m_pool
/// @return
static void* paddr_get_page(struct pool* m_pool) {
    int bit_idx = bitmap_request_bits(&m_pool->pool_bitmap, 1);
    if (bit_idx == -1)
        return NULL;
    bitmap_set_bit(&m_pool->pool_bitmap, bit_idx, 1);
    u32 page_pyaddr = (m_pool->phy_addr_start + PG_SIZE * bit_idx);
    return (void*)page_pyaddr;
}

/// @brief 在页表中添加虚拟地址页和物理地址页的映射
/// @param _vaddr 虚拟地址
/// @param _page_phyaddr 物理页地址
static void bind_vaddr_paddr_page(void* _vaddr, void* _page_phyaddr) {
    u32 vaddr = (u32)_vaddr;
    u32 page_phyaddr = (u32)_page_phyaddr;

    u32* pde = pde_ptr(vaddr);
    u32* pte = pte_ptr(vaddr);

    // 页目录项已存在，先判断是否存在页表项
    if (*pde & 0x00000001) {
        ASSERT(!(*pte & 0x00000001));
        // 不存在相应页表，创建
        if (!(*pte & 0x00000001))
            *pte = (page_phyaddr | 0b0111);
        // 已存在相应页表，错误
        else
            PANIC("pte repeated!");
    } else {
        // 页目录项不存在，先创建页目录项
        u32 pde_phyaddr = (u32)paddr_get_page(&kernel_pool);
        *pde = (pde_phyaddr | 0b0111);
        memset((void*)((u32)pte & 0xfffff000), 0, PG_SIZE);
        ASSERT(!(*pte & 0x00000001));
        *pte = (page_phyaddr | 0b0111);
    }
}

//-----------------------------------------------------------------
// 对外接口

/// @brief 分配连续物理页，设置页表并返回其虚拟地址空间
/// @param
/// @param count
/// @return
static void* malloc_pages(enum pool_type type, u32 count) {
    ASSERT(count > 0 && count < 3840);

    // 先分配虚拟地址
    void* vaddr_start = vaddr_get_pages(type, count);
    if (vaddr_start == NULL)
        return NULL;
    u32 vaddr = (u32)vaddr_start;

    // 再分配物理地址（一页页分配，分配每一页后就建立页表项）
    struct pool* mem_pool = type & KERNEL_POOL ? &kernel_pool : &user_pool;
    u32 cnt = count;
    while (cnt-- > 0) {
        void* page_phyaddr = paddr_get_page(mem_pool);
        if (page_phyaddr == NULL)
            return NULL;
        // 建立页表项
        bind_vaddr_paddr_page((void*)vaddr, page_phyaddr);
        vaddr += PG_SIZE;
    }
    memset(vaddr_start, 0, count * PG_SIZE);

    return vaddr_start;
}

/// @brief 申请内核空间地址，返回其虚拟地址
/// @param count
/// @return
void* malloc_kernel_pages(u32 count) {
    lock_acquire(&kernel_pool.lock);
    void* vaddr = malloc_pages(KERNEL_POOL, count);
    lock_release(&kernel_pool.lock);
    return vaddr;
}

/// @brief 申请用户空间地址，返回其虚拟地址
/// @param count
/// @return
void* malloc_user_pages(u32 count) {
    lock_acquire(&user_pool.lock);
    void* vaddr = malloc_pages(USER_POOL, count);
    lock_release(&user_pool.lock);
    return vaddr;
}

/// @brief 将虚拟地址与一个物理页关联，物理页现场分配
/// @param  
/// @param vaddr 
/// @return 
void* get_a_page(enum pool_type pf, u32 vaddr) {
    struct pool* mem_pool = pf & KERNEL_POOL ? &kernel_pool : &user_pool;
    lock_acquire(&mem_pool->lock);

    struct task_struct* cur = running_thread();
    i32 bit_idx = -1;

    if (cur->pgdir != NULL && pf == USER_POOL) {
        bit_idx = (vaddr - cur->userprog_vaddr.addr_start) / PG_SIZE;
        ASSERT(bit_idx > 0);
        bitmap_set_bit(&cur->userprog_vaddr.addr_bitmap, bit_idx, 1);
    } else if (cur->pgdir == NULL && pf == KERNEL_POOL) {
        bit_idx = (vaddr - kernel_vaddr.addr_start) / PG_SIZE;
        ASSERT(bit_idx > 0);
        bitmap_set_bit(&kernel_vaddr.addr_bitmap, bit_idx, 1);
    } else
        PANIC(
            "get_a_page:not allow kernel alloc userspace or user alloc "
            "kernelspace by get_a_page");

    void* page_phyaddr = paddr_get_page(mem_pool);
    if (page_phyaddr == NULL)
        return NULL;

    bind_vaddr_paddr_page((void*)vaddr, page_phyaddr);
    lock_release(&mem_pool->lock);
    return (void*)vaddr;
}

/// @brief 虚拟地址转物理地址
/// @param vaddr 
/// @return 
u32 vaddr_to_paddr(u32 vaddr) {
    u32* pte = pte_ptr(vaddr);
    return ((*pte & 0xfffff000) + (vaddr & 0x00000fff));
}

/// @brief 内存初始化的主函数
void memory_init() {
    // 先前将内存总容量信息存放在0xd00处，读取出来进行空间分配
    u32 mem_bytes_total = (*(u32*)(0xd00));
    mem_pool_init(mem_bytes_total);

    lock_init(&user_pool.lock);
    lock_init(&kernel_pool.lock);

    printf("memory_init done\n");
}

//-----------------------------------------------------------------