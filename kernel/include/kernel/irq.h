#ifndef __KERNEL_IRQ_H__
#define __KERNEL_IRQ_H__

enum hwirq_flags {
	HWIRQ_RISING = 1,
	HWIRQ_FALLING = 2,
	HWIRQ_HIGH = 4,
	HWIRQ_LOW = 8,
	HWIRQ_DISABLED = 16
};

struct irqchip {
	void (*remap)(unsigned);
	void (*setup)(unsigned, unsigned long);
	void (*mask)(unsigned);
	void (*unmask)(unsigned);
	void (*eoi)(unsigned);
};

void irqchip_register(const struct irqchip *chip);

void irqchip_remap(unsigned offset);
void irqchip_setup(unsigned irq, unsigned long flags);
void irqchip_mask(unsigned irq);
void irqchip_unmask(unsigned irq);
void irqchip_eoi(unsigned irq);

#endif /*__KERNLE_IRQ_H__*/
