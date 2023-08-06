	.file	"test.c"
	.text


	.globl	sub
	.type	sub, @function
sub:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	movl	8(%ebp), %eax
	subl	12(%ebp), %eax
	movl	%eax, -4(%ebp)
	movl	-4(%ebp), %eax
	leave
	ret
	.size	sub, .-sub

	
	.section	.rodata
.LC0:
	.string	"%d + %d = %d\n"
	.text
	.globl	main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$12, %esp
	movl	$5, -4(%ebp)
	movl	$10, -8(%ebp)
	pushl	-8(%ebp)
	pushl	-4(%ebp)
	call	add
	addl	$8, %esp
	movl	%eax, -12(%ebp)
	pushl	-12(%ebp)
	pushl	-8(%ebp)
	pushl	-4(%ebp)
	pushl	$.LC0
	call	printf
	addl	$16, %esp
	movl	$0, %eax
	leave
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
