[org 0x900]

; 检测内存
detect_memory:
  xor ebx,ebx
  mov ax,ds
  mov es,ax
  mov edi,ards_buffer
  mov edx,0x534d4150
  .next:
    mov eax,0xe820
    mov ecx,20
    int 0x15
    jc .error
    add di,cx
    inc dword [ards_count]
    cmp ebx,0
    jnz .next
    jmp get_total_mem_bytes 
  .error:
    mov si, .msg
    call print
    hlt
    jmp $
    .msg db "Memory detection failed.", 10, 13, 0

; 计算内存容量，写入地址total_mem_bytes
get_total_mem_bytes:
  mov cx,[ards_count]
  mov ebx,ards_buffer
  xor edx,edx
  .find_max_mem_area:
    mov eax,[ebx]
    add eax,[ebx+8]
    add ebx,20
    cmp edx,eax
    jge .next_ards
    mov edx,eax
  .next_ards:
    loop .find_max_mem_area
  mov [total_mem_bytes],edx
  ; 将内存容量写到指定内存处，供内存初始化使用
  mov [0xd00],edx
  ; 将gdt的地址写到内存，后续可以读取
  mov dword [0xd04],gdt_base
; 查看内存容量
mov eax,[total_mem_bytes]
; 输出提示信息，重置光标
mov si,memory_detect_msg
call print

; 开始切换到保护模式
; 先关闭中断，不然在qemu中会出错
cli
; 打开A20，以关闭地址回绕机制
in al,0x92
or al,0b10
out 0x92,al
; 加载GDT
lgdt [gdt_ptr]
; 开启分段机制
mov eax,cr0
or eax,1
mov cr0,eax
; 跳转到保护模式代码，刷新流水线
jmp dword code_selector:p_mode_start

;--------------------------------------
; 变量定义和函数引入
loader_base_addr equ 0x900
memory_detect_msg db "Memory detected.",10,13,0
%include "./boot/incs/print.inc"
%include "./boot/incs/position_cursor.inc"
kernel_bin_addr equ 0x70000
;--------------------------------------

; 保护模式代码
[bits 32]
p_mode_start:
  mov ax,data_selector
  mov ds,ax
  mov es,ax
  mov ss,ax
  mov fs,ax
  mov esp,loader_base_addr
  mov ax,video_selector
  mov gs,ax
  mov byte [gs:320],'S'
  mov byte [gs:322],'e'
  mov byte [gs:324],'g'
  mov byte [gs:326],'m'
  mov byte [gs:328],'e'
  mov byte [gs:330],'n'
  mov byte [gs:332],'t'
  mov byte [gs:334],'i'
  mov byte [gs:336],'n'
  mov byte [gs:338],'g'
  mov byte [gs:340],' '
  mov byte [gs:342],'o'
  mov byte [gs:344],'p'
  mov byte [gs:346],'e'
  mov byte [gs:348],'n'
  mov byte [gs:350],'e'
  mov byte [gs:352],'d'
  mov byte [gs:354],'.'

; 加载内核到内存
; edi指内核加载到内存的位置
; ecx指内核起始扇区号
; bl指内核扇区数量
; 写入磁盘时需要按照此规则
mov edi, kernel_bin_addr
mov ecx, 9
mov bl, 200
call read_disk_32

; 开启分页机制
call setup_page
sgdt [gdt_ptr]
mov ebx,[gdt_ptr+2]
or dword [ebx+0x18+4],0xc0000000
add dword [gdt_ptr+2],0xc0000000
add esp,0xc0000000
mov eax,page_dir_table_pos
mov cr3,eax
mov eax,cr0
or eax,0x80000000
mov cr0,eax
lgdt [gdt_ptr]
mov byte [gs:480],'P'
mov byte [gs:482],'a'
mov byte [gs:484],'g'
mov byte [gs:486],'i'
mov byte [gs:488],'n'
mov byte [gs:490],'g'
mov byte [gs:492],' '
mov byte [gs:494],'o'
mov byte [gs:496],'p'
mov byte [gs:498],'e'
mov byte [gs:500],'n'
mov byte [gs:502],'e'
mov byte [gs:504],'d'
mov byte [gs:506],'.'

; 内核初始化，并跳转到内核代码中执行
call kernel_init
mov esp,0xc009f000

; 跳转到内核代码
call 0xc0001500
; 死循环
jmp $


