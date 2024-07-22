#ifndef __PROCESS_H
#define __PROCESS_H
#include "types.h"
#include "thread.h"

void process_execute(void* filename, char* name);
void start_process(void* filename_);
void process_activate(struct task_struct* p_thread);
void page_dir_activate(struct task_struct* p_thread);
u32* create_page_dir();
void create_user_vaddr_bitmap(struct task_struct* user_prog);

#endif