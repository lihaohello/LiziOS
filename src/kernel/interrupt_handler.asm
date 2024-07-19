[bits 32]
section .data
extern c_interrupt_entry_table

; 定义中断描述符表的起点
global real_interrupt_entry_table
real_interrupt_entry_table:

; 汇编宏定义
%macro VECTOR 2
section .text
intr%1entry:
   %ifn %2
      push 0
   %endif

   ; 进入中断执行逻辑之前，先保存执行现场
   push ds
   push es
   push fs
   push gs
   pushad
   
   ; 结束中断（必须在执行之前发送EOI，应该在中断逻辑中涉及进程切换，代码不会走这一步）
   mov al,0x20
   out 0xa0,al
   out 0x20,al

   ; 执行具体的中断处理函数
   push %1
   call [c_interrupt_entry_table+4*%1]
   jmp intr_exit
   
section .data
   dd intr%1entry
%endmacro

section .text
global intr_exit
intr_exit:	     
   add esp, 4	
   popad
   pop gs
   pop fs
   pop es
   pop ds
   add esp, 4	
   iretd

; 定义0~32号中断处理函数（不是可屏蔽中断）
VECTOR 0x00,0
VECTOR 0x01,0
VECTOR 0x02,0
VECTOR 0x03,0 
VECTOR 0x04,0
VECTOR 0x05,0
VECTOR 0x06,0
VECTOR 0x07,0 
VECTOR 0x08,1
VECTOR 0x09,0
VECTOR 0x0a,1
VECTOR 0x0b,1 
VECTOR 0x0c,0
VECTOR 0x0d,1
VECTOR 0x0e,1
VECTOR 0x0f,0 
VECTOR 0x10,0
VECTOR 0x11,1
VECTOR 0x12,0
VECTOR 0x13,0 
VECTOR 0x14,0
VECTOR 0x15,0
VECTOR 0x16,0
VECTOR 0x17,0 
VECTOR 0x18,1
VECTOR 0x19,0
VECTOR 0x1a,1
VECTOR 0x1b,1 
VECTOR 0x1c,0
VECTOR 0x1d,1
VECTOR 0x1e,1
VECTOR 0x1f,0 

VECTOR 0x20,0
VECTOR 0x21,0
VECTOR 0x22,0
VECTOR 0x23,0
VECTOR 0x24,0
VECTOR 0x25,0
VECTOR 0x26,0
VECTOR 0x27,0
VECTOR 0x28,0
VECTOR 0x29,0
VECTOR 0x2a,0
VECTOR 0x2b,0
VECTOR 0x2c,0
VECTOR 0x2d,0
VECTOR 0x2e,0
VECTOR 0x2f,0