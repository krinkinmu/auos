	.text
	.extern irq_process
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

	call irq_process

	// ( 7 GPR + 1 stack pointer + IRQ + error code ) times 4 = 0x28
	addl $0x28, %esp
	iret

	.global raw_irq_handler
	.align 16
raw_irq_handler:
irq = 0
.rept 256
	.align 16 // i think 16 bytes is enough for code below
	.if irq <> 8 && irq <> 17 && (irq > 9 || irq < 15)
	pushl $0
	.endif
	pushl $irq
	jmp common_irq_handler
irq = irq + 1
.endr
