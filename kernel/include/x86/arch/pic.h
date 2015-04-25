#ifndef __X86_ARCH_PIC_H__
#define __X86_ARCH_PIC_H__

#include <kernel/kernel.h>

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA    0x21
#define PIC_SLAVE_COMMAND  0xA0
#define PIC_SLAVE_DATA     0xA1

#define PIC_SLAVE_IRQ      0x02
#define PIC_EOI            0x20

#define PIC_ICW1_EXPECT_ICW4    BITU(0)
#define PIC_ICW1_INIT           BITU(4)
#define PIC_ICW4_8086           BITU(0)

void pic_mask_interrupts(unsigned mask);
void pic_unmask_interrupts(unsigned mask);
unsigned pic_interrupts_mask(void);
void pic_eoi(unsigned irq);
void pic_remap(unsigned irqbase);

#endif /*__X86_ARCH_PIC_H__*/
