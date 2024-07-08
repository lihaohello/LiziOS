#include "thread.h"
#include "assert.h"
#include "interrupt.h"
#include "memory.h"
#include "stdio.h"
#include "string.h"

#define PG_SIZE 4096
#define BOCHS_BREAK asm volatile("xchg %bx,%bx");

struct task_struct* main_thread;
struct list thread_ready_list;
struct list thread_all_list;
static struct list_elem* thread_tag;
extern void switch_to(struct task_struct* cur, struct task_struct* next);

/// @brief 获取当前线程的PCB指针
/// @return
struct task_struct* running_thread() {
    u32 esp;
    asm volatile("mov %%esp, %0;" : "=g"(esp));
    return (struct task_struct*)(esp & 0xfffff000);
}

/// @brief 由内核线程执行的函数
/// @param function
/// @param func_arg
static void kernel_thread(thread_func* function, void* func_arg) {
    intr_enable();
    function(func_arg);
}

void init_thread(struct task_struct* pthread, char* name, int prio) {
    memset(pthread, 0, sizeof(*pthread));
    strcpy(pthread->name, name);

    if (pthread == main_thread)
        pthread->status = TASK_RUNNING;
    else
        pthread->status = TASK_READY;

    pthread->self_kstack = (u32*)((u32)pthread + PG_SIZE);
    pthread->priority = prio;
    pthread->ticks = prio;
    pthread->pgdir = NULL;
    pthread->stack_magic = 0x19870916;  // 自定义的魔数
}

void thread_create(struct task_struct* pthread,
                   thread_func function,
                   void* func_arg) {
    /* 先预留中断使用栈的空间,可见thread.h中定义的结构 */
    pthread->self_kstack -= sizeof(struct intr_stack);

    /* 再留出线程栈空间,可见thread.h中定义 */
    pthread->self_kstack -= sizeof(struct thread_stack);
    struct thread_stack* kthread_stack =
        (struct thread_stack*)pthread->self_kstack;
    kthread_stack->ebp = kthread_stack->ebx = kthread_stack->esi =
        kthread_stack->edi = 0;

    kthread_stack->eip = kernel_thread;

    kthread_stack->function = function;
    kthread_stack->func_arg = func_arg;
}

struct task_struct* thread_start(char* name,
                                 int prio,
                                 thread_func function,
                                 void* func_arg) {
    struct task_struct* thread = get_kernel_pages(1);

    init_thread(thread, name, prio);
    thread_create(thread, function, func_arg);

    ASSERT(!elem_find(&thread_ready_list, &thread->general_tag));
    list_append(&thread_ready_list, &thread->general_tag);
    ASSERT(!elem_find(&thread_all_list, &thread->all_list_tag));
    list_append(&thread_all_list, &thread->all_list_tag);
    return thread;
}

// 进程调度
void schedule() {
    // 当前线程要被换下，恢复其滴答数
    struct task_struct* cur = running_thread();
    if (cur->status == TASK_RUNNING) {
        ASSERT(!elem_find(&thread_ready_list, &cur->general_tag));
        list_append(&thread_ready_list, &cur->general_tag);
        cur->ticks = cur->priority;
        cur->status = TASK_READY;
    } else {
    }

    ASSERT(!list_empty(&thread_ready_list));
    thread_tag = NULL;
    thread_tag = list_pop(&thread_ready_list);
    struct task_struct* next =
        elem2entry(struct task_struct, general_tag, thread_tag);
    next->status = TASK_RUNNING;
    switch_to(cur, next);
}

static void make_main_thread() {
    main_thread = running_thread();
    init_thread(main_thread, "main", 4);

    ASSERT(!elem_find(&thread_all_list, &main_thread->all_list_tag));
    list_append(&thread_all_list, &main_thread->all_list_tag);
}

void thread_init() {
    list_init(&thread_ready_list);
    list_init(&thread_all_list);
    make_main_thread();
    printf("thread_init is done.\n");
}

/// @brief 线程阻塞
/// @param
void thread_block(enum task_status status) {
    ASSERT((status == TASK_BLOCKED) || (status == TASK_WAITING) ||
           (status == TASK_HANGING));
    enum intr_status old_status = intr_disable();
    struct task_struct* cur_thread = running_thread();
    cur_thread->status = status;
    schedule();
    intr_set_status(old_status);
}

/// @brief 线程唤醒
/// @param pthread
void thread_unblock(struct task_struct* pthread) {
    enum intr_status old_status = intr_disable();
    enum task_status status = pthread->status;
    ASSERT((status == TASK_BLOCKED) || (status == TASK_WAITING) ||
           (status == TASK_HANGING));
    if (elem_find(&thread_ready_list, &pthread->general_tag)) {
        PANIC("thread is not blocked, but in ready_list");
    }
    list_push(&thread_ready_list, &pthread->general_tag);
    pthread->status = TASK_READY;
    intr_set_status(old_status);
}