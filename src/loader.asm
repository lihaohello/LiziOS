[org 0x1000]

;内存检测
check_memory:
    mov ax,0
    mov es,ax
    xor ebx,ebx
    mov edx,0x534d4150
    mov di,adrs_buffer
.next:
    mov eax,0xE820
    mov ecx,20
    int 0x15
    jc .error
    add di,cx
    inc word [adrs_count]
    cmp ebx,0
    jnz .next
    jmp prepare_protect_mode
.error:
    mov ax,0xb800
    mov es,ax
    mov byte [es:0],'E'
    jmp $

;准备进入32位保护模式
prepare_protect_mode:
    ; 关闭中断
    cli
    ; 打开A20线
    in al,0x92
    or al,0b10
    out 0x92,al
    ; 加载全局描述符表
    lgdt [gdt_ptr]
    ;启动保护模式
    mov eax,cr0
    or eax,1
    mov cr0,eax
    ;史诗级跳转
    jmp word code_selector:protect_enable
    ;出现异常
    ud2

[bits 32]
; 正式进入保护模式
protect_enable:
    ; 初始化数据段寄存器(栈段基址同数据段，一起设置)
    mov ax,data_selector
    mov ds,ax
    mov ss,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov esp,0x10000
    ;能通过保护模式内存访问方式访问内存，说保护模式启动成功
    mov byte [0xb8000],'P'
    mov byte [0xb8000+2],'P'

    ;开启分页机制
    call setup_page
    ; 向线性地址0xFFFFF0FF写入0x55aa数据，对比相应位置的物理地址和线性地址数据，一致说明分页机制开启成功
    mov ax,data_selector
    mov ds,ax
    mov dword [0xFFFFF0FF],0x55aa

    call load_kernel

    jmp code_selector: 0x11000
    jmp $

load_kernel:
    mov edi, 0x10000
    mov ecx, 10
    mov bl, 100
    call readDisk_LBA
    ret

;引入程序库
%include "./libs/readDisk_LBA.inc"

PDE equ 0x2000
PTE equ 0x3000
ATTR equ 0b11

setup_page:
    mov eax, PDE
    call .clear_page
    mov eax, PTE
    call .clear_page

    ; 前面的 1M 内 前面的 1M
    ; 前面的 1M 映射到 0xC0000000 ~ 0xC0100000
    mov eax, PTE
    or eax, ATTR
    mov [PDE], eax; 
    ;0b_00000_00000_00000_00000_00000_00000_00
    mov [PDE + 0x300 * 4], eax;
    ;0b_11000_00000_00000_00000_00000_00000_00

    mov eax, PDE
    or eax, ATTR
    mov [PDE + 0x3ff * 4], eax; 吧最后一个页表指向了页目录

    mov ebx, PTE
    mov ecx, (0x100000 / 0x1000); 256
    mov esi, 0
    ; xchg bx, bx

.next_page:
    mov eax, esi
    shl eax, 12
    or eax, ATTR

    mov [ebx + esi * 4], eax
    inc esi
    loop .next_page

    ; xchg bx, bx
    mov eax, PDE
    mov cr3, eax

    mov eax, cr0
    or eax, 0b1000_0000_0000_0000_0000_0000_0000_0000
    mov cr0, eax

    ret

.clear_page:
    ; 清空一个内存页，地址参数存在 eax 中
    mov ecx, 0x1000
    mov esi, 0
.set:
    mov byte [eax + esi], 0
    inc esi
    loop .set
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; 以下定义一些关键符号
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 定义一些符号常量
base equ 0
limit equ 0xfffff

;代码段、数据段选择子
code_selector equ (1 << 3)
data_selector equ (2 << 3)

; 全局描述符表的大小和基址
gdt_ptr:
    dw (gdt_end-gdt_base-1)
    dw gdt_base

; 第0个全局描述符
gdt_base:
    dd 0,0
; 第1个全局描述符
gdt_code:
    dw limit & 0xffff
    dw base & 0xffff
    db (base >> 16) & 0xff
    db 0b1110 | 0b1001_0000
    db 0b1100_0000 | (limit >> 16)
    db (base >> 24) & 0xff
;第2个全局描述符
gdt_data:
    dw limit & 0xffff
    dw base & 0xffff
    db (base >> 16) & 0xff
    db 0b0010 | 0b1001_0000
    db 0b1100_0000 | (limit >> 16)
    db (base >> 24) & 0xff
;全局描述符表结束地址
gdt_end:

adrs_count: 
dw 0
adrs_buffer:
