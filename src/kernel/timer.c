#include "../include/timer.h"
#include "../include/io.h"
#include "../include/print.h"
#include "../include/types.h"

#define IRQ0_FREQUENCY 100
#define INPUT_FREQUENCY 1193180
#define COUNTER0_VALUE INPUT_FREQUENCY / IRQ0_FREQUENCY
#define CONTRER0_PORT 0x40
#define COUNTER0_NO 0
#define COUNTER_MODE 2
#define READ_WRITE_LATCH 3
#define PIT_CONTROL_PORT 0x43

// 把操作的计数器counter_no、读写锁属性rwl、计数器模式counter_mode写入模式控制寄存器并赋予初始值counter_value
static void frequency_set(u8 counter_port,
                          u8 counter_no,
                          u8 rwl,
                          u8 counter_mode,
                          u16 counter_value) {
    // 往控制字寄存器端口0x43中写入控制字
    outb(PIT_CONTROL_PORT,
         (u8)(counter_no << 6 | rwl << 4 | counter_mode << 1));
    // 先写入counter_value的低8位
    outb(counter_port, (u8)counter_value);
    // 再写入counter_value的高8位
    outb(counter_port, (u8)counter_value >> 8);
}

// 初始化PIT8253
void timer_init() {
    print_str("timer_init start\n");
    // 设置8253的定时周期,也就是发中断的周期
    frequency_set(CONTRER0_PORT, COUNTER0_NO, READ_WRITE_LATCH, COUNTER_MODE,
                  COUNTER0_VALUE);
    print_str("timer_init done\n");
}