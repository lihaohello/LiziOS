#include "init.h"
#include "interrupt.h"
#include "print.h"

void init_all() {
    print_str("init_all\n");
    idt_init();
}