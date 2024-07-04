#ifndef __THREAD_H
#define __THREAD_H

#include "types.h"

typedef void thread_func(void*);

enum task_status {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIED
};

struct intr_stack {
    u32 vec_no;
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 esp_dummy;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;
    u32 err_code;
    void (*eip)(void);
    u32 cs;
    u32 eflags;
    void* esp;
    u32 ss;
};

struct thread_stack {
    u32 ebp;
    u32 ebx;
    u32 edi;
    u32 esi;

    void (*eip)(thread_func* func, void* func_arg);

    void(*unused_retaddr);
    thread_func* function;
    void* func_arg;
};

struct task_struct {
    u32* self_kstack;
    enum task_status status;
    u8 priority;
    char name[16];
    u32 stack_magic;
};

struct task_struct* thread_start(char*, int, thread_func, void*);

#endif