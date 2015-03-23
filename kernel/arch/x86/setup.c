#include <arch/x86/memory.h>

#include "utility.h"
#include "multiboot.h"
#include "io.h"

static inline void clone_pgd_range(pgd_t *dst, pgd_t *src, int count)
{
	memcpy(dst, src, count * sizeof(pgd_t));
}

void setup_arch(void *bootstrap)
{
	struct multiboot_info *mbi = bootstrap;
	unsigned long mmap_entry = mbi->mmap_addr;
	unsigned long mmap_end = mmap_entry + mbi->mmap_length;

	while (mmap_entry < mmap_end) {
		struct multiboot_mmap_entry *entry = (void *)mmap_entry;
		unsigned long addr = entry->addr;
		unsigned long size = entry->len;
		unsigned long type = entry->type;

		printf("memory region: 0x%x-0x%x type=%u\n",
			addr, addr + size - 1, type);
		mmap_entry += entry->size + sizeof(entry->size);
	}

	clone_pgd_range(swapper_page_dir + KERNEL_PGD_BOUNDARY,
			initial_page_dir + KERNEL_PGD_BOUNDARY,
			KERNEL_PGD_PTRS);
	load_cr3(phys_addr(swapper_page_dir));
}

void main(void *bootstrap)
{
	init_io();
	puts("Hello, World!");
	setup_arch(bootstrap);
	while (1);
}
