#include <arch/idt.h>
#include <arch/gdt.h>

static void init_gate(struct idt_entry *entry, uint32_t offset,
			uint16_t selector, unsigned type)
{
	const uint64_t descr = (((uint64_t)offset & 0x0000FFFFUL) |
				(((uint64_t)offset & 0xFFFF0000UL) << 32) |
				((uint64_t)selector << 16) |
				((uint64_t)type << 40));
	entry->entry = descr;
}

void init_isr_gate(struct idt_entry *entry, void (*isr)(void), unsigned dpl)
{
	init_gate(entry, (uint32_t)isr, KERNEL_CODE_SELECTOR,
			INTERRUPT_GATE | dpl);
}

void init_trap_gate(struct idt_entry *entry, void (*sc)(void), unsigned dpl)
{
	init_gate(entry, (uint32_t)sc, KERNEL_CODE_SELECTOR,
			TRAP_GATE | dpl);
}

void init_idt_ptr(struct idt_ptr *ptr, struct idt_entry *table, size_t size)
{
	ptr->size = (uint16_t)size;
	ptr->base = (uint32_t)table;
}

void set_idt(const struct idt_ptr *ptr)
{
	__asm__ ("lidt (%0)" : : "a"(ptr));
}
