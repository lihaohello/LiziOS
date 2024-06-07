[org 0x7c00]
; 初始化寄存器
mov ax,cs
mov ds,ax
mov es,ax
mov ss,ax
mov fs,ax
mov sp,0x7c00

; 清屏并重置光标位置
call clear_screen
; 打印提示字符
mov si,boot_msg
call print

; 将loader加载器从硬盘读到内存
mov ax,0
mov ds,ax
mov di,0x900
mov cx,4
mov ebx,2
call read_disk_16

; 跳转到加载器代码
jmp loader_base_addr

;--------------------------------------
; 定义常量
; 加载器在内存中的起点地址
loader_base_addr equ 0x900
; 启动提示字符串
boot_msg db 'Booting...',13,10,0

; 加载封装好的函数
%include "./incs/position_cursor.inc"
%include "./incs/clear_screen.inc"
%include "./incs/print.inc"
%include "./incs/read_disk_16.inc"
;--------------------------------------

times 510-($-$$) db 0
dw 0xaa55
