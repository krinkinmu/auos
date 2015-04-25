#ifndef __X86_ARCH_APIC_H__
#define __X86_ARCH_APIC_H__

#include <kernel/kmap.h>

#define IO_APIC_DEFAULT_PADDR 0xFEC00000UL
#define IO_APIC_PAGE_FLAGS    (PTE_KERNEL | PTE_READ | PTE_WRITE | PTE_NOCACHE)
#define IO_APIC_VERSION_REG   0x01U

struct io_apic {
	void *vaddr;
	unsigned gsi_begin;
	unsigned gsi_end;
};

void setup_io_apic(void);

#endif /*__X86_ARCH_APIC_H__*/
