;输入参考
;mov ax,0
;mov es,ax
;mov bx,0x1000
;mov al,1
;mov dl,0b10000000
;mov ch,0
;mov dh,0
;mov cl,2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;End of readDisk_BIOS
; readDisk_BIOS-读磁盘函数(AH=0x02)
; input:
;       1. ES:BX=缓存位置
;       2. AL=读扇区数量
;       3. DL=驱动器号
;       4. CH=柱面号
;       5. DH=磁头号
;       6. CL=扇区号
; output: 
;       FLAGS.CF==0:没有错误,AH=0;
;       || FLAGS.CF==1:有错误,AH存储错误号;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
readDisk_BIOS:
  mov si,0x00
readDisk_BIOS_retry:
  mov ah,0x02
  int 0x13
  jnc readDisk_BIOS_over   ;读取成功，返回
  add si,1
  cmp si,5
  jae readDisk_BIOS_error  ;读取次数已经最大，返回错误
  xor ah,ah
  mov dl,0b10000000
  int 0x13  ;重置驱动器
  jmp readDisk_BIOS_retry
readDisk_BIOS_over:
  ret

readDisk_BIOS_error:
  mov si,readDisk_BIOS_error_msg
  call print
  jmp readDisk_BIOS_over

readDisk_BIOS_error_msg:
  db "Failed to read disk!",10,13,0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;End of readDisk_BIOS
