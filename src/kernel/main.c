#include "assert.h"
#include "memory.h"
#include "stdio.h"
#include "timer.h"
#include "thread.h"
#include "interrupt.h"

void k_thread(void *);

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

    // 创建内核线程
    thread_start("k_thread_a", 4, k_thread, "A ");
    thread_start("k_thread_b", 4, k_thread, "B ");
    thread_start("k_thread_c", 4, k_thread, "C ");
    thread_start("k_thread_d", 4, k_thread, "D ");

    // 开启中断
    intr_enable();

    while (1){
        asm volatile("cli");
        printf("0 ");
        asm volatile("sti");
    }

    return 0;
}

/// @brief 内核线程指定的函数
/// @param arg 
void k_thread(void *arg)
{
    char *para = arg;
    while (1)
    {
        asm volatile("cli");
        printf(para);
        asm volatile("sti");
    }
}
