#include "interrupt.h"
#include "assert.h"
#include "io.h"
#include "stdio.h"
#include "thread.h"
#include "types.h"

#define interrupt_num 0x30

/// @brief 中断描述符
struct interrupt_descriptor {
    u16 func_offset_low_word;
    u16 selector;
    u8 dcount;
    u8 attribute;
    u16 func_offset_high_word;
};

// 内部链接函数声明
static void pic_init();
static void idt_init();
static void make_idt_desc(struct interrupt_descriptor*, u8, intr_handler);
static void default_interrupt_handler(u8);

// 变量声明
char* interrupt_names[interrupt_num];
intr_handler c_interrupt_entry_table[interrupt_num];
extern intr_handler real_interrupt_entry_table[interrupt_num];
static struct interrupt_descriptor idt[interrupt_num];

void interrupt_init() {
    pic_init();
    idt_init();

    u64 idt_operand = ((sizeof(idt) - 1) | ((u64)(u32)idt << 16));
    asm volatile("lidt %0" : : "m"(idt_operand));
    printf("interrupt_init is done.\n");
}

/// @brief 可编程中断控制器初始化
static void pic_init() {
    // 写入ICW1
    outb(0x20, 0b00010001);
    outb(0xa0, 0b00010001);

    // 写入ICW2
    outb(0x21, 0x20);
    outb(0xa1, 0x28);

    // 写入ICW3
    outb(0x21, 0b00000100);
    outb(0xa1, 0b00000010);

    // 写入ICW4
    outb(0x21, 0b00000001);
    outb(0xa1, 0b00000001);

    // 写入OCW1,设置中断屏蔽，这里只开放时钟中断
    outb(0x21, 0xfc);
    outb(0xa1, 0xff);
}

/// @brief 中断描述符表初始化
static void idt_init() {
    for (int i = 0; i < interrupt_num; i++) {
        c_interrupt_entry_table[i] = default_interrupt_handler;
    }
    interrupt_names[0] = "#DE Divide Error";
    interrupt_names[1] = "#DB Debug Exception";
    interrupt_names[2] = "NMI Interrupt";
    interrupt_names[3] = "#BP Breakpoint Exception";
    interrupt_names[4] = "#OF Overflow Exception";
    interrupt_names[5] = "#BR BOUND Range Exceeded Exception";
    interrupt_names[6] = "#UD Invalid Opcode Exception";
    interrupt_names[7] = "#NM Device Not Available Exception";
    interrupt_names[8] = "#DF Double Fault Exception";
    interrupt_names[9] = "Coprocessor Segment Overrun";
    interrupt_names[10] = "#TS Invalid TSS Exception";
    interrupt_names[11] = "#NP Segment Not Present";
    interrupt_names[12] = "#SS Stack Fault Exception";
    interrupt_names[13] = "#GP General Protection Exception";
    interrupt_names[14] = "#PF Page-Fault Exception";
    interrupt_names[16] = "#MF x87 FPU Floating-Point Error";
    interrupt_names[17] = "#AC Alignment Check Exception";
    interrupt_names[18] = "#MC Machine-Check Exception";
    interrupt_names[19] = "#XF SIMD Floating-Point Exception";

    // 构造中断描述符表
    for (int i = 0; i < interrupt_num; i++)
        make_idt_desc(&idt[i], 0b10001110, real_interrupt_entry_table[i]);
}

/// @brief 创建中断描述符
/// @param p_descriptor
/// @param attr
/// @param function
static void make_idt_desc(struct interrupt_descriptor* p_descriptor,
                          u8 attr,
                          intr_handler function) {
    p_descriptor->func_offset_low_word = (u32)function & 0x0000FFFF;
    p_descriptor->selector = (1 << 3);
    p_descriptor->dcount = 0;
    p_descriptor->attribute = attr;
    p_descriptor->func_offset_high_word = ((u32)function & 0xFFFF0000) >> 16;
}

// -----------------------------------------------------------------------
/// @brief 注册中断处理函数（中断默认函数在本模块中注册），其余具体中断处理函数在相应模块中注册
/// @param num 中断编号
/// @param handler 中断处理函数
void register_handler(u8 num, intr_handler handler) {
    c_interrupt_entry_table[num] = handler;
}

// 中断处理函数
static void default_interrupt_handler(u8 intr_id) {
    if (intr_id == 0x27 || intr_id == 0x2f)
        return;

    set_cursor(0);
    int cursor_pos = 0;
    while (cursor_pos < 320) {
        printf(" ");
        cursor_pos++;
    }
    set_cursor(0);
    printf("----- Exception message begin -----\n");
    printf("%s\n", interrupt_names[intr_id]);
    if (intr_id == 14) {
        // 缺页中断
        u32 page_fault_vaddr = 0;
        asm volatile("movl %%cr2,%0;" : "=r"(page_fault_vaddr));
        printf("page fault addr if: %x\n");
    }
    printf("----- Exception message end -----\n");
    while (1)
        ;
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// 开关中断
enum intr_status intr_enable() {
    enum intr_status old_status;
    if (INTR_ON == intr_get_status()) {
        old_status = INTR_ON;
        return old_status;
    } else {
        old_status = INTR_OFF;
        asm volatile("sti");
        return old_status;
    }
}

enum intr_status intr_disable() {
    enum intr_status old_status;
    if (INTR_ON == intr_get_status()) {
        old_status = INTR_ON;
        asm volatile("cli" : : : "memory");
    } else
        old_status = INTR_OFF;
    return old_status;
}

enum intr_status intr_set_status(enum intr_status status) {
    return status & INTR_ON ? intr_enable() : intr_disable();
}

enum intr_status intr_get_status() {
    u32 eflags = 0;
    asm volatile("pushfl; popl %0;" : "=g"(eflags));
    return (0x00000200 & eflags) ? INTR_ON : INTR_OFF;
}
// -----------------------------------------------------------------------
