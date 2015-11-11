#ifndef __X86_IDT_H__
#define __X86_IDT_H__

#include "defs.h"

#define IDT_SIZE  256
#define EFLAGS_IF BITUL(9)

static inline disable_interrupts(void)
{ __asm__ volatile("cli"); }

static inline enable_interrupts(void)
{ __asm__ volatile("sti"); }

static inline unsigned long irqsave(void)
{
	unsigned long flags;

	__asm__ volatile("pushf; pop %0" : "=a"(flags));
	disable_interrupts();
	return flags;
}

static inline void irqrestore(unsigned long flags)
{
	if (flags & EFLAGS_IF)
		enable_interrupts();
}

typedef void (*isr_t)(unsigned long, unsigned long);

void set_interrupt_handler(unsigned long intno, isr_t isr);
void idt_setup(void);

#endif /*__X86_IDT_H__*/
