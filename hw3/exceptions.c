#include "exceptions.h"
#include "stdio.h"

#define EXCEPTIONS 0x20

static const char *error[] = {
	"Divide-by-zero",
	"Debug",
	"NMI",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Unexpected",
	"x87 Floating-Point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Unexpected",
	"Security Exception",
	"Unexpected"
};

static void general_error_handler(unsigned long intno, unsigned long errno)
{
	myputs(error[intno]);
	while(1);
}

void exceptions_setup(void)
{
	unsigned i;

	for (i = 0; i != EXCEPTIONS; ++i)
		set_interrupt_handler(i, general_error_handler);
}
