section mbr vstart=0x7c00
  mov ax,cs
  mov ds,ax
  mov es,ax
  mov ss,ax
  mov fs,ax
  mov sp,0x7c00
  mov ax,0xb800
  mov gs,ax

  ; 清屏
  mov ax,0600h
  mov bx,0700h
  mov cx,0
  mov dx,184fh
  int 10h

  ; 显示“1 MBR”提示语
  mov byte [gs:0x00],'1'
  mov byte [gs:0x01],0xA4
  mov byte [gs:0x02],' '
  mov byte [gs:0x03],0xA4
  mov byte [gs:0x04],'M'
  mov byte [gs:0x05],0xA4
  mov byte [gs:0x06],'B'
  mov byte [gs:0x07],0xA4
  mov byte [gs:0x08],'R'
  mov byte [gs:0x09],0xA4

  ; 读硬盘
  mov edi,LOADER_BASE_ADDR
  mov bl,1
  mov ecx,LOADER_START_SECTOR
  call readDisk_LBA

  xchg bx,bx
  jmp LOADER_BASE_ADDR

  %include "boot.inc"
  %include "./libs/readDisk_LBA.inc"

  times 510-($-$$) db 0
  dw 0xaa55
