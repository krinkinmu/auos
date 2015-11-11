#include "defs.h"
#include "gdt.h"
#include "idt.h"

/***********************************************************
 *        15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0  *
 *       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ *
 * 15-0  |                 OFFSET [15:0]                 | *
 *       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ *
 * 31-16 |                SELECTOR [15:0]                | *
 *       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ *
 * 47-32 |PR|Privl| S|   TYPE    | 0| 0| 0| 0| 0| 0| 0| 0| *
 *       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ *
 * 63-48 |                OFFSET [16:31]                 | *
 *       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ *
 ***********************************************************/

#define IDT_PRESENT BITUL(15)
#define IDT_32INT   (0xEul << 8)
#define IDT_32TRAP  (0xFul << 8)
#define INT_HIGH    0x30

typedef void (*raw_isr_t)(void);

struct idt_entry {
	unsigned long low;
	unsigned long high;
} __attribute__((packed));

struct idt_ptr {
	unsigned short size;
	unsigned long base;
} __attribute__((packed));

static void setup_idt_entry(struct idt_entry *entry, unsigned short selector,
			unsigned long isr, unsigned long flags)
{
	entry->low = (isr & 0xFFFFul) | ((unsigned long)selector << 16);
	entry->high = flags | (isr & 0xFFFF0000ul);
}

static void set_idt(const struct idt_ptr *ptr)
{ __asm__ volatile ("lidt (%0)" : : "a"(ptr)); }

static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idt_ptr;
static isr_t handlers[IDT_SIZE];
extern raw_isr_t raw_isrs_table[];

static void setup_irq(raw_isr_t isr, unsigned no)
{
	setup_idt_entry(idt + no, GDT_CODE_SELECTOR, (unsigned long)isr,
			IDT_32INT | IDT_PRESENT);
}

static void setup_trap(raw_isr_t isr, unsigned no)
{
	setup_idt_entry(idt + no, GDT_CODE_SELECTOR, (unsigned long)isr,
			IDT_32TRAP | IDT_PRESENT);
}

void set_interrupt_handler(unsigned long intno, isr_t isr)
{ handlers[intno] = isr; }

struct trap_frame {
	unsigned long edi;
	unsigned long esi;
	unsigned long ebp;
	unsigned long esp;
	unsigned long ebx;
	unsigned long edx;
	unsigned long ecx;
	unsigned long eax;
	unsigned long intno;
	unsigned long errno;
};

void raw_isr(struct trap_frame *frame)
{
	const unsigned long intno = frame->intno;
	const unsigned long errno = frame->errno;

	isr_t isr = handlers[intno];
	if (isr)
		isr(intno, errno);
}

void idt_setup(void)
{
	unsigned i;

	for (i = 0; i != INT_HIGH; ++i)
		setup_irq(raw_isrs_table[i], i);

	for (i = INT_HIGH; i != IDT_SIZE; ++i)
		setup_trap(raw_isrs_table[i], i);

	idt_ptr.size = sizeof(idt) - 1;
	idt_ptr.base = (unsigned long)idt;
	set_idt(&idt_ptr);
}
