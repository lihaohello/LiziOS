#include "../include/assert.h"
#include "../include/memory.h"
#include "../include/stdio.h"
#include "../include/timer.h"
#include "../include/thread.h"

void k_thread_a(void *);

int main(void)
{
    printf("\n\nLiziOS is initializing the kernel...\n");

    // 中断初始化
    interrupt_init();
    // 计时器初始化
    // timer_init();
    // 内存管理系统初始化
    memory_init();
    // 主进程初始化
    thread_init();

    thread_start("k_thread_a", 31, k_thread_a, "A ");

    while (1){
        printf("0 ");
        schedule();
    }

    return 0;
}

void k_thread_a(void *arg)
{
    char *para = arg;
    while (1){
        printf(para);
        schedule();
    }
}
