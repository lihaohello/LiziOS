#include "timer.h"
#include "io.h"
#include "stdio.h"
#include "types.h"

static void pic_init()
{
    // 往控制字寄存器端口0x43中写入控制字
    outb(0x43, 0b00110100);
    // 先写入低8位
    outb(0x40, (u8)(1193180 / 100));
    // 写入高8位
    outb(0x40, (u8)((1193180 / 100) >> 8));
}

// 初始化PIT8253
void timer_init()
{
    // 设置8253的定时周期,也就是发中断的周期
    pic_init();

    printf("timer_init is done\n");
}