#include "boot.h"

	/* multiboot header, required by GRUB */
	.section ".header", "a"
	.align 4
#define AUOS_HEADER_FLAGS (MB_PAGE_ALIGN | MB_MEMORY_INFO)
	.long MB_HEADER_MAGIC
	.long AUOS_HEADER_FLAGS
	.long -(MB_HEADER_MAGIC + AUOS_HEADER_FLAGS)


	.section ".bootstrap", "ax"
	.extern main
	.global _start
_start:
	/* fill .bss section with zeroes */
	xorl	%eax,         %eax
	movl	$__bss_begin, %edi
	movl	$__bss_end,   %ecx
	subl	%edi,         %ecx
	shrl	$2,           %ecx
        rep
        stosl

	movl	$stack_top,   %esp
	call	main

halt:
	hlt
	jmp	halt


	/* early boot stack is in .bss section */
	.bss
	.space BOOT_TIME_STACK_SIZE
stack_top:
