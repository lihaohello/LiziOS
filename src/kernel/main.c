#include "../include/assert.h"
#include "../include/interrupt.h"
#include "../include/stdio.h"
#include "../include/timer.h"

int main(void) {
    printf("\n\nLiziOS is initializing the kernel...\n");

    // 中断初始化
    interrupt_init();
    // 计时器初始化
    timer_init();

    ASSERT(1 == 2);

    while (1) {
    };

    return 0;
}
