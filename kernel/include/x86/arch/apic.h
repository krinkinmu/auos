#ifndef __X86_ARCH_APIC_H__
#define __X86_ARCH_APIC_H__

#include <kernel/kmap.h>

#define IO_APIC_DEFAULT_PADDR 0xFEC00000UL
#define IO_APIC_DEFAULT_PADDR 0xFEC00000UL
#define IO_APIC_PAGE_FLAGS    (PTE_KERNEL | PTE_READ | PTE_WRITE | PTE_NOCACHE)
#define LOCAL_APIC_PAGE_FLAGS IO_APIC_PAGE_FLAGS
#define IO_APIC_ID_REG        0x00U
#define IO_APIC_VERSION_REG   0x01U

#define IO_APIC_MAX_COUNT     0x10
#define LOCAL_APIC_MAX_COUNT  0x08

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

void setup_apic(void);
void setup_local_apic(void);

#endif /*__X86_ARCH_APIC_H__*/
