;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 使用示例
; mov bx,0x1000
; mov al,4
; mov dl,0b10000000
; mov ch,0
; mov dh,0
; mov cl,2
; call read_disk_BIOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; read_disk_BIOS: 使用BIOS的0x13中断读取磁盘
; input:
;   ES:BX=缓存位置
;   AL=读扇区数量
;   DL=驱动器号
;   CH=柱面号
;   DH=磁头号
;   CL=扇区号
; output: 
;   FLAGS.CF==0: 没有错误,AH=0;
;   FLAGS.CF==1: 有错误,AH存储错误号;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
read_disk_BIOS:
  push si
  push ax
  push dx
  mov si,0x00
.retry:
  mov ah,0x02
  int 0x13
  jnc .over   ;读取成功，返回
  add si,1
  cmp si,5
  jae .error  ;读取次数已经最大，返回错误
  xor ah,ah
  mov dl,0b10000000
  int 0x13  ;重置驱动器
  jmp .retry
.over:
  pop dx
  pop ax
  pop si
  ret

.error:
  mov si,msg
  call print
  jmp .over

;--------------------------------------
msg db "Failed to read the disk!",10,13,0
%include "print.inc"
;--------------------------------------
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;