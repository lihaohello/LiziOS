[org 0x7c00]

; 将显示模式设置成文本模式
mov ax,3
int 0x10

;初始化堆栈
mov ax,0
mov ds,ax
mov es,ax
mov ss,ax
mov sp,0x7c00

;显示字符串-BIOS
mov si,booting
call print

;读取加载器-LBA
mov edi,0x1000
mov ecx,1
mov bl,4
call readDisk_LBA

;跳转执行加载器代码
jmp 0:0x1000

booting:
  db "LiziOS is booting...",10,13,0

;引入程序库
%include "./libs/print.inc"
%include "./libs/readDisk_LBA.inc"

times 510-($-$$) db 0

; 启动扇区最后两个字节（第510、第512个字节）
; 是引导扇区的重要标识，无此标识则不被视为启动扇区
dw 0xaa55
