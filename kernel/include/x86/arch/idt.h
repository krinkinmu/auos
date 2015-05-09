#ifndef __X86_ARCH_IDT_H__
#define __X86_ARCH_IDT_H__

#include <kernel/kernel.h>

#include <stddef.h>
#include <stdint.h>

#define IDT_PRESENT    BITU(15)
#define IDT_USER       (BITU(13) | BITU(14))
#define IDT_KERNEL     0
#define IDT_GATE32     BITU(11)
#define IDT_INT_GATE   (BITU(9) | BITU(10))
#define IDT_TRAP_GATE  (BITU(8) | BITU(9) | BITU(10))

#define INT_GATE       (IDT_PRESENT | IDT_INT_GATE | IDT_GATE32)
#define TRAP_GATE      (IDT_PRESENT | IDT_TRAP_GATE | IDT_GATE32)

#define IDT_SIZE 256

struct idt_entry {
	uint64_t entry;
} __attribute__((packed));

struct idt_ptr {
	uint16_t size;
	uint32_t base;
} __attribute__((packed));

void init_isr_gate(struct idt_entry *entry, unsigned long isr, unsigned dpl);
void init_trap_gate(struct idt_entry *entry, unsigned long sc, unsigned dpl);
void init_idt_ptr(struct idt_ptr *ptr, struct idt_entry *table, size_t size);
void set_idt(const struct idt_ptr *ptr);

#endif /*__X86_ARCH_IDT_H__*/
