	.code16gcc
	.text
	.global start
start:
	ljmp $0, $real_start

	.align 4
gdt:
	.quad 0x0000000000000000
	.quad 0x00cf9a000000ffff
	.quad 0x00cf92000000ffff
gdt_ptr:
	.word (gdt_ptr - gdt - 1)
	.long gdt

real_start:
	cli
	lgdt gdt_ptr
	movl %cr0, %eax
	orl $1, %eax
	movl %eax, %cr0
	jmp 1f
1:

	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss

	ljmp $0x8, $start32

	.code32
	.extern main
start32:
	movl $0x7c00, %esp
	call main
	hlt
