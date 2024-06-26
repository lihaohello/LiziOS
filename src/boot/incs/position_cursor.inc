;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 使用示例
; mov dh,0
; mov dl,0
; call position_cursor 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; position_cursor: 重置光标位置（默认BH=0）
; input:
;   DH=行
;   DL=列
; output:
;   无
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
position_cursor:
  push ax
  push bx
  mov bh,0
  mov ah,0x02
  int 0x10
  pop bx
  pop ax
  ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;