[org 0x1000]

dw 0x55aa

mov si,loading
call print

jmp $

loading:
    db "LiziOS is loading...",10,13,0

%include "./libs/print.inc"