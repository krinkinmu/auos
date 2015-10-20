#include "handler.h"
#include "idt.h"
#include "gdt.h"
#include "pic.h"
#include "pit.h"
#include "stdio.h"

static const int CLOCK_IRQ;
static const unsigned long RATE = 10ul; // ms

inline static void enable_interrupts(void)
{ __asm__ volatile ("sti"); }

void c_interrupt_handler(void)
{
	static unsigned long tick;

	if (!tick)
		myputs("tick\n");
	tick = (tick + 1) % 100;
	pic_eoi(CLOCK_IRQ);
}

void main(void)
{
	clrscr();
	gdt_setup();
	idt_setup();
	pic_setup();
	enable_interrupts();
	setup_interrupt(&interrupt_handler, PIC_OFFSET + CLOCK_IRQ);
	pit_set_channel0_rate(10ul);
	pic_unmask(CLOCK_IRQ);

	while (1);
}
