#ifndef __IOQUEUE_H
#define __IOQUEUE_H

#include "sync.h"
#include "thread.h"
#include "types.h"

#define bufsize 64

struct ioqueue {
    struct lock lock;
    struct task_struct* producer;
    struct task_struct* consumer;
    char buf[bufsize];
    u32 head;
    u32 tail;
};

void ioq_init(struct ioqueue* ioq);
u32 next_pos(u32 pos);
bool ioq_full(struct ioqueue* ioq);
bool ioq_empty(struct ioqueue* ioq);
char ioq_getchar(struct ioqueue* ioq);
void ioq_putchar(struct ioqueue* ioq, char byte);

#endif