[org 0x7c00]
; 初始化寄存器
mov ax,cs
mov ds,ax
mov es,ax
mov ss,ax
mov fs,ax
mov sp,0x7c00
; mov ax,0xb800
; mov gs,ax

; 清屏
mov ax,0600h
mov bx,0700h
mov cx,0
mov dx,184fh
int 10h

; ; 显示“1 MBR”提示语
; mov byte [gs:0x00],'1'
; mov byte [gs:0x01],0xA4
; mov byte [gs:0x02],' '
; mov byte [gs:0x03],0xA4
; mov byte [gs:0x04],'M'
; mov byte [gs:0x05],0xA4
; mov byte [gs:0x06],'B'
; mov byte [gs:0x07],0xA4
; mov byte [gs:0x08],'R'
; mov byte [gs:0x09],0xA4
mov si,boot_msg
call print

; 将loader加载器从硬盘读到内存
mov edi,LOADER_BASE_ADDR
mov bl,4
mov ecx,LOADER_START_SECTOR
call readDisk_LBA

; 跳转到加载器代码
jmp LOADER_BASE_ADDR

;--------------------------------------
; 定义常量
LOADER_BASE_ADDR equ 0x900
LOADER_START_SECTOR equ 0x2
boot_msg db 'Booting',13,10,0

; 加载封装好的函数
%include "./libs/readDisk_LBA.inc"
%include "./libs/print.inc"
;--------------------------------------

times 510-($-$$) db 0
dw 0xaa55
