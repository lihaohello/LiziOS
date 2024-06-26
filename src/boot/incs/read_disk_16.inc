;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 使用示例
; mov ax,0
; mov ds,ax
; mov di,0x900
; mov cx,4
; mov ebx,2
; call read_disk_16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 读取磁盘:read_disk_16
; read_disk_16: 在实模式下读取磁盘
; input:
;   ebx=扇区逻辑号
;   cx=读入的扇区数,8位
;   di=读取后的写入内存地址
; output:
;   无
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
read_disk_16:
    ;设置读取的扇区数
    mov al,cl
    mov dx,0x1F2
    out dx,al
    
    ;设置lba地址
    ;设置低8位
    mov al,bl
    mov dx,0x1F3
    out dx,al
    
    ;设置中8位
    shr ebx,8
    mov al,bl
    mov dx,0x1F4
    out dx,al
    
    ;设置高8位
    shr ebx,8
    mov al,bl
    mov dx,0x1F5
    out dx,al
    
    ;设置高4位和device
    shr ebx,8
    and bl,0x0F
    or bl,0xE0
    mov al,bl
    mov dx,0x1F6
    out dx,al
        
    ;设置commond
    mov al,0x20
    mov dx,0x1F7
    out dx,al

.check_status:;检查磁盘状态
    nop
    in al,dx
    and al,0x88			;第4位为1表示硬盘准备好数据传输，第7位为1表示硬盘忙
    cmp al,0x08
    jnz .check_status   ;磁盘数据没准备好，继续循环检查
        
  ;设置循环次数到cx
    mov ax,cx 			;乘法ax存放目标操作数
    mov dx,256
    mul dx
    mov cx,ax			;循环次数 = 扇区数 x 512 / 2 
    mov bx,di
    mov dx,0x1F0
    
.read_data: 				
    in ax,dx			;读取数据
    mov [bx],ax			;复制数据到内存
    add bx,2    		;读取完成，内存地址后移2个字节
    
    loop .read_data
    ret