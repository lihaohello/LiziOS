[org 0x7c00]

mov ax,3
int 0x10

mov ax,0
mov ds,ax
mov es,ax
mov ss,ax
mov sp,0x7c00

mov si,booting
call print

jmp $

;引入库文件
%include "lib.inc"

booting:
  db "LiziOS is booting...",10,13,0

times 510-($-$$) db 0

dw 0xaa55
