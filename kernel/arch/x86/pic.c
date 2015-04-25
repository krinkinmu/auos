#include <arch/ioport.h>
#include <arch/pic.h>

void pic_remap(unsigned irqbase)
{
	const unsigned irq_mask = pic_interrupts_mask();

	out8(PIC_MASTER_COMMAND, PIC_ICW1_EXPECT_ICW4 | PIC_ICW1_INIT);
	out8(PIC_SLAVE_COMMAND, PIC_ICW1_EXPECT_ICW4 | PIC_ICW1_INIT);
	out8(PIC_MASTER_DATA, irqbase);
	out8(PIC_SLAVE_DATA, irqbase + 8);
	out8(PIC_MASTER_DATA, BITU(PIC_SLAVE_IRQ));
	out8(PIC_SLAVE_DATA, PIC_SLAVE_IRQ);
	out8(PIC_MASTER_DATA, PIC_ICW4_8086);
	out8(PIC_SLAVE_DATA, PIC_ICW4_8086);

	pic_mask_interrupts(irq_mask);
}

void pic_mask_interrupts(unsigned mask)
{
	const unsigned new_mask = mask | pic_interrupts_mask();
	out8(PIC_MASTER_DATA, new_mask & 0xFFU);
	out8(PIC_SLAVE_DATA, new_mask >> 8);
}

void pic_unmask_interrupts(unsigned mask)
{
	const unsigned new_mask = ~mask & pic_interrupts_mask();
	out8(PIC_MASTER_DATA, new_mask & 0xFFU);
	out8(PIC_SLAVE_DATA, new_mask >> 8);
}

unsigned pic_interrupts_mask(void)
{
	return in8(PIC_MASTER_DATA) | ((unsigned)in8(PIC_SLAVE_DATA) << 8);
}

void pic_eoi(unsigned irq)
{
	if (irq >= 8)
		out8(PIC_SLAVE_COMMAND, PIC_EOI);
	out8(PIC_MASTER_COMMAND, PIC_EOI);
}
