	.file	"TinyHelloWorld.c"
	.text
	.p2align 4,,15
	.globl	print
	.type	print, @function
print:
.LFB0:
	.cfi_startproc
#APP
# 4 "TinyHelloWorld.c" 1
	movl $13,%edx;         movl str,%ecx;         movl $0,%ebx;         movl $4,%eax;         int $0x80;
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE0:
	.size	print, .-print
	.p2align 4,,15
	.globl	exit
	.type	exit, @function
exit:
.LFB1:
	.cfi_startproc
#APP
# 12 "TinyHelloWorld.c" 1
	movl $42,%ebx;         movl $1,%eax;         int $0x80;
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE1:
	.size	exit, .-exit
	.p2align 4,,15
	.globl	nomain
	.type	nomain, @function
nomain:
.LFB2:
	.cfi_startproc
#APP
# 4 "TinyHelloWorld.c" 1
	movl $13,%edx;         movl str,%ecx;         movl $0,%ebx;         movl $4,%eax;         int $0x80;
# 0 "" 2
# 12 "TinyHelloWorld.c" 1
	movl $42,%ebx;         movl $1,%eax;         int $0x80;
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE2:
	.size	nomain, .-nomain
	.globl	str
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Hello World!\n"
	.section	.data.rel.local,"aw",@progbits
	.align 4
	.type	str, @object
	.size	str, 4
str:
	.long	.LC0
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
