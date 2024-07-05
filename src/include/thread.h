#ifndef __THREAD_H
#define __THREAD_H

#include "list.h"
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

/// @brief 中断上下文备份结构
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

/// @brief 线程栈结构
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

/// @brief PCB结构
struct task_struct {
    u32* self_kstack;
    enum task_status status;
    char name[16];
    u8 priority;
    u8 ticks;
    u32 elapsed_ticks;
    struct list_elem general_tag;
    struct list_elem all_list_tag;
    u32* pgdir;
    u32 stack_magic;
};

struct task_struct* thread_start(char*, int, thread_func, void*);
void thread_init();
void schedule();
struct task_struct* thread_start(char* name,
                                 int prio,
                                 thread_func function,
                                 void* func_arg);
void thread_create(struct task_struct* pthread,
                   thread_func function,
                   void* func_arg);
void init_thread(struct task_struct* pthread, char* name, int prio);
struct task_struct* running_thread();

#endif