#include "../include/interrupt.h"
#include "../include/assert.h"
#include "../include/io.h"
#include "../include/stdio.h"
#include "../include/types.h"
#include "../include/thread.h"

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
// 定义各中断的名字
char *interrupt_names[interrupt_num];
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// 中断初始化函数
/// @brief 1.中断初始化
void interrupt_init()
{
    pic_init();
    idt_init();

    printf("interrupt_init is done.\n");
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
        // if (i == 0x20)
        //     c_interrupt_entry_table[i] = clock_interrupt_handler;
        // else
            c_interrupt_entry_table[i] = default_interrupt_handler;
    }

    // 初始化各中断的名称
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
    interrupt_names[32] = "This is clock interruption!";

    for (int i = 0; i < interrupt_num; i++)
        make_interrupt_descriptor(&idt[i], interrupt_desc_attr,
                                  real_interrupt_entry_table[i]);

    // 开启中断
    u64 idt_operand = ((sizeof(idt) - 1) | ((u64)((u32)idt << 16)));
    asm volatile("lidt %0" : : "m"(idt_operand));
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
    if (i == 0x27 || i == 0x2f)
        return;

    set_cursor(0);
    int cursor_pos = 0;
    while (cursor_pos < 320)
    {
        printf(" ");
        cursor_pos++;
    }
    set_cursor(0);
    printf("----- Exception message begin -----\n");
    printf("%s\n", interrupt_names[i]);
    if (i == 14)
    {
        // 缺页中断
        u32 page_fault_vaddr = 0;
        asm volatile("movl %%cr2,%0;" : "=r"(page_fault_vaddr));
        printf("page fault addr if: %x\n");
    }
    printf("----- Exception message end -----\n");
    while (1)
    {
    };
}

/// @brief 时钟中断处理函数
static void clock_interrupt_handler()
{
    printf("C ");
    struct task_struct *cur_thread = running_thread();
    ASSERT(cur_thread->stack_magic == 0x19870916);
    u32 tt = --cur_thread->ticks;
    if (cur_thread->ticks == 0)
        schedule();
    else
        cur_thread->ticks--;
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// 开关中断
enum intr_status intr_get_status()
{
    u32 eflags = 0;
    asm volatile("pushfl; popl %0" : "=g"(eflags));
    return (0x00000200 & eflags) ? INTR_ON : INTR_OFF;
}

enum intr_status intr_enable()
{
    enum intr_status old_status;
    if (INTR_ON == intr_get_status())
    {
        old_status = INTR_ON;
    }
    else
    {
        old_status = INTR_OFF;
        asm volatile("sti");
    }
    return old_status;
}

enum intr_status intr_disable()
{
    enum intr_status old_status;
    if (INTR_OFF == intr_get_status())
    {
        old_status = INTR_OFF;
    }
    else
    {
        old_status = INTR_ON;
        asm volatile("cli" : : : "memory");
    }
    return old_status;
}

enum intr_status intr_set_status(enum intr_status status)
{
    return status & INTR_ON ? intr_enable() : intr_disable();
}
// -----------------------------------------------------------------------