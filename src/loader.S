section loader vstart=LOADER_BASE_ADDR

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
    jc error
    add di,cx
    inc dword [ards_count]
    cmp ebx,0
    jnz .next

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

; 查看内存容量
mov eax,[total_mem_bytes]
xchg bx,bx

jmp loader_start 

loadermsg db '2 loader in real',10, 13,0
%include "./libs/print.inc"

; 开始切换到保护模式
loader_start:
; 打印信息
mov si,loadermsg
call print
; 打开A20，关闭地址回绕机制
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

; 保护模式代码
[bits 32]
p_mode_start:
  mov ax,data_selector
  mov ds,ax
  mov es,ax
  mov ss,ax
  mov fs,ax
  mov esp,LOADER_BASE_ADDR
  mov ax,video_selector
  mov gs,ax
  mov byte [gs:160],'L'
  mov byte [gs:161],0xA4
  mov byte [gs:162],'i'
  mov byte [gs:163],0xA4
  mov byte [gs:164],'z'
  mov byte [gs:165],0xA4
  mov byte [gs:166],'i'
  mov byte [gs:167],0xA4
  mov byte [gs:168],'O'
  mov byte [gs:169],0xA4
  mov byte [gs:170],'S'
  mov byte [gs:171],0xA4

  ; 开启分页机制
  call setup_page
  sgdt [gdt_ptr]
  mov ebx,[gdt_ptr+2]
  or dword [ebx+0x18+4],0xc0000000
  add dword [gdt_ptr+2],0xc0000000
  add esp,0xc0000000
  mov eax,PAGE_DIR_TABLE_POS
  mov cr3,eax
  mov eax,cr0
  or eax,0x80000000
  mov cr0,eax
  xchg bx,bx
  lgdt [gdt_ptr]
  mov byte [gs:320],'P'
  mov byte [gs:322],'A'
  mov byte [gs:324],'G'
  mov byte [gs:326],'E'

; 循环执行
jmp $

; 启动页表机制
setup_page:
  ; 清空页目录
  mov ecx,0x1000
  mov esi,0
  .clear_page_dir:
    mov byte [PAGE_DIR_TABLE_POS+esi],0
    inc esi
    loop .clear_page_dir
  ; 创建页目录（两个页目录项，最后一个指向自己）
  .create_pde:
    mov eax,PAGE_DIR_TABLE_POS
    add eax,0x1000
    mov ebx,eax
    or eax,0b111
    mov [PAGE_DIR_TABLE_POS+0x0],eax
    mov [PAGE_DIR_TABLE_POS+0xc00],eax
    sub eax,0x1000
    mov [PAGE_DIR_TABLE_POS+4092],eax
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
  mov eax,PAGE_DIR_TABLE_POS
  add eax,0x2000
  or eax,0b111
  mov ebx,PAGE_DIR_TABLE_POS
  mov ecx,254
  mov esi,769
  .create_kernel_pde:
    mov [ebx+esi*4],eax
    inc esi
    add eax,0x1000
    loop .create_kernel_pde
    ret


error:
  mov si, .msg
  call print
  hlt
  jmp $
  .msg db "Loading Error!!!", 10, 13, 0


; 相关定义
LOADER_BASE_ADDR equ 0x900
PAGE_DIR_TABLE_POS equ 0x100000

; 选择子
code_selector equ (1<<3)
data_selector equ (2<<3)
video_selector equ (3<<3)

; 定义GDT表的长度和基址
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

; 用于保存内存容量数据
total_mem_bytes:
  dd 0
ards_count:
  dd 0
ards_buffer:
