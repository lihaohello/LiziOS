	.file	"main.c"
# GNU C11 (Ubuntu 7.5.0-3ubuntu1~18.04) version 7.5.0 (x86_64-linux-gnu)
#	compiled by GNU C version 7.5.0, GMP version 6.1.2, MPFR version 4.0.1, MPC version 1.1.0, isl version isl-0.19-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed:  -nostdinc -imultilib 32 -imultiarch i386-linux-gnu
# main.c -m32 -mpreferred-stack-boundary=2 -mtune=generic -march=i686
# -auxbase-strip main.s -O0 -fno-asynchronous-unwind-tables -fno-pie
# -fno-stack-protector -fno-builtin -fomit-frame-pointer -fverbose-asm
# -Wformat -Wformat-security
# options enabled:  -faggressive-loop-optimizations -fauto-inc-dec
# -fchkp-check-incomplete-type -fchkp-check-read -fchkp-check-write
# -fchkp-instrument-calls -fchkp-narrow-bounds -fchkp-optimize
# -fchkp-store-bounds -fchkp-use-static-bounds
# -fchkp-use-static-const-bounds -fchkp-use-wrappers -fcommon
# -fdelete-null-pointer-checks -fdwarf2-cfi-asm -fearly-inlining
# -feliminate-unused-debug-types -ffp-int-builtin-inexact -ffunction-cse
# -fgcse-lm -fgnu-runtime -fgnu-unique -fident -finline-atomics
# -fira-hoist-pressure -fira-share-save-slots -fira-share-spill-slots
# -fivopts -fkeep-static-consts -fleading-underscore -flifetime-dse
# -flto-odr-type-merging -fmath-errno -fmerge-debug-strings
# -fomit-frame-pointer -fpcc-struct-return -fpeephole -fplt
# -fprefetch-loop-arrays -fsched-critical-path-heuristic
# -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
# -fsched-last-insn-heuristic -fsched-rank-heuristic -fsched-spec
# -fsched-spec-insn-heuristic -fsched-stalled-insns-dep -fschedule-fusion
# -fsemantic-interposition -fshow-column -fshrink-wrap-separate
# -fsigned-zeros -fsplit-ivs-in-unroller -fssa-backprop -fstdarg-opt
# -fstrict-volatile-bitfields -fsync-libcalls -ftrapping-math -ftree-cselim
# -ftree-forwprop -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
# -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop
# -ftree-reassoc -ftree-scev-cprop -funit-at-a-time -fverbose-asm
# -fzero-initialized-in-bss -m32 -m80387 -m96bit-long-double
# -malign-stringops -mavx256-split-unaligned-load
# -mavx256-split-unaligned-store -mfancy-math-387 -mfp-ret-in-387 -mglibc
# -mieee-fp -mlong-double-80 -mno-red-zone -mno-sse4 -mpush-args -msahf
# -mtls-direct-seg-refs -mvzeroupper

	.text
	.globl	message
	.data
	.align 4
	.type	message, @object
	.size	message, 6
message:
	.string	"hello"
	.text
	.globl	main
	.type	main, @function
main:
	subl	$8, %esp	#,
# main.c:4:     char* video = (char*)0xb8000;
	movl	$753664, 4(%esp)	#, video
# main.c:6:     for (int i = 0; i < sizeof(message); i++) {
	movl	$0, (%esp)	#, i
	jmp	.L2	#
.L3:
# main.c:7:         *video = message[i];
	movl	(%esp), %eax	# i, tmp93
	addl	$message, %eax	#, tmp92
	movzbl	(%eax), %eax	# message, _1
	movl	%eax, %edx	# _1, _2
	movl	4(%esp), %eax	# video, tmp94
	movb	%dl, (%eax)	# _2, *video_4
# main.c:8:         video++;
	addl	$1, 4(%esp)	#, video
# main.c:9:         *video = 0b00100101;
	movl	4(%esp), %eax	# video, tmp95
	movb	$37, (%eax)	#, *video_12
# main.c:10:         video++;
	addl	$1, 4(%esp)	#, video
# main.c:6:     for (int i = 0; i < sizeof(message); i++) {
	addl	$1, (%esp)	#, i
.L2:
# main.c:6:     for (int i = 0; i < sizeof(message); i++) {
	movl	(%esp), %eax	# i, i.0_3
	cmpl	$5, %eax	#, i.0_3
	jbe	.L3	#,
# main.c:13:     return 0;
	movl	$0, %eax	#, _10
# main.c:14: }
	addl	$8, %esp	#,
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
