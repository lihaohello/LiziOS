;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 使用示例
; call clear_screen
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; clear_screen: 清空全屏，且将光标设置到(0,0)处
; input:
;   无
; output:
;   无
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
clear_screen:
  push ax
  push cx
  push dx
  push bx

  ; 清屏
  mov ax,0x0600
  mov cx,0x0000
  mov dh,24
  mov dl,79
  mov bh,0x17
  int 0x10

  ; 重置光标位置
  mov ah,0x02
  mov bh,0
  mov dx,0
  int 0x10

  pop bx
  pop dx
  pop cx
  pop ax
  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;