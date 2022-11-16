;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;End of print
; print-写字符串函数
; input: si,指向字符串首地址（注意：字符串以0结尾）
; output: 无
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
print:
  push ax
  mov ah,0xe
.next:
  mov al,[si]
  cmp al,0
  jz .done
  int 0x10
  inc si
  jmp .next
.done:
  pop ax
  ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;End of print