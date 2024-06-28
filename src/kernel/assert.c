#include "../include/assert.h"
#include "../include/stdio.h"
#include "../include/interrupt.h"

void panic_spin(char* filename,
                int line,
                const char* func,
                const char* condition) {
    intr_disable();
    
    print_str("\n-----ERROR-----\n");
    print_str("filename:");
    print_str(filename);
    print_str("\n");

    print_str("function:");
    print_str((char*)func);
    print_str("\n");

    print_str("condition:");
    print_str((char*)condition);
    print_str("\n");
    while (1) {
    };
}