;--------------------------------------
; 保护模式下读磁盘
read_disk_32:
  ; 设置读写扇区的数量
  mov dx, 0x1f2
  mov al, bl
  out dx, al

  inc dx; 0x1f3
  mov al, cl; 起始扇区的前八位
  out dx, al

  inc dx; 0x1f4
  shr ecx, 8
  mov al, cl; 起始扇区的中八位
  out dx, al

  inc dx; 0x1f5
  shr ecx, 8
  mov al, cl; 起始扇区的高八位
  out dx, al

  inc dx; 0x1f6
  shr ecx, 8
  and cl, 0b1111; 将高四位置为 0

  mov al, 0b1110_0000;
  or al, cl
  out dx, al; 主盘 - LBA 模式

  inc dx; 0x1f7
  mov al, 0x20; 读硬盘
  out dx, al

  xor ecx, ecx; 将 ecx 清空
  mov cl, bl; 得到读写扇区的数量
  .read:
    push cx; 保存 cx
    call .waits; 等待数据准备完毕
    call .reads; 读取一个扇区
    pop cx; 恢复 cx
    loop .read
  ret

  .waits:
    mov dx, 0x1f7
    .check:
      in al, dx
      jmp $+2; nop 直接跳转到下一行
      jmp $+2; 一点点延迟
      jmp $+2
      and al, 0b1000_1000
      cmp al, 0b0000_1000
      jnz .check
    ret

  .reads:
    mov dx, 0x1f0
    mov cx, 256; 一个扇区 256 字
    .readw:
      in ax, dx
      jmp $+2; 一点点延迟
      jmp $+2
      jmp $+2
      mov [edi], ax
      add edi, 2
      loop .readw
    ret
;--------------------------------------

;--------------------------------------
; 页机制子函数定义
; 设置页表
setup_page:
  ; 清空页目录
  mov ecx,0x1000
  mov esi,0
  .clear_page_dir:
    mov byte [page_dir_table_pos+esi],0
    inc esi
    loop .clear_page_dir
  ; 创建页目录（两个页目录项，最后一个指向自己）
  .create_pde:
    mov eax,page_dir_table_pos
    add eax,0x1000
    mov ebx,eax
    or eax,0b111
    mov [page_dir_table_pos+0x0],eax
    mov [page_dir_table_pos+0xc00],eax
    sub eax,0x1000
    mov [page_dir_table_pos+4092],eax
  ; 创建页表项1
  mov ecx,256
  mov esi,0
  mov edx,0b111
  .create_pte:
    mov [ebx+esi*4],edx
    add edx,0x1000
    inc esi
    loop .create_pte
  ; 创建内核其他页表的PDE
  mov eax,page_dir_table_pos
  add eax,0x2000
  or eax,0b111
  mov ebx,page_dir_table_pos
  mov ecx,254
  mov esi,769
  .create_kernel_pde:
    mov [ebx+esi*4],eax
    inc esi
    add eax,0x1000
    loop .create_kernel_pde
    ret
;--------------------------------------

;--------------------------------------
; 内核初始化函数
kernel_init:
  xor eax,eax
  xor ebx,ebx
  xor edx,edx
  xor ecx,ecx
  ; 程序头表的起始地址
  mov ebx,[kernel_bin_addr+28]
  ; 程序头表在内存中的起始位置
  add ebx,kernel_bin_addr
  ; 程序头表的条目大小
  mov dx,[kernel_bin_addr+42]
  ; 程序头表的条目数量
  mov cx,[kernel_bin_addr+44]
  .copy_each_segment:
    cmp byte [ebx+0],0
    je .ptnull
    ; 合法程序表头，开始复制，为函数传参memcpy(dst,src,size)
    ; size
    push dword [ebx+16]
    ; src
    mov eax,[ebx+4]
    add eax,kernel_bin_addr
    push eax
    ; des:段的虚拟地址
    push dword [ebx+8]
    call mem_cpy
    ; 清空栈参数
    add esp,12  
    ; 转向下一个程序头表目录
    .ptnull:
      add ebx,edx
    loop .copy_each_segment
    ret

; 内存数据拷贝
mem_cpy:
  cld
  push ebp
  mov ebp,esp
  push ecx
  mov edi,[ebp+8]
  mov esi,[ebp+12]
  mov ecx,[ebp+16]
  rep movsb
  pop ecx
  pop ebp
  ret
;--------------------------------------

;--------------------------------------
; 页机制相关数据定义
page_dir_table_pos equ 0x100000

; 选择子
code_selector equ (1<<3)
data_selector equ (2<<3)
video_selector equ (3<<3)

; 定义全局描述符
gdt_ptr:
  dw (gdt_end-gdt_base-1)
  dd gdt_base
; 各全局描述符的限长和基址
base equ 0          ; 段基址
limit equ 0xfffff   ; 段大小限值
video_base equ 0xb8000
video_limit equ 0x7
; 定义3个全局描述符（第0个无效、代码段、数据段、显存段）
gdt_base:
  dd 0,0
gdt_code:
  dw limit & 0xffff
  dw base & 0xffff
  db (base>>16) & 0xff
  db 0b1110 | 0b1001_0000 
  db 0b1100_0000 | (limit >> 16)
  db (base >> 24) & 0xff
gdt_data:
  dw limit & 0xffff
  dw base & 0xffff
  db (base >> 16) & 0xff
  db 0b0010 | 0b1001_0000
  db 0b1100_0000 | (limit >> 16)
  db (base >> 24) & 0xff
gdt_video:
  dw video_limit & 0xffff
  dw video_base & 0xffff
  db (video_base >> 16) & 0xff
  db 0b0010 | 0b1001_0000
  db 0b1100_0000 | (video_limit >> 16)
  db (video_base >> 24) & 0xff
gdt_end:
;--------------------------------------

;--------------------------------------
; 用于保存内存容量数据
total_mem_bytes:
  dd 0
ards_count:
  dd 0
ards_buffer:
;--------------------------------------
