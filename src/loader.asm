[org 0x1000]

dw 0x55aa

mov si,loading
call print

jmp $

%include "lib.inc"

loading:
    db "LiziOS is loading...",10,13,0