#include "sync.h"
#include "assert.h"
#include "interrupt.h"

void sema_init(struct semaphore* psema, u8 value) {
    psema->value = value;
    list_init(&psema->waiters);
}

void sema_dowm(struct semaphore* psema) {
    enum intr_status old_status = intr_disable();

    while (psema->value == 0) {
        ASSERT(!elem_find(&psema->waiters, &running_thread()->general_tag));
        list_append(&psema->waiters, &running_thread()->general_tag);
        thread_block(TASK_BLOCKED);
    }
    psema->value--;
    ASSERT(psema->value == 0);

    intr_set_status(old_status);
}

void sema_up(struct semaphore* psema) {
    enum intr_status old_status = intr_disable();
    ASSERT(psema->value == 0);
    if (!list_empty(&psema->waiters)) {
        struct task_struct* thread_blocked = elem2entry(
            struct task_struct, general_tag, list_pop(&psema->waiters));
        thread_unblock(thread_blocked);
    }
    psema->value++;
    ASSERT(psema->value == 1);
    intr_set_status(old_status);
}

void lock_init(struct lock* plock) {
    plock->holder = NULL;
    plock->holder_repeat_nr = 0;
    sema_init(&plock->semaphore, 1);
}

void lock_acquire(struct lock* plock) {
    
}

void lock_release(struct lock* plock) {}