#include "scheduler.h"
#include "idt.h"
#include "irq.h"
#include "pic.h"

#define IRQS     0x10
#define IRQ_LOW  PIC_OFFSET
#define IRQ_HIGH (IRQ_LOW + IRQS)

static handler_t handlers[IRQS];

static void irq_raw_handler(unsigned long intno, unsigned long errno)
{
	const int irq = intno - IRQ_LOW;
	handler_t handler = handlers[irq];

	(void) errno;

	if (handler)
		handler(irq);
	pic_eoi(irq);

	if (current->remain <= 0)
		schedule();
}

void setup_irq_handler(int irq, handler_t handler)
{ handlers[irq] = handler; }

void irq_mask(int irq)
{ pic_mask(irq); }

void irq_unmask(int irq)
{ pic_unmask(irq); }

void irq_setup(void)
{
	unsigned i;

	for (i = IRQ_LOW; i != IRQ_HIGH; ++i)
		set_interrupt_handler(i, irq_raw_handler);
}
