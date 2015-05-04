#include <kernel/debug.h>
#include <kernel/irq.h>

static const struct irqchip *irqchip;

#define irqchip_must_have(fun)                                      \
do                                                                  \
{                                                                   \
	assert(irqchip && irqchip->fun,                             \
		"IRQ chip isn't initialized or has no " #fun "\n"); \
}                                                                   \
while (0)

void irqchip_register(const struct irqchip *chip)
{
	irqchip = chip;
}

void irqchip_remap(unsigned offset)
{
	irqchip_must_have(remap);
	irqchip->remap(offset);
}

void irqchip_setup(unsigned irq, unsigned long flags)
{
	irqchip_must_have(setup);
	irqchip->setup(irq, flags);
}

void irqchip_mask(unsigned irq)
{
	irqchip_must_have(mask);
	irqchip->mask(irq);
}

void irqchip_unmask(unsigned irq)
{
	irqchip_must_have(unmask);
	irqchip->unmask(irq);
}

void irqchip_eoi(unsigned irq)
{
	irqchip_must_have(eoi);
	irqchip->eoi(irq);
}
