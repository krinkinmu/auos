#define MB_HEADER_MAGIC 0x1BADB002
#define MB_HEADER_FLAGS 0x00000000
#define MB_HEADER_CKSUM -(MB_HEADER_MAGIC + MB_HEADER_FLAGS)

	.section .header, "a"
	.global start
	.extern main

	.align 4
	.long MB_HEADER_MAGIC
	.long MB_HEADER_FLAGS
	.long MB_HEADER_CKSUM

start:
	movl $stack_top, %esp

	xorl %eax, %eax
	movl __bss_begin, %edi
	movl __bss_end, %ecx
	subl %edi, %ecx
	shrl $2, %ecx
	rep
	stosl

	pushl $0
	popf

	call main

	.bss
	.space 0x1000
stack_top:
