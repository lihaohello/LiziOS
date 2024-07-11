#include "assert.h"
#include "console.h"
#include "interrupt.h"
#include "ioqueue.h"
#include "keyboard.h"
#include "memory.h"
#include "stdio.h"
#include "thread.h"
#include "timer.h"
#include "tss.h"

void k_thread(void*);

int main(void) {
    printf("\n\nLiziOS is initializing the kernel...\n");

    // 中断初始化
    interrupt_init();
    // 计时器初始化
    timer_init();
    // 内存管理系统初始化
    memory_init();
    // 主进程初始化
    thread_init();
    // 控制台初始化
    console_init();
    // 键盘初始化
    keyboard_init();
    // 初始化任务
    tss_init();

    // thread_start("k_thread_a", 8, k_thread, "A_");
    // thread_start("k_thread_b", 8, k_thread, "B_");
    intr_enable();

    while (1)
        ;
    return 0;
}

void k_thread(void* arg) {
    while (1) {
        enum intr_status old_status = intr_disable();
        if (!ioq_empty(&kbd_buf)) {
            console_print_str(arg);
            char byte = ioq_getchar(&kbd_buf);
            console_print_char(byte);
            console_print_char(' ');
        }
        intr_set_status(old_status);
    }
}
