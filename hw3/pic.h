#ifndef __X86_PIC_H__
#define __X86_PIC_H__

#define PIC_OFFSET 0x20

void pic_setup(void);
void pic_mask(int irq);
void pic_unmask(int irq);
void pic_eoi(int irq);

#endif /*__X86_PIC_H__*/
