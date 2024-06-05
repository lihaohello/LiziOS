;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 使用示例
; mov si,msg
; call print
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; print: 在界面显示字符串
; input:
;   si=指向字符串在内存中的首地址（字符串以0结尾）
; output:
;   无
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
print:
  push ax
  push si
  mov ah,0xe
.next:
  mov al,[si]
  cmp al,0
  jz .done
  int 0x10
  inc si
  jmp .next
.done:
  pop si
  pop ax
  ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;