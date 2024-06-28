#include "../include/interrupt.h"
#include "../include/io.h"
#include "../include/print.h"
#include "../include/types.h"

// -----------------------------------------------------------------------
// 常量定义
#define SELECTOR_K_CODE 0b1000
#define IDT_DESC_ATTR_DPL0 0b10001110
#define IDT_DESC_CNT 0x21

/// @brief 中断描述符结构体
struct interrupt_descriptor {
    u16 func_offset_low_word;
    u16 selector;
    u8 dcount;
    u8 attribute;
    u16 func_offset_high_word;
};

// 内部链接的变量和函数声明
static struct interrupt_descriptor idt[IDT_DESC_CNT];
static void pic_init();
static void idt_init();
static void make_interrupt_descriptor(struct interrupt_descriptor* p_gdesc,
                                      u8 attr,
                                      interrupt_handler function);
static void default_interrupt_handler();
static void clock_interrupt_handler();

// 外部变量声明
extern interrupt_handler real_interrupt_entry_table[IDT_DESC_CNT];

// 定义C语言中断处理函数，供汇编代码的调用
interrupt_handler c_interrupt_entry_table[IDT_DESC_CNT];
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// 中断初始化函数
/// @brief 1.中断初始化
void interrupt_init() {
    pic_init();
    idt_init();

    print_str("Interrupt is initialized.\n");
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
    for (int i = 0; i < IDT_DESC_CNT; i++) {
        if (i == 0x20) {
            c_interrupt_entry_table[i] = clock_interrupt_handler;
            continue;
        }
        c_interrupt_entry_table[i] = default_interrupt_handler;
    }

    for (int i = 0; i < IDT_DESC_CNT; i++)
        make_interrupt_descriptor(&idt[i], IDT_DESC_ATTR_DPL0,
                                  real_interrupt_entry_table[i]);

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
    p_descriptor->selector = SELECTOR_K_CODE;
    p_descriptor->dcount = 0;
    p_descriptor->attribute = attr;
    p_descriptor->func_offset_high_word = ((u32)function & 0xFFFF0000) >> 16;
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// 中断具体处理逻辑
/// @brief 默认中断处理函数
static void default_interrupt_handler() {
    print_str("default_interrupt_handler\n");
}

/// @brief 时钟中断处理函数
static void clock_interrupt_handler() {
    print_str("clock interruption occured!\n");
}
// -----------------------------------------------------------------------


