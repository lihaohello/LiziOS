#include "assert.h"
#include "memory.h"
#include "stdio.h"
#include "timer.h"
#include "thread.h"
#include "interrupt.h"

void k_thread_a(void *);

int main(void)
{
    printf("\n\nLiziOS is initializing the kernel...\n");

    // 中断初始化
    interrupt_init();
    // 计时器初始化
    timer_init();
    // 内存管理系统初始化
    memory_init();
    // 主进程初始化
    thread_init();

    thread_start("k_thread_a", 8, k_thread_a, "A ");
    thread_start("k_thread_b", 4, k_thread_a, "B ");
    // thread_start("k_thread_c", 31, k_thread_a, "C ");
    // thread_start("k_thread_d", 31, k_thread_a, "D ");

    intr_enable();

    while (1)
    {
        printf("0 ");
    }

    return 0;
}

void k_thread_a(void *arg)
{
    char *para = arg;
    while (1)
    {
        printf(para);
    }
}
