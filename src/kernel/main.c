#include "assert.h"
#include "console.h"
#include "interrupt.h"
#include "ioqueue.h"
#include "keyboard.h"
#include "memory.h"
#include "process.h"
#include "stdio.h"
#include "thread.h"
#include "timer.h"
#include "tss.h"

void k_thread_a(void*);
void k_thread_b(void*);
void u_prog_a();
void u_prog_b();
int test_var_a = 0, test_var_b = 0;

int main(void) {
    printf("\n\nLiziOS is initializing the kernel...\n");

    // 中断初始化
    interrupt_init();
    // 内存管理系统初始化
    memory_init();
    // 主进程初始化
    thread_init();
    // 计时器初始化
    timer_init();
    // 控制台初始化
    console_init();
    // 键盘初始化
    keyboard_init();
    // 初始化任务
    tss_init();

    // thread_start("k_thread_a", 31, k_thread_a, "argA ");
    // thread_start("k_thread_b", 31, k_thread_b, "argB ");
    process_execute(u_prog_a, "user_prog_a");
    // process_execute(u_prog_b, "user_prog_b");
    intr_enable();

    while (1)
        ;
    return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
    char* para = arg;
    while (1) {
        console_print_str(" v_a:");
        console_print_num(test_var_a++);
    }
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {
    char* para = arg;
    while (1) {
        console_print_str(" v_b:");
        console_print_num(test_var_b++);
    }
}

/* 测试用户进程 */
void u_prog_a() {
    while (1) {
        test_var_a++;
    }
}

/* 测试用户进程 */
void u_prog_b() {
    while (1) {
        test_var_b++;
    }
}
