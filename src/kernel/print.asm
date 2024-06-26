SELECTOR_VIDEO equ 0x0003<<3

[bits 32]
section .text

;--------------------------------------
; 打印字符
global print_char
print_char:
  ; xchg bx,bx

  pushad
  mov ax,SELECTOR_VIDEO
  mov gs,ax
  
  ; 获取当前光标位置
  ; 获取高8位
  mov dx,0x03d4
  mov al,0x0e
  out dx,al
  mov dx,0x03d5
  in al,dx
  mov ah,al
  ; 获取低8位
  mov dx,0x03d4
  mov al,0x0f
  out dx,al
  mov dx,0x03d5
  in al,dx
  ; 将光标存入bx
  mov bx,ax

  ; 开始打印
  ; xchg bx,bx
  mov ecx,[esp+36]
  cmp cl,0xa
  jz .is_line_feed
  cmp cl,0xd
  jz .is_carriage_return
  cmp cl,0x8
  jz .is_backspace
  jmp .put_other

  ; 换行（同时考虑回车）
  .is_line_feed:
    add bx,80

  ; 回车
  .is_carriage_return:
    xor dx,dx
    mov ax,bx
    mov si,80
    div si
    sub bx,dx
    
    cmp bx,2000
    jl .set_cursor
    jmp .roll_screen

  ; 退格
  .is_backspace:
    dec bx
    shl bx,1
    mov byte [gs:bx],0x20
    inc bx
    mov byte [gs:bx],0x07
    shr bx,1
    jmp .set_cursor
  
  ; 其它字符
  .put_other:
    shl bx,1
    mov [gs:bx],cl
    inc bx
    mov byte [gs:bx],0x07
    shr bx,1
    inc bx
    cmp bx,2000
    jl .set_cursor 
    jmp .roll_screen
  
  ; 简单滚屏
  .roll_screen:
    cld
    mov ecx,960
    mov esi,0xc00b80a0
    mov edi,0xc00b8000
    rep movsd
    mov ebx,3840
    mov ecx,80
    .cls:
      mov word [gs:ebx],0x0720
      add ebx,2
      loop .cls
      mov bx,1920
  
  ; 根据bx的值设置光标位置
  .set_cursor:
    mov dx,0x03d4
    mov al,0x0e
    out dx,al
    mov dx,0x03d5
    mov al,bh
    out dx,al

    mov dx,0x03d4
    mov al,0x0f
    out dx,al
    mov dx,0x03d5
    mov al,bl
    out dx,al
  
  .print_char_done:
    popad
    ret
;--------------------------------------

;--------------------------------------
; 打印字符串
global print_str
print_str:
  push ebx
  push ecx
  xor ecx,ecx
  mov ebx,[esp+12]
  .goon:
    mov cl,[ebx]
    cmp cl,0
    jz .over
    push ecx
    call print_char
    add esp,4
    inc ebx
    jmp .goon
  .over:
    pop ecx
    pop ebx
  ret
;--------------------------------------


; 远期目标
;--------------------------------------
; 打印整数
;--------------------------------------

;--------------------------------------
; 打印浮点数
;--------------------------------------