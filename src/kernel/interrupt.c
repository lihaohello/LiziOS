#include "../include/interrupt.h"
#include "../include/io.h"
#include "../include/print.h"
#include "../include/types.h"

// -----------------------------------------------------------------------
// 常量定义
#define code_selector 0b1000
#define interrupt_desc_attr 0b10001110
#define interrupt_num 0x21

/// @brief 中断描述符结构体
struct interrupt_descriptor
{
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
static void make_interrupt_descriptor(struct interrupt_descriptor *p_gdesc,
                                      u8 attr,
                                      interrupt_handler function);
static void default_interrupt_handler();
static void clock_interrupt_handler();

// 外部变量声明
extern interrupt_handler real_interrupt_entry_table[interrupt_num];

// 定义C语言中断处理函数，供汇编代码的调用
interrupt_handler c_interrupt_entry_table[interrupt_num];
// 定义0~0x20号中断的名称，用于打印调试
char *interrupt_name[interrupt_num];
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// 中断初始化函数
/// @brief 1.中断初始化
void interrupt_init()
{
    pic_init();
    idt_init();

    print_str("Interrupt is initialized.\n");
}

/// @brief 1-2.中断控制器初始化
static void pic_init()
{
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
static void idt_init()
{
    // 先为c_interrupt_entry_table数组赋值
    for (int i = 0; i < interrupt_num; i++)
    {
        c_interrupt_entry_table[i] = default_interrupt_handler;
        if (i == 0x20)
            c_interrupt_entry_table[i] = clock_interrupt_handler;
    }
    // 为各中断的名字赋值
    interrupt_name[0] = "0x00";
    interrupt_name[1] = "0x01";
    interrupt_name[2] = "0x02";
    interrupt_name[3] = "0x03";
    interrupt_name[4] = "0x04";
    interrupt_name[5] = "0x05";
    interrupt_name[6] = "0x06";
    interrupt_name[7] = "0x07";
    interrupt_name[8] = "0x08";
    interrupt_name[9] = "0x09";
    interrupt_name[10] = "0x0a";
    interrupt_name[11] = "0x0b";
    interrupt_name[12] = "0x0c";
    interrupt_name[13] = "0x0d";
    interrupt_name[14] = "0x0e";
    interrupt_name[15] = "0x0f";
    interrupt_name[16] = "0x10";
    interrupt_name[17] = "0x11";
    interrupt_name[18] = "0x12";
    interrupt_name[19] = "0x13";
    interrupt_name[20] = "0x14";
    interrupt_name[21] = "0x15";
    interrupt_name[22] = "0x16";
    interrupt_name[23] = "0x17";
    interrupt_name[24] = "0x18";
    interrupt_name[25] = "0x19";
    interrupt_name[26] = "0x1a";
    interrupt_name[27] = "0x1b";
    interrupt_name[28] = "0x1c";
    interrupt_name[29] = "0x1d";
    interrupt_name[30] = "0x1e";
    interrupt_name[31] = "0x1f";
    interrupt_name[32] = "0x20";

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
static void make_interrupt_descriptor(struct interrupt_descriptor *p_descriptor,
                                      u8 attr,
                                      interrupt_handler function)
{
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
static void default_interrupt_handler(u32 i)
{
    print_str("interrupt number is: \t");
    print_str(interrupt_name[i]);
    print_str("\n");
}

/// @brief 时钟中断处理函数
static void clock_interrupt_handler()
{
    print_str("clock interruption occured!\n");
}
// -----------------------------------------------------------------------
