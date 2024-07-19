#ifndef __TSS_H
#define __TSS_H

#include "thread.h"
#include "types.h"

struct gdt_desc {
    u16 limit_low_word;
    u16 base_low_word;
    u8 base_mid_byte;
    u8 attr_low_byte;
    u8 limit_high_attr_high;
    u8 base_high_byte;
};

void update_tss_esp(struct task_struct* pthread);
void tss_init(void);

#endif