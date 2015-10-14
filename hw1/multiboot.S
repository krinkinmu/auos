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
	pushl $0
	popf
	call main

	.bss
	.space 0x100
stack_top:
