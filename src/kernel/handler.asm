[bits 32]
section .data
extern c_interruption_entry_table

; 定义中断描述符表的起点
global real_interruption_entry_table
real_interruption_entry_table:

; 汇编宏定义
%macro VECTOR 2
section .text
extern c_interruption_entry_table
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

   ; 执行具体的中断处理函数
   call [c_interruption_entry_table+4*%1]

   ; 结束中断
   mov al,0x20
   out 0xa0,al
   out 0x20,al

   ; 还原执行现场
   popad
   pop gs
   pop fs
   pop es
   pop ds

   ; 将错误码出栈
   add esp,4
   ; 返回中断函数执行前的地址
   iret

section .data
   dd intr%1entry
%endmacro


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
