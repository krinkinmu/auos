#include <utility.h>
#include <debug.h>

#include <arch/x86/memory.h>
#include <arch/x86/cpu.h>

#include "memblock.h"
#include "multiboot.h"

unsigned long __heap_end = phys_addr(__heap_begin);

static inline void clone_pgd_range(pde_t *dst, pde_t *src, int count)
{
	memcpy(dst, src, count * sizeof(pde_t));
}

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

static unsigned long alloc_low_page(void)
{
	unsigned long addr = memblock_alloc_range(PAGE_SIZE, PAGE_SIZE,
					__heap_end, phys_addr(__kernel_end));

	if (!addr)
		panic("Cannot allocate page!");

	return addr;
}

static void map_lower_memory(void)
{
	static unsigned long KERN_PTE_FLAGS = X86_PTE_PRESENT | X86_PTE_WRITE;
	static unsigned long KERN_PDE_FLAGS = X86_PDE_PRESENT | X86_PDE_WRITE;

	clone_pgd_range(swapper_page_dir + KERNEL_PGD_BOUNDARY,
			initial_page_dir + KERNEL_PGD_BOUNDARY,
			KERNEL_PGD_PTRS);

	unsigned long pde_begin = pgd_index(__kernel_end);
	unsigned long pde_end = PTRS_PER_PGD;
	for (unsigned long i = pde_begin; i != pde_end; ++i) {
		unsigned long addr = phys_addr(i << PGDIR_SHIFT);
		pte_t *pt = virt_addr(alloc_low_page());

		for (unsigned long j = 0; j != PTRS_PER_PT; ++j) {
			pt[j] = page_table_entry(addr, KERN_PTE_FLAGS);
			addr += PAGE_SIZE;
		}
		swapper_page_dir[i] = page_dir_entry(pt, KERN_PDE_FLAGS);
	}
}

static void reserve_kernel_memory(void)
{
	unsigned long kern_addr = phys_addr(__kernel_begin);
	unsigned long kern_end = __heap_end;
	unsigned long kern_size = kern_end - kern_addr;

	debug("Reserve kernel memory 0x%x-0x%x\n", kern_addr, kern_end - 1);
	memblock_reserve(kern_addr, kern_size);
}

void setup_memory(struct multiboot_info *mbi)
{
	fill_memory_map(mbi);
	reserve_kernel_memory();
	map_lower_memory();
	load_cr3(phys_addr(swapper_page_dir));
}
