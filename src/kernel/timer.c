#include "timer.h"
#include "assert.h"
#include "interrupt.h"
#include "io.h"
#include "stdio.h"
#include "thread.h"
#include "types.h"

// 定义时钟频率
#define HZ 100
#define BOCHS_BREAK asm volatile("xchg %bx,%bx");

static void pic_init() {
    // 往控制字寄存器端口0x43中写入控制字
    outb(0x43, 0b00110100);
    // 先写入低8位
    outb(0x40, (u8)(1193180 / HZ));
    // 写入高8位
    outb(0x40, (u8)((1193180 / HZ) >> 8));
}

/// @brief 时钟中断执行逻辑
static void clock_interrupt_handler() {
    struct task_struct* cur_thread = running_thread();
    ASSERT(cur_thread->stack_magic == 0x19870916);

    if (cur_thread->ticks == 0){
        schedule();
    }
    else
        cur_thread->ticks--;
}

// 初始化PIT8253
void timer_init() {
    register_handler(0x20, clock_interrupt_handler);
    pic_init();

    printf("timer_init is done\n");
}