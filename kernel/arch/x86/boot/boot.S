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
	.global _start
_start:
	/* first of all load our gdt */
	lgdt	pa(boot_gdt_descr)
	movl	$(BOOT_DS), %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss

	ljmp	$(BOOT_CS), $1f
1:
	/*
	 Now we use custom gdt that covers all address space.
	 Protected mode is enabled by GRUB, paging is disabled.
	 Binary image is compiled to work in high address, so
	 we have translate all virtual address to physical
	 addresses manually by pa macro.
	*/

	/* fill .bss section with zeroes */
	xorl	%eax, %eax
	movl	$pa(__bss_begin), %edi
	movl	$pa(__bss_end), %ecx
	subl	%edi, %ecx
	shrl	$2, %ecx
        rep
        stosl

	/* load stack pointer */
	movl	$pa(stack_top), %esp

	/*
	 Map map first severeal MB of physical memory to virtual
	 address space from 0 address (identity mapping) and to
	 vritual address space from PAGE_OFFSET address (kernel
	 mapping).

	 Invariant:
	 %edi - pointer to page table entry (byte offset of current pte entry)
	 %edx - pointer to page dir entry (byte offset of current pde entry)
	 %eax - current pysical address + initial pte entry attributes

	 Initialization contains 3 steps:
	  - initial page tables are stored starting from __heap_end, so
	    initialize to __heap_end physical address;
	  - store initial_page_dir physical address to %edx;
	  - store initial pte entry attribute to %eax.

	 Than run two nested loops, outer loop iterates over pde entries,
         inner loop iterates over pte entries.

	 PDE_OFFSET is offset of PAGE_OFFSET address pde entry in bytes.
	*/
PDE_OFFSET = (PAGE_OFFSET >> 20);

	movl	$pa(__heap_begin), %edi
	movl	$pa(initial_page_dir), %edx
	movl	$PTE_INIT_ATTR, %eax

outer_loop:
	leal	PDE_INIT_ATTR(%edi), %ecx
	movl	%ecx, (%edx)
	movl	%ecx, PDE_OFFSET(%edx)
	addl	$4, %edx
	movl	$1024, %ecx

inner_loop:
	stosl
	addl	$PAGE_SIZE, %eax
	loop	inner_loop

	movl	$pa(__kernel_end) + PTE_INIT_ATTR, %ebp
	cmpl	%ebp, %eax
	jb	outer_loop

	movl	%edi, pa(__heap_end)

	/*
	 Now we have two mappings for kernel physical address space,
	 but paging is not enabled yet.

	 Drop EFLAGS, enable paging, reload gdt, reset pipeline and fix
	 addresses.
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

	/* load new gdt descriptor with fixed address */
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

boot_gdt_descr:
	.word (boot_gdt_descr - gdt - 1)
	.long pa(gdt)

gdt_descr:
	.word (boot_gdt_descr - gdt - 1)
	.long gdt

	/* early boot stack is in .bss section */
	.bss
	.align PAGE_SIZE
	.space BOOT_TIME_STACK_SIZE
stack_top:

	.align PAGE_SIZE
	.global initial_page_dir
initial_page_dir:
	.fill 1024, 4, 0

	.align PAGE_SIZE
	.global swapper_page_dir
swapper_page_dir:
	.fill 1024, 4, 0

