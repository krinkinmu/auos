#include <arch/idt.h>
#include <arch/gdt.h>

static void init_gate(struct idt_entry *entry, unsigned long offset,
			unsigned long selector, unsigned long type)
{
	const unsigned long low = (offset & 0xFFFFUL) | (selector << 16);
	const unsigned long high = (offset & 0xFFFF0000UL) | type;

	entry->entry = ((uint64_t)high << 32) | low;
}

void init_isr_gate(struct idt_entry *entry, unsigned long isr, unsigned dpl)
{
	init_gate(entry, isr, KERNEL_CODE_SELECTOR, INT_GATE | dpl);
}

void init_trap_gate(struct idt_entry *entry, unsigned long sc, unsigned dpl)
{
	init_gate(entry, sc, KERNEL_CODE_SELECTOR, TRAP_GATE | dpl);
}

void init_idt_ptr(struct idt_ptr *ptr, struct idt_entry *table, size_t size)
{
	ptr->size = (uint16_t)size * sizeof(*table);
	ptr->base = (uint32_t)table;
}

void set_idt(const struct idt_ptr *ptr)
{
	__asm__ ("lidt (%0)" : : "a"(ptr));
}
