[bits 32]

extern main

section .text
global _start
_start:
    mov byte [0xb8000], 'S'
    mov byte [0xb8000+2], 'B'

    xchg bx,bx

    call main

    xchg bx,bx

    jmp $
