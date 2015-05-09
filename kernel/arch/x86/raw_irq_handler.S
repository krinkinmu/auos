	.text
	.extern arch_raw_irq_handler
	.align 32
common_irq_handler:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %esp

	call arch_raw_irq_handler

	popl %esp
	popl %esi
	popl %edi
	popl %ebp
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	addl $8, %esp
	iret

	.global raw_irq_handler
	.align 16
raw_irq_handler:
irq = 0
.rept 256
	.align 16 // i think 16 bytes is enough for code below
	.if (irq <> 8) && (irq <> 17) && ((irq > 14) || (irq < 10))
	pushl $0
	.endif
	pushl $irq
	jmp common_irq_handler
irq = irq + 1
.endr
