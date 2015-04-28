#ifndef __X86_ARCH_APIC_H__
#define __X86_ARCH_APIC_H__

#include <kernel/kmap.h>

#define IO_APIC_DEFAULT_PADDR 0xFEC00000UL
#define IO_APIC_DEFAULT_PADDR 0xFEC00000UL
#define IO_APIC_PAGE_FLAGS    (PTE_KERNEL | PTE_READ | PTE_WRITE | PTE_NOCACHE)
#define LOCAL_APIC_PAGE_FLAGS IO_APIC_PAGE_FLAGS
#define IO_APIC_ID_REG        0x00U
#define IO_APIC_VERSION_REG   0x01U
#define IO_APIC_REDTBL_REG    0x10U

#define IO_APIC_IRQ_POLARITY_HIGH  0x0UL
#define IO_APIC_IRQ_POLARITY_LOW   BITUL(12)
#define IO_APIC_IRQ_TRIGGER_EDGE   0x0UL
#define IO_APIC_IRQ_TRIGGER_LEVEL  BITUL(15)
#define IO_APIC_IRQ_DEST_LOGICAL   BITUL(11)
#define IO_APIC_IRQ_LOW_PRIORITY   BITUL(8)
#define IO_APIC_IRQ_MASKED         BITUL(16)
#define IO_APIC_DEST_ALL    0xFFU

#define IO_APIC_MAX_COUNT     0x10
#define LOCAL_APIC_MAX_COUNT  0x08

#define ISA_POLARITY_MASK     0x03
#define ISA_POLARITY_DEFAULT  0x00
#define ISA_POLARITY_HIGH     0x01
#define ISA_POLARITY_LOW      0x03

#define ISA_TRIGGER_MASK      0x0C
#define ISA_TRIGGER_DEFAULT   0x00
#define ISA_TRIGGER_EDGE      0x04
#define ISA_TRIGGER_LEVEL     0x0C

#define ISA_IRQ_MAX_COUNT     0x10

struct io_apic {
	void *vaddr;
	unsigned gsi_begin;
	unsigned gsi_end;
	unsigned id;
};

struct local_apic {
	void *vaddr;
	unsigned id;
};

struct isa_irq_map {
	unsigned gsi;
	unsigned long flags;
};

void setup_apic(void);
void setup_local_apic(void);

#endif /*__X86_ARCH_APIC_H__*/
