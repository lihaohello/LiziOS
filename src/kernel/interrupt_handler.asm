[bits 32]

section .data
extern c_interrupt_entry_table
global real_interrupt_entry_table
real_interrupt_entry_table:

%macro VECTOR 2
section .text
intr%1entry:
   %ifn %2
      push 0
   %endif	
   push ds
   push es
   push fs
   push gs
   pushad

   ; 先后顺序很重要(必须在实际逻辑之前)
   mov al,0x20  
   out 0xa0,al   
   out 0x20,al    

   push %1		
   call [c_interrupt_entry_table + %1*4]  
   add esp, 4	

   popad
   pop gs
   pop fs
   pop es
   pop ds
   add esp, 4	
   iret

section .data
   dd    intr%1entry	 
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
