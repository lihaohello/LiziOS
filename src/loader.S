section loader vstart=LOADER_BASE_ADDR
jmp loader_start

%include "./libs/print.inc"
loadermsg db '2 loader in real.',0

; 开始切换到保护模式
loader_start:
; 打印信息
mov si,loadermsg
call print

; 打开A20，关闭地址回绕机制
in al,0x92
or al,0b10
out 0x92,al
; 加载GDT
lgdt [gdt_ptr]
; 开启分段机制
mov eax,cr0
or eax,1
mov cr0,eax
; 跳转到保护模式代码，刷新流水线
jmp dword code_selector:p_mode_start

; 保护模式代码
[bits 32]
p_mode_start:
  mov ax,data_selector
  mov ds,ax
  mov es,ax
  mov ss,ax
  mov fs,ax
  mov esp,LOADER_BASE_ADDR
  mov ax,video_selector
  mov gs,ax
  mov byte [gs:160],'P'
  jmp $

; 相关定义
LOADER_BASE_ADDR equ 0x900
LOADER_START_SECTOR equ 0x2

; 选择子
code_selector equ (1<<3)
data_selector equ (2<<3)
video_selector equ (3<<3)
; 定义GDT表的长度和基址
gdt_ptr:
  dw (gdt_end-gdt_base-1)
  dd gdt_base
; 各全局描述符的限长和基址
base equ 0          ; 段基址
limit equ 0xfffff   ; 段大小限值
video_base equ 0xb8000
video_limit equ 0x7
; 定义3个全局描述符（第0个无效）
gdt_base:
  dd 0,0
gdt_code:
  dw limit & 0xffff
  dw base & 0xffff
  db (base>>16) & 0xff
  db 0b1110 | 0b1001_0000 
  db 0b1100_0000 | (limit >> 16)
  db (base >> 24) & 0xff
gdt_data:
  dw limit & 0xffff
  dw base & 0xffff
  db (base >> 16) & 0xff
  db 0b0010 | 0b1001_0000
  db 0b1100_0000 | (limit >> 16)
  db (base >> 24) & 0xff
gdt_video:
  dw video_limit & 0xffff
  dw video_base & 0xffff
  db (video_base >> 16) & 0xff
  db 0b0010 | 0b1001_0000
  db 0b1100_0000 | (video_limit >> 16)
  db (video_base >> 24) & 0xff
gdt_end: