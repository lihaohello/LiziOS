#include "../include/interrupt.h"
#include "../include/assert.h"
#include "../include/io.h"
#include "../include/stdio.h"
#include "../include/types.h"

// -----------------------------------------------------------------------
// 常量定义
#define code_selector 0b1000
#define interrupt_desc_attr 0b10001110
#define interrupt_num 0x21

/// @brief 中断描述符结构体
struct interrupt_descriptor {
    u16 func_offset_low_word;
    u16 selector;
    u8 dcount;
    u8 attribute;
    u16 func_offset_high_word;
};

// 内部链接的变量和函数声明
static struct interrupt_descriptor idt[interrupt_num];
static void pic_init();
static void idt_init();
static void make_interrupt_descriptor(struct interrupt_descriptor* p_gdesc,
                                      u8 attr,
                                      interrupt_handler function);
static void default_interrupt_handler();
static void clock_interrupt_handler();

// 外部变量声明
extern interrupt_handler real_interrupt_entry_table[interrupt_num];

// 定义C语言中断处理函数，供汇编代码的调用
interrupt_handler c_interrupt_entry_table[interrupt_num];
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// 中断初始化函数
/// @brief 1.中断初始化
void interrupt_init() {
    pic_init();
    idt_init();

    printf("interrupt_init is done.\n");
}

/// @brief 1-2.中断控制器初始化
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
    outb(0x21, 0xfe);
    outb(0xa1, 0xff);
}

/// @brief 1-1.中断描述符表初始化
static void idt_init() {
    // 先为c_interrupt_entry_table数组赋值
    for (int i = 0; i < interrupt_num; i++) {
        c_interrupt_entry_table[i] = default_interrupt_handler;
        if (i == 0x20)
            c_interrupt_entry_table[i] = clock_interrupt_handler;
    }

    for (int i = 0; i < interrupt_num; i++)
        make_interrupt_descriptor(&idt[i], interrupt_desc_attr,
                                  real_interrupt_entry_table[i]);

    // 开启中断
    u64 idt_operand = ((sizeof(idt) - 1) | ((u64)((u32)idt << 16)));
    asm volatile("lidt %0" : : "m"(idt_operand));
    asm volatile("sti");
}

/// @brief 1-1-1.创建中断描述符
/// @param p_descriptor
/// @param attr
/// @param function
static void make_interrupt_descriptor(struct interrupt_descriptor* p_descriptor,
                                      u8 attr,
                                      interrupt_handler function) {
    p_descriptor->func_offset_low_word = (u32)function & 0x0000FFFF;
    p_descriptor->selector = code_selector;
    p_descriptor->dcount = 0;
    p_descriptor->attribute = attr;
    p_descriptor->func_offset_high_word = ((u32)function & 0xFFFF0000) >> 16;
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// 中断具体处理逻辑
/// @brief 默认中断处理函数
static void default_interrupt_handler(u32 i) {
    // 当前除了时钟中断外，没有开启其它的中断，所以不会进到这里
    ASSERT(0 > 1);

    printf("the interrup id is %d\n", i);
}

/// @brief 时钟中断处理函数
static void clock_interrupt_handler() {
    printf("clock interruption occured!\n");
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// 开关中断
enum intr_status intr_get_status() {
    u32 eflags = 0;
    asm volatile("pushfl;popl %0" : "=g"(eflags));
    return (0x00000200 & eflags) ? INTR_ON : INTR_OFF;
}

enum intr_status intr_enable() {
    enum intr_status old_status;
    if (INTR_ON == intr_get_status()) {
        old_status = INTR_ON;
    } else {
        old_status = INTR_OFF;
        asm volatile("sti");
    }
    return old_status;
}

enum intr_status intr_disable() {
    enum intr_status old_status;
    if (INTR_OFF == intr_get_status()) {
        old_status = INTR_OFF;
    } else {
        old_status = INTR_ON;
        asm volatile("cli" ::: "memory");
    }
    return old_status;
}

enum intr_status intr_set_status(enum intr_status status) {
    return status & INTR_ON ? intr_enable() : intr_disable();
}
// -----------------------------------------------------------------------