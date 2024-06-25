#include "init.h"
#include "print.h"

int main(void) {
    print_str("\n\n\n\n\n\n");
    print_str("I\'m making LiziOS!\n");
    print_str("Let\'s enjoy the journey!\n");

    init_all();

    // 开始切换保护模式之前，关闭中断
    // 中断描述符加载完整后，才可以开启中断
    asm volatile("sti");
    while (1) {
    };

    return 0;
}