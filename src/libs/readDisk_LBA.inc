;输入参考
; xchg bx,bx
; mov edi,0x1000
; mov ecx,1
; mov bl,4
; call readDisk_LBA

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;End of readDisk_LBA
; readDisk_LBA-采用IDE/ATA PIO方式读取磁盘数据
; input:
;       1. edi=内存地址
;       2. ecx=起始扇区
;       3. bl=扇区数量
; output: 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
readDisk_LBA:
    mov dx,0x1f2
    ;设置读写扇区的数量
    mov al,bl
    out dx,al

    ;设置起始扇区
    mov al,cl
    inc dx
    out dx,al

    mov al,ch
    inc dx
    out dx,al

    shr ecx,16
    mov al,cl
    inc dx
    out dx,al

    shr ecx,8
    and cl,0b0000_1111

    ;设置相关指令
    mov al,0b1110_0000
    or al,cl
    inc dx
    out dx,al

    ;设置读写硬盘
    mov al,0x20
    inc dx
    out dx,al

    ;读写硬盘操作
    xor ecx,ecx
    mov cl,bl

    .read:
        push cx
        call .waits
        call .reads
        pop cx
        loop .read
    ret

    ;等待数据准备完毕
    .waits:
        mov dx,0x1f7
        .check:
            in al,dx
            jmp $+2
            jmp $+2
            jmp $+2
            and al,0b1000_1000
            cmp al,0b0000_1000
            jnz .check
        ret
    
    ;开始读写数据
    .reads:
        mov dx,0x1f0
        mov cx,256
        .readw:
            in ax,dx
            jmp $+2
            jmp $+2
            jmp $+2
            mov [edi],ax
            add edi,2
            loop .readw
        ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;End of readDisk_LBA
