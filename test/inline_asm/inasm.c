#include <stdio.h>

void main() {
    int ret_cnt = 0, test = 0;
    char* fmt = "hello,world\n";
    asm volatile("   pushl %1;       \
            call printf;    \
            addl $4,%%esp;  \
            movl $6,%2"
        : "=&a"(ret_cnt)
        : "m"(fmt), "r"(test));
    printf("The number is %d\n", ret_cnt);
}