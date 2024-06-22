#include "init.h"
#include "print.h"

int main(void) {
    print_str("\n\n\n\n\n\n");
    print_str("I\'m making LiziOS!\n");
    print_str("Let\'s enjoy the journey!\n");

    init_all();
    asm volatile("sti");
    while (1) {
    };

    return 0;
}