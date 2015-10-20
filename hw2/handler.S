	.text
	.global interrupt_handler
	.extern c_interrupt_handler
interrupt_handler:
	pushal
	cld
	call c_interrupt_handler
	popal
	iret
