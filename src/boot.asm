[org 0x7c00]

mov ax,3
int 0x10

mov ax,0
mov ds,ax
mov es,ax
mov ss,ax
mov sp,0x7c00

;显示字符串
mov si,booting
call print

;读取加载器-BIOS
mov bx,0x1000
mov al,4
mov dl,0b10000000
mov ch,0
mov dh,0
mov cl,2
call readDisk_BIOS

;读取加载器-LBA
;xchg bx,bx
;mov edi,0x1000
;mov ecx,1
;mov bl,4
;call readDisk_LBA

cmp word [0x1000],0x55aa
jnz error

jmp fin

;完成
fin:
  hlt
  jmp $

booting:
  db "LiziOS is booting...",10,13,0

error:
  mov si,error_msg
  call print
  jmp fin

error_msg:
  db "ERROR!",10,13,0

;引入库文件
%include "./libs/print.inc"
%include "./libs/readDisk_BIOS.inc"
;%include "./libs/readDisk_LBA.inc"

times 510-($-$$) db 0

dw 0xaa55
