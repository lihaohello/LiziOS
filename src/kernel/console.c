#include "console.h"
#include "stdio.h"
#include "sync.h"
#include "thread.h"

static struct lock console_lock;

void console_init() {
    lock_init(&console_lock);
}

static void console_acquire() {
    lock_acquire(&console_lock);
}

static void console_release() {
    lock_release(&console_lock);
}

void console_print_str(char* str) {
    console_acquire();
    printf("%s", str);
    console_release();
}

void console_print_char(char c) {
    console_acquire();
    printf("%c", c);
    console_release();
}

void console_print_num(u32 num) {
    console_acquire();
    printf("%d", num);
    console_release();
}