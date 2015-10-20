#ifndef __X86_IDT_H__
#define __X86_IDT_H__

#define IDT_SIZE 256

void setup_interrupt(void (*isr)(void), int no);
void idt_setup(void);

#endif /*__X86_IDT_H__*/
