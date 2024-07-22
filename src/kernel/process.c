#include "process.h"
#include "assert.h"
#include "console.h"
#include "interrupt.h"
#include "list.h"
#include "memory.h"
#include "string.h"
#include "thread.h"
#include "tss.h"
#include "types.h"
#include "stdio.h"

#define BOCHS_BREAK asm volatile("xchg %bx,%bx");
#define PG_SIZE 4096
#define USER_VADDR_START 0x8048000
#define DIV_ROUND_UP(X, STEP) ((X + STEP - 1) / (STEP))
extern void intr_exit(void);

void start_process(void* filename) {
    void* function = filename;
    struct task_struct* cur = running_thread();
    cur->self_kstack += sizeof(struct thread_stack);
    struct intr_stack* proc_stack = (struct intr_stack*)cur->self_kstack;
    proc_stack->edi = proc_stack->esi = proc_stack->ebp =
        proc_stack->esp_dummy = 0;
    proc_stack->ebx = proc_stack->edx = proc_stack->ecx = proc_stack->eax = 0;
    proc_stack->gs = 0;
    proc_stack->ds = proc_stack->es = proc_stack->fs = 0b110011;
    proc_stack->eip = function;
    proc_stack->cs = 0b101011;
    proc_stack->eflags = ((1 < 1) | (1 << 9));
    proc_stack->esp =
        (void*)((u32)get_a_page(USER_POOL, 0xc0000000 - 0x1000) + PG_SIZE);
    proc_stack->ss = 0b110011;
    BOCHS_BREAK
    asm volatile("movl %0, %%esp; jmp intr_exit"
                 :
                 : "g"(proc_stack)
                 : "memory");
}

void set_cr3(u32 pde){
    printf("pde: 0x%x\n", pde);
    BOCHS_BREAK
    asm volatile("movl %%eax,%%cr3\n" ::"a"(pde));
}

void page_dir_activate(struct task_struct* p_thread) {
    u32 pagedir_phy_addr = 0x100000;
    if (p_thread->pgdir != NULL)
        pagedir_phy_addr = vaddr_to_paddr((u32)(p_thread->pgdir));

    printf("pagedir_phy_addr: %x\n", pagedir_phy_addr);
    printf("-----change cr3 beginning!-----\n");
    set_cr3(pagedir_phy_addr);
    BOCHS_BREAK
    // 这里会发生问题
    printf("-----change cr3 finished!-----\n");
}

void process_activate(struct task_struct* p_thread) {
    ASSERT(p_thread != NULL);
    page_dir_activate(p_thread);
    if (p_thread->pgdir) 
        update_tss_esp(p_thread);
}

u32* create_page_dir() {
    u32* page_dir_vaddr = malloc_kernel_pages(1);
    if (page_dir_vaddr == NULL) {
        console_print_str("create_page_dir: get_kernel_page failed!");
        return NULL;
    }
    // FFFF F000对应物理地址0x100000处，刚好是内核进程的页表起始位置
    // FFFF FC00是第768个页目录项开始的地方
    memcpy((u32*)((u32)page_dir_vaddr + 0x300 * 4),
           (u32*)(0xfffff000 + 0x300 * 4), 1024);

    u32 new_page_dir_phy_addr = vaddr_to_paddr((u32)page_dir_vaddr);
    page_dir_vaddr[1023] = new_page_dir_phy_addr | 4 | 2 | 1;

    return page_dir_vaddr;
}

void create_user_vaddr_bitmap(struct task_struct* user_prog) {
    user_prog->userprog_vaddr.addr_start = USER_VADDR_START;
    u32 bitmap_pg_cnt =
        DIV_ROUND_UP((0xc0000000 - USER_VADDR_START) / PG_SIZE / 8, PG_SIZE);
    user_prog->userprog_vaddr.addr_bitmap.bits =
        malloc_kernel_pages(bitmap_pg_cnt);
    user_prog->userprog_vaddr.addr_bitmap.length =
        (0xc0000000 - USER_VADDR_START) / PG_SIZE / 8;
    bitmap_init(&user_prog->userprog_vaddr.addr_bitmap);
}

void process_execute(void* filename, char* name) {
    struct task_struct* thread = malloc_kernel_pages(1);
    init_thread(thread, name, 31);
    create_user_vaddr_bitmap(thread);
    thread_create(thread, start_process, filename);
    thread->pgdir = create_page_dir();

    enum intr_status old_status = intr_disable();
    ASSERT(!elem_find(&thread_ready_list, &thread->general_tag));
    list_append(&thread_ready_list, &thread->general_tag);
    ASSERT(!elem_find(&thread_all_list, &thread->all_list_tag));
    list_append(&thread_all_list, &thread->all_list_tag);
    intr_set_status(old_status);
}