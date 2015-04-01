#include <utility.h>
#include <debug.h>

#include <arch/x86/memory.h>
#include <arch/x86/cpu.h>

#include "memblock.h"
#include "multiboot.h"

void init_page_alloc(void);

unsigned long page_frames;
unsigned long low_mem_page_frames;

static void init_phys_mem_map(struct multiboot_info *mbi)
{
	unsigned long long max_phys_addr = 0;
	unsigned long mmap_entry = mbi->mmap_addr;
	unsigned long mmap_end = mmap_entry + mbi->mmap_length;

	debug("Physical memory map:\n");
	while (mmap_entry < mmap_end) {
		struct multiboot_mmap_entry *entry = (void *)mmap_entry;
		unsigned long long addr = entry->addr;
		unsigned long long size = entry->len;
		unsigned long long last = addr + size - 1;
		unsigned long type = entry->type;


		if (type == MULTIBOOT_AVAILABLE) {
			if (last > max_phys_addr)
				max_phys_addr = last;
			memblock_add(addr, size);
		}

		debug("memory region: 0x%x-0x%x type=%u\n",
			(unsigned long)addr,
			(unsigned long)(addr + size - 1),
			type);
		mmap_entry += entry->size + sizeof(entry->size);
	}

	if (max_phys_addr > PHYS_MEM_MAX)
		max_phys_addr = PHYS_MEM_MAX;

	page_frames = 1 + ((max_phys_addr - PAGE_SIZE + 1) >> PAGE_SHIFT);
	low_mem_page_frames = LOW_PAGE_FRAMES_MAX;
	if (low_mem_page_frames > page_frames)
		low_mem_page_frames = page_frames;
	debug("Page frames at all: 0x%x\n", page_frames);
	debug("Low mem page frames at all: 0x%x\n", low_mem_page_frames);
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
	init_phys_mem_map(mbi);
	reserve_kernel_memory();
	remap_lower_memory();
	init_page_alloc();
}
