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

xchg bx,bx
; 读取加载器
mov ax,0
mov es,ax
mov bx,0x1000
mov al,1
mov dl,0
mov ch,0
mov dh,0
mov cl,2
call readDisk

xchg bx,bx
jmp fin

; 读取完成
fin:
  hlt
  jmp $

;引入库文件
%include "./libs/print.inc"
%include "./libs/readDisk.inc"

booting:
  db "LiziOS is booting...",10,13,0

times 510-($-$$) db 0

dw 0xaa55
