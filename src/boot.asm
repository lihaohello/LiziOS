[org 0x7c00]

mov ax,3
int 0x10

mov ax,0
mov ds,ax
mov es,ax
mov ss,ax
mov sp,0x7c00

xchg bx,bx
;显示字符串
mov si,booting
call print

mov ax,0
mov ds,ax
mov es,ax
mov ss,ax
mov sp,0x7c00
; 读取加载器
call readloader

jmp $

;引入库文件
%include "lib.inc"

readloader:
  mov dx,0x0000
  mov cx,0x0002
  mov bx,0x1000
  mov ax,0x0204
  int 0x13
  jnc okloader
  mov dx,0x0000
  mov ax,0x0000
  int 0x13
  jmp readloader
okloader:
  ret

booting:
  db "LiziOS is booting...",10,13,0

times 510-($-$$) db 0

dw 0xaa55
