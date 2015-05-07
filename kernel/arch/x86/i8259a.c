#include <kernel/kernel.h>
#include <kernel/debug.h>
#include <kernel/irq.h>
#include <arch/ioport.h>

#define PIC_MASTER_CMD_PORT  0x20
#define PIC_MASTER_DATA_PORT 0x21
#define PIC_SLAVE_CMD_PORT   0xA0
#define PIC_SLAVE_DATA_PORT  0xA1

#define PIC_SLAVE_IRQ        0x02
#define PIC_SLAVE_IRQ_BIT    BITU(PIC_SLAVE_IRQ)
#define PIC_IRQS_PER_CHIP    8
#define PIC_EOI_CMD          0x60
#define PIC_ISR              0x0B

#define PIC_ICW1_EXPECT_ICW4 BITU(0)
#define PIC_ICW1_INIT        BITU(4)
#define PIC_ICW4_8086        BITU(0)

static unsigned pic_irq_mask = 0xFFFFU;

static void pic_remap(unsigned offset)
{
	out8(PIC_MASTER_CMD_PORT, PIC_ICW1_EXPECT_ICW4 | PIC_ICW1_INIT);
	out8(PIC_SLAVE_CMD_PORT, PIC_ICW1_EXPECT_ICW4 | PIC_ICW1_INIT);
	out8(PIC_MASTER_DATA_PORT, offset);
	out8(PIC_SLAVE_DATA_PORT, offset + PIC_IRQS_PER_CHIP);
	out8(PIC_MASTER_DATA_PORT, PIC_SLAVE_IRQ_BIT);
	out8(PIC_SLAVE_DATA_PORT, PIC_SLAVE_IRQ);
	out8(PIC_MASTER_DATA_PORT, PIC_ICW4_8086);
	out8(PIC_SLAVE_DATA_PORT, PIC_ICW4_8086);

	out8(PIC_MASTER_DATA_PORT, pic_irq_mask & 0xFFU);
	out8(PIC_SLAVE_DATA_PORT, (pic_irq_mask >> 8) & 0xFFU);
}

static void pic_mask(unsigned irq)
{
	pic_irq_mask |= BITU(irq);
	if (irq < 8)
		out8(PIC_MASTER_DATA_PORT, pic_irq_mask & 0xFFU);
	else
		out8(PIC_SLAVE_DATA_PORT, (pic_irq_mask >> 8) & 0xFFU);
}

static void pic_unmask(unsigned irq)
{
	pic_irq_mask &= ~BITU(irq);
	if (irq < 8)
		out8(PIC_MASTER_DATA_PORT, pic_irq_mask & 0xFFU);
	else
		out8(PIC_SLAVE_DATA_PORT, (pic_irq_mask >> 8) & 0xFFU);
}

static void pic_setup(unsigned irq, unsigned long flags)
{
	if ((flags & HWIRQ_HIGH) || (flags & HWIRQ_LOW))
		debug("Level triggered irqs aren't supported\n");

	if (flags & HWIRQ_DISABLED)
		pic_mask(irq);
	else
		pic_unmask(irq);
}

/*
static int pic_real(unsigned irq)
{
	if (irq < 8) {
		out8(PIC_MASTER_CMD_PORT, PIC_ISR);
		return in8(PIC_MASTER_CMD_PORT) & BITU(irq);
	}
	out8(PIC_SLAVE_CMD_PORT, PIC_ISR);
	return in8(PIC_SLAVE_CMD_PORT) & (BITU(irq) >> PIC_IRQS_PER_CHIP);
}
*/

static void pic_eoi(unsigned irq)
{
	if (irq >= 8) {
		out8(PIC_SLAVE_CMD_PORT, PIC_EOI_CMD + (irq & 7));
		out8(PIC_MASTER_CMD_PORT, PIC_EOI_CMD + PIC_SLAVE_IRQ);
	} else {
		out8(PIC_MASTER_CMD_PORT, PIC_EOI_CMD + irq);
	}
}

static const struct irqchip i8259a = {
	.setup = &pic_setup,
	.mask = &pic_mask,
	.unmask = &pic_unmask,
	.eoi = &pic_eoi
};

void setup_i8259a(void)
{
	pic_remap(HW_INTERRUPT_OFFSET);
	irqchip_register(&i8259a);
}
