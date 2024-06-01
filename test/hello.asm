SECTION .data                    ; Section containing initialised data
    HelloMsg db "Hello World!",10
    HelloLen equ $-HelloMsg 

SECTION .bss                    ; Section containing uninitialized data    

SECTION .text                   ; Section containing code

global  _start                 ; Linker needs this tofind the entry point!

_start:
    nop                             ; This no-op keeps gdbhappy...
    mov eax,4                  ; Specify sys_writecall
    mov ebx,1                  ; Specify FileDescriptor 1: Standard Output

    mov ecx,HelloMsg    ; Pass offset ofthe message
    mov edx,HelloLen     ; Pass the lengthof the message

    int 80H                        ; Make kernelcall

    mov eax,1                  ; Code for ExitSyscall
    mov ebx,0                  ; Return a code ofzero   

    int 80H                        ; Make kernelcall