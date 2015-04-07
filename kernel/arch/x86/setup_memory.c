#include <kernel/utility.h>
#include <kernel/debug.h>
#include <arch/descriptor_table.h>
#include <arch/memory.h>
#include <arch/cpu.h>

#include "memblock.h"
#include "multiboot.h"

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
		} else {
			memblock_reserve(addr, size);
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

static void setup_init_gdt(void)
{
	static gdt_entry_t init_gdt[GDT_SIZE];
	static gdt_descr_t init_gdt_descr;

	init_entry(&init_gdt[KERNEL_CODE_ENTRY],
				0xFFFFFULL, 0x0ULL, KERNEL_CODE_TYPE);
	init_entry(&init_gdt[KERNEL_DATA_ENTRY],
				0xFFFFFULL, 0x0ULL, KERNEL_DATA_TYPE);

	init_entry(&init_gdt[USER_CODE_ENTRY],
				0xFFFFFULL, 0x0ULL, USER_CODE_TYPE);
	init_entry(&init_gdt[USER_DATA_ENTRY],
				0xFFFFFULL, 0x0ULL, USER_DATA_TYPE);

	init_descriptor_ptr(&init_gdt_descr, init_gdt, GDT_SIZE);
	set_gdt(&init_gdt_descr);
	reload_segment_registers();
}

void setup_memory(struct multiboot_info *mbi)
{
	setup_init_gdt();
	init_phys_mem_map(mbi);
	reserve_kernel_memory();
	remap_lower_memory();
	setup_page_alloc();
}
