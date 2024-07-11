#include "tss.h"
#include "stdio.h"
#include "string.h"
#include "thread.h"
#include "types.h"

struct tss {
    u32 backlink;

    u32* esp0;
    u32 ss0;
    u32* esp1;
    u32 ss1;
    u32* esp2;
    u32 ss2;

    u32 cr3;

    u32 (*eip)(void);
    u32 eflags;

    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;

    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;

    u32 ldt;

    u32 trace;
    u32 io_base;
};
static struct tss tss;

void update_tss_esp(struct task_struct* pthread) {
    tss.esp0 = (u32*)((u32)pthread + PG_SIZE);
}

static struct gdt_desc make_gdt_desc(u32* desc_addr,
                                     u32 limit,
                                     u8 attr_low,
                                     u8 attr_high) {
    u32 desc_base = (u32)desc_addr;
    struct gdt_desc desc;
    desc.limit_low_word = (u16)(limit & 0x0000ffff);
    desc.base_low_word = (u16)(desc_base & 0x0000ffff);
    desc.base_mid_byte = (u8)((desc_base & 0x00ff0000) >> 16);
    desc.attr_low_byte = attr_low;
    desc.limit_high_attr_high =
        (((limit & 0x000f0000) >> 16) + (u8)(attr_high));
    desc.base_high_byte = desc_base >> 24;
    return desc;
}

void tss_init() {
    u32 tss_size = sizeof(tss);
    memset(&tss, 0, tss_size);
    tss.ss0 = (2 << 3);
    tss.io_base = tss_size;

    u32 gdt_base = *((u32*)0xd04);
    *((struct gdt_desc*)(gdt_base + 0x20)) =
        make_gdt_desc((u32*)&tss, tss_size - 1, 0b10001001, 0b10000000);
    *((struct gdt_desc*)(gdt_base + 0x28)) =
        make_gdt_desc((u32*)0, 0xfffff, 0b11111000, 0b11000000);
    *((struct gdt_desc*)(gdt_base + 0x30)) =
        make_gdt_desc((u32*)0, 0xfffff, 0b11110010, 0b11000000);

    u64 gdt_operand = ((8 * 7 - 1) | ((u64)gdt_base << 16));
    asm volatile("lgdt %0" : : "m"(gdt_operand));
    asm volatile("ltr %w0" : : "r"(4 << 3));

    printf("tss_init is done.\n");
}
