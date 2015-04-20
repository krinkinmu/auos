#include "boot.h"

	/* multiboot header, required by GRUB */
	.section ".header", "a"
	.align 4
#define AUOS_HEADER_FLAGS (MB_PAGE_ALIGN | MB_MEMORY_INFO)
	.long MB_HEADER_MAGIC
	.long AUOS_HEADER_FLAGS
	.long -(MB_HEADER_MAGIC + AUOS_HEADER_FLAGS)

	.section ".bootstrap", "ax"
	.extern start_kernel
	.extern kernel_page_tables
	.global _start
_start:
	/*
	 Protected mode is enabled by GRUB, paging is disabled.
	 Binary image is compiled to work in high address, so
	 we have translate all virtual address to physical
	 addresses manually by pa macro.
	*/

	/* load stack pointer as soon as possible */
	movl	$pa(stack_top), %esp

	/* fill .bss section with zeroes */
	xorl	%eax, %eax
	movl	$pa(__bss_begin), %edi
	movl	$pa(__bss_end), %ecx
	subl	%edi, %ecx
	shrl	$2, %ecx
        rep
        stosl

	/*
	 Fill page tables for low memory (<1Gb of physical memory)
	*/
	movl	$pa(kernel_page_tables), %edi
	movl	$PTE_INIT_ATTR, %eax
	movl	$LOWMEM_PAGE_FRAMES, %ecx
1:
	stosl
	addl	$PAGE_SIZE, %eax
	loop	1b

	/*
	 Map [0, <1Gb) and [PAGE_OFFSET, PAGE_OFFSET + <1Gb) virtual address
	 ranges to [0, <1Gb) range of physical memory.
	*/
	movl	$pa(kernel_page_tables), %esi
	movl	$pa(initial_page_dir), %edx
	movl	$KERNEL_PAGE_DIRS, %ecx
1:
	leal	PDE_INIT_ATTR(%esi), %eax
	movl	%eax, (%edx)
	movl	%eax, (LOWMEM_PAGE_DIR << 2)(%edx)
	addl	$4, %edx
	addl	$PAGE_SIZE, %esi
	loop	1b

	/*
	 Now we have two mappings for kernel physical address space,
	 but paging is not enabled yet.

	 Drop EFLAGS, enable paging, load gdt, reset pipeline and stack
	 pointer.
	*/
	pushl	$0
	popfl
	

#define CR0_STATE	(X86_CR0_PE | X86_CR0_MP | X86_CR0_ET | \
			 X86_CR0_NE | X86_CR0_WP | X86_CR0_AM | \
			 X86_CR0_PG)
	movl	$pa(initial_page_dir), %eax
	movl	%eax, %cr3
	movl	$CR0_STATE, %eax
	movl	%eax, %cr0

	lgdt	gdt_descr
	movl	$BOOT_DS, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss
	ljmp	$BOOT_CS, $1f
1:

	addl	$PAGE_OFFSET, %esp

	/*
	 we have finised initial setup and ready to start kernel,
	 ebx contains pointer to multiboot info (and we stored it alive
	 along the previous code) pass it via stack.
	*/
	push	%ebx
	call	start_kernel

halt:
	hlt
	jmp	halt

	.data
	/*
	 align gdt on cache line size (128 is max possible value, as i know)
	*/
	.align 128
gdt:
	/* first gdt entry must be null */
	.quad 0x0000000000000000
	.quad 0x00cf9a000000ffff
	.quad 0x00cf92000000ffff
gdt_descr:
	.word (gdt_descr - gdt - 1)
	.long gdt

	/* kernel stack is in .bss section */
	.bss
	.align PAGE_SIZE
	.space BOOT_TIME_STACK_SIZE
stack_top:

	.align PAGE_SIZE
	.global swapper_page_dir
swapper_page_dir:
	.fill 1024, 4, 0

	.align PAGE_SIZE
	.global initial_page_dir
initial_page_dir:
	.fill 1024, 4, 0
