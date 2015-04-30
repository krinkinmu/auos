#ifndef __X86_ARCH_APIC_H__
#define __X86_ARCH_APIC_H__

#include <kernel/kmap.h>

#define IO_APIC_DEFAULT_PADDR      0xFEC00000UL
#define IO_APIC_DEFAULT_PADDR      0xFEC00000UL
#define IO_APIC_PAGE_FLAGS         (PTE_KERNEL | PTE_READ | PTE_WRITE | \
					PTE_NOCACHE)
#define IO_APIC_IDR                0x00U
#define IO_APIC_VR                 0x01U
#define IO_APIC_REDTBLR            0x10U
#define IO_APIC_IRQ_POLARITY_HIGH  0x0UL
#define IO_APIC_IRQ_POLARITY_LOW   BITUL(12)
#define IO_APIC_IRQ_TRIGGER_EDGE   0x0UL
#define IO_APIC_IRQ_TRIGGER_LEVEL  BITUL(15)
#define IO_APIC_IRQ_DEST_LOGICAL   BITUL(11)
#define IO_APIC_IRQ_LOW_PRIORITY   BITUL(8)
#define IO_APIC_IRQ_MASKED         BITUL(16)
#define IO_APIC_DEST_ALL           0xFFU
#define IO_APIC_MAX_COUNT          0x10U

#define LOCAL_APIC_IDR             0x08U
#define LOCAL_APIC_LDR             0x34U
#define LOCAL_APIC_DFR             0x38U
#define LOCAL_APIC_SIVR            0x3CU
#define LOCAL_APIC_MAX_COUNT       0x08U
#define LOCAL_APIC_PAGE_FLAGS      IO_APIC_PAGE_FLAGS

#define ISA_POLARITY_MASK          0x03U
#define ISA_POLARITY_DEFAULT       0x00U
#define ISA_POLARITY_HIGH          0x01U
#define ISA_POLARITY_LOW           0x03U
#define ISA_TRIGGER_MASK           0x0CU
#define ISA_TRIGGER_DEFAULT        0x00U
#define ISA_TRIGGER_EDGE           0x04U
#define ISA_TRIGGER_LEVEL          0x0CU
#define ISA_IRQ_MAX_COUNT          0x10U

struct io_apic {
	void *vaddr;
	unsigned gsi_begin;
	unsigned gsi_end;
	unsigned id;
};

struct isa_irq_map {
	unsigned gsi;
	unsigned long flags;
};

void setup_apic(void);
void setup_local_apic(void);

#endif /*__X86_ARCH_APIC_H__*/
