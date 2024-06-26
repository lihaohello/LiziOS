[bits 32]
section .data
idt_msg db "Interrup occurred!",0xa,0
global intr_entry_table
intr_entry_table:

; 汇编宏定义
%define cpu_error_code nop
%define no_cpu_error_code push 0

extern print_str
%macro VECTOR 2
section .text
intr%1entry:
   %2

   ; 执行具体的中断处理函数
   push idt_msg
   call print_str
   add esp,4

   ; 结束中断
   mov al,0x20
   out 0xa0,al
   out 0x20,al
   ; 将错误码出栈
   add esp,4
   ; 返回中断函数执行前的地址
   iret

section .data
   dd intr%1entry
%endmacro

; 定义0~32号中断处理函数（不是可屏蔽中断）
VECTOR 0x00,no_cpu_error_code
VECTOR 0x01,no_cpu_error_code
VECTOR 0x02,no_cpu_error_code
VECTOR 0x03,no_cpu_error_code 
VECTOR 0x04,no_cpu_error_code
VECTOR 0x05,no_cpu_error_code
VECTOR 0x06,no_cpu_error_code
VECTOR 0x07,no_cpu_error_code 
VECTOR 0x08,cpu_error_code
VECTOR 0x09,no_cpu_error_code
VECTOR 0x0a,cpu_error_code
VECTOR 0x0b,cpu_error_code 
VECTOR 0x0c,no_cpu_error_code
VECTOR 0x0d,cpu_error_code
VECTOR 0x0e,cpu_error_code
VECTOR 0x0f,no_cpu_error_code 
VECTOR 0x10,no_cpu_error_code
VECTOR 0x11,cpu_error_code
VECTOR 0x12,no_cpu_error_code
VECTOR 0x13,no_cpu_error_code 
VECTOR 0x14,no_cpu_error_code
VECTOR 0x15,no_cpu_error_code
VECTOR 0x16,no_cpu_error_code
VECTOR 0x17,no_cpu_error_code 
VECTOR 0x18,cpu_error_code
VECTOR 0x19,no_cpu_error_code
VECTOR 0x1a,cpu_error_code
VECTOR 0x1b,cpu_error_code 
VECTOR 0x1c,no_cpu_error_code
VECTOR 0x1d,cpu_error_code
VECTOR 0x1e,cpu_error_code
VECTOR 0x1f,no_cpu_error_code 
VECTOR 0x20,no_cpu_error_code
