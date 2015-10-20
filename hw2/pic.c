#include "defs.h"
#include "ioport.h"
#include "pic.h"

static const unsigned short PIC_MASTER_CMD_PORT = 0x20;
static const unsigned short PIC_MASTER_DATA_PORT = 0x21;
static const unsigned short PIC_SLAVE_CMD_PORT = 0xA0;
static const unsigned short PIC_SLAVE_DATA_PORT = 0xA1;

static const unsigned PIC_SLAVE_IRQ = 0x02;
static const unsigned PIC_IRQS = 8;

static unsigned pic_irq_mask = 0xFFFFu;

static void pic_remap(unsigned offset)
{
	out8(PIC_MASTER_CMD_PORT, BITU(0) | BITU(4));
	out8(PIC_SLAVE_CMD_PORT, BITU(0) | BITU(4));

	out8(PIC_MASTER_DATA_PORT, offset);
	out8(PIC_SLAVE_DATA_PORT, offset + PIC_IRQS);

	out8(PIC_MASTER_DATA_PORT, BITU(PIC_SLAVE_IRQ));
	out8(PIC_SLAVE_DATA_PORT, PIC_SLAVE_IRQ);

	out8(PIC_MASTER_DATA_PORT, BITU(0));
	out8(PIC_SLAVE_DATA_PORT, BITU(0));

	out8(PIC_MASTER_DATA_PORT, pic_irq_mask & 0xFFu);
	out8(PIC_SLAVE_DATA_PORT, (pic_irq_mask >> 8) & 0xFFu);
}

void pic_setup(void)
{ pic_remap(PIC_OFFSET); }

void pic_mask(int irq)
{
	const unsigned mask = BITU(irq);

	if (mask & pic_irq_mask)
		return;

	pic_irq_mask |= BITU(irq);
	if (irq < PIC_IRQS)
		out8(PIC_MASTER_DATA_PORT, pic_irq_mask & 0xFFu);
	else
		out8(PIC_SLAVE_DATA_PORT, (pic_irq_mask >> PIC_IRQS) & 0xFFu);
}

void pic_unmask(int irq)
{
	const unsigned mask = BITU(irq);

	if (!(mask & pic_irq_mask))
		return;

	pic_irq_mask &= ~mask;
	if (irq < PIC_IRQS)
		out8(PIC_MASTER_DATA_PORT, pic_irq_mask & 0xFFu);
	else
		out8(PIC_SLAVE_DATA_PORT, (pic_irq_mask >> PIC_IRQS) & 0xFFu);
}

void pic_eoi(int irq)
{
	static const unsigned PIC_EOI_CMD = 0x60;

	if (irq < PIC_IRQS) {
		out8(PIC_MASTER_CMD_PORT, PIC_EOI_CMD + irq);
	} else {
		out8(PIC_SLAVE_CMD_PORT, PIC_EOI_CMD + (irq & (PIC_IRQS - 1)));
		out8(PIC_MASTER_CMD_PORT, PIC_EOI_CMD + PIC_SLAVE_IRQ);
	}
}
