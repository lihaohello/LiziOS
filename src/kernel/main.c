#include "interrupt.h"
#include "print.h"

int main(void) {
    print_str("\n\n\n\n\n\nEnter the kernel...\n");

    // 中断初始化
    interrupt_init();

    // asm volatile("sti");
    while (1) {
    };

    return 0;
}