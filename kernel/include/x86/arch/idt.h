#ifndef __X86_ARCH_IDT_H__
#define __X86_ARCH_IDT_H__

#include <kernel/kernel.h>

#include <stddef.h>
#include <stdint.h>

#define IDT_PRESENT    BITU(7)
#define IDT_USER    (BITU(5) | BITU(6))
#define IDT_KERNEL  0
#define IDT_INTGATE32  (BITU(3) | BITU(2) | BITU(1))
#define IDT_TRAPGATE32 (BITU(3) | BITU(2) | BITU(1) | BITU(0))

#define INTERRUPT_GATE (IDT_PRESENT | IDT_INTGATE32)
#define TRAP_GATE      (IDT_PRESENT | IDT_TRAPGATE32)

#define IDT_SIZE 256

struct idt_entry {
	uint64_t entry;
} __attribute__((packed));

struct idt_ptr {
	uint16_t size;
	uint32_t base;
} __attribute__((packed));

void init_isr_gate(struct idt_entry *entry, void (*isr)(void), unsigned dpl);
void init_trap_gate(struct idt_entry *entry, void (*sc)(void), unsigned dpl);
void init_idt_ptr(struct idt_ptr *ptr, struct idt_entry *table, size_t size);
void set_idt(const struct idt_ptr *ptr);

#endif /*__X86_ARCH_IDT_H__*/
