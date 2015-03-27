#include <arch/x86/memory.h>
#include <utility.h>
#include "memblock.h"
#include "multiboot.h"
#include "io.h"

unsigned long __heap_end = phys_addr(__heap_begin);

static inline void clone_pgd_range(pgd_t *dst, pgd_t *src, int count)
{
	memcpy(dst, src, count * sizeof(pgd_t));
}

static void fill_memblock(struct multiboot_info *mbi)
{
	unsigned long mmap_entry = mbi->mmap_addr;
	unsigned long mmap_end = mmap_entry + mbi->mmap_length;

	while (mmap_entry < mmap_end) {
		struct multiboot_mmap_entry *entry = (void *)mmap_entry;
		unsigned long long addr = entry->addr;
		unsigned long long size = entry->len;
		unsigned long type = entry->type;

		if (addr + size - 1 > PHYS_MEM_MAX)
			continue;

		if (type == MULTIBOOT_AVAILABLE)
			memblock_add(addr, size);

		printf("memory region: 0x%x-0x%x type=%u\n",
			(unsigned long)addr,
			(unsigned long)(addr + size - 1),
			type);
		mmap_entry += entry->size + sizeof(entry->size);
	}
}

void setup_memory(struct multiboot_info *mbi)
{
	unsigned long kern_addr = phys_addr(__kernel_begin);
	unsigned long kern_end = __heap_end;
	unsigned long kern_size = kern_end - kern_addr;

	fill_memblock(mbi);

	printf("reserve kernel memory: 0x%x-0x%x\n", kern_addr, kern_end - 1);
	memblock_reserve(kern_addr, kern_size);

	clone_pgd_range(swapper_page_dir + KERNEL_PGD_BOUNDARY,
			initial_page_dir + KERNEL_PGD_BOUNDARY,
			KERNEL_PGD_PTRS);
	load_cr3(phys_addr(swapper_page_dir));
}
