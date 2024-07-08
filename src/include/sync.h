#ifndef __SYNC_H
#define __SYNC_H

#include "list.h"
#include "thread.h"
#include "types.h"

struct semaphore {
    u8 value;
    struct list waiters;
};

struct lock {
    struct task_struct* holder;
    struct semaphore semaphore;
    u32 holder_repeat_nr;
};

#endif