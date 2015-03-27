#include <utility.h>
#include <debug.h>

#include <arch/x86/memory.h>
#include <arch/x86/cpu.h>

#include "memblock.h"
#include "multiboot.h"

static void fill_memory_map(struct multiboot_info *mbi)
{
	unsigned long mmap_entry = mbi->mmap_addr;
	unsigned long mmap_end = mmap_entry + mbi->mmap_length;

	debug("Physical memory map:\n");
	while (mmap_entry < mmap_end) {
		struct multiboot_mmap_entry *entry = (void *)mmap_entry;
		unsigned long long addr = entry->addr;
		unsigned long long size = entry->len;
		unsigned long type = entry->type;

		if (addr + size - 1 > PHYS_MEM_MAX)
			continue;

		if (type == MULTIBOOT_AVAILABLE)
			memblock_add(addr, size);

		debug("memory region: 0x%x-0x%x type=%u\n",
			(unsigned long)addr,
			(unsigned long)(addr + size - 1),
			type);
		mmap_entry += entry->size + sizeof(entry->size);
	}
}

static void remap_lower_memory(void)
{
	memcpy(swapper_page_dir + KERNEL_PGD_BOUNDARY,
		initial_page_dir + KERNEL_PGD_BOUNDARY,
		KERNEL_PGD_PTRS * sizeof(pde_t));

	load_cr3(phys_addr(swapper_page_dir));
}

static void reserve_kernel_memory(void)
{
	unsigned long kern_addr = phys_addr(__kernel_begin);
	unsigned long kern_end = phys_addr(__kernel_end);
	unsigned long kern_size = kern_end - kern_addr;

	debug("Reserve kernel memory 0x%x-0x%x\n", kern_addr, kern_end - 1);
	memblock_reserve(kern_addr, kern_size);
}

void setup_memory(struct multiboot_info *mbi)
{
	fill_memory_map(mbi);
	reserve_kernel_memory();
	remap_lower_memory();
}
