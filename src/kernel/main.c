#include "../include/assert.h"
#include "../include/interrupt.h"
#include "../include/memory.h"
#include "../include/stdio.h"
#include "../include/timer.h"



int main(void) {
    printf("\n\nLiziOS is initializing the kernel...\n");

    // 中断初始化
    interrupt_init();
    // 计时器初始化
    timer_init();
    // 内存管理系统初始化
    mem_init();

    // 分配虚拟地址
    void* addr = get_kernel_pages(3);
    printf("addr is: %x", (u32)addr);

    ASSERT(1 == 2);

    while (1) {
    };

    return 0;
}
