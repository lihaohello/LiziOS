#include "../include/interrupt.h"
#include "../include/print.h"

int main(void) {
    print_str("\n\nLiziOS is initializing the kernel...\n");

    // 中断初始化
    interrupt_init();

    while (1) {
    };

    return 0;
}