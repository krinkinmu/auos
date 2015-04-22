#ifndef __X86_ARCH_APIC_H__
#define __X86_ARCH_APIC_H__

#include <kernel/kmap.h>

#define IO_APIC_DEFAULT_PADDR 0xFEC00000UL
#define IO_APIC_PAGE_FLAGS    (PTE_KERNEL | PTE_READ | PTE_WRITE | PTE_NOCACHE)
#define IO_APIC_ID_REG        0x00UL

struct io_apic {
	void *vaddr;
	uint32_t id;
	uint32_t gsi_base;
};

uint32_t io_apic_read(const struct io_apic *apic, int reg);

void setup_io_apic(void);

#endif /*__X86_ARCH_APIC_H__*/
