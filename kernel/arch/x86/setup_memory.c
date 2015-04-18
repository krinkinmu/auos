#include <kernel/utility.h>
#include <kernel/debug.h>
#include <arch/asm/cpu.h>
#include <arch/memory.h>
#include <arch/memblock.h>
#include <arch/multiboot.h>

size_t page_frames;
size_t low_mem_page_frames;

static void init_phys_mem_map(struct multiboot_info *mbi)
{
	uint64_t max_phys_addr = 0;
	uintptr_t mmap_entry = mbi->mmap_addr;
	uintptr_t mmap_end = mmap_entry + mbi->mmap_length;

	debug("Physical memory map:\n");
	while (mmap_entry < mmap_end) {
		const struct multiboot_mmap_entry *entry = (void *)mmap_entry;
		const uint64_t addr = entry->addr;
		const uint64_t size = entry->len;
		const uint64_t last = addr + size - 1;
		const unsigned type = entry->type;

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
	const uintptr_t kern_addr = phys_addr(__kernel_begin);
	const uintptr_t kern_end = phys_addr(__kernel_end);
	const uintptr_t kern_size = kern_end - kern_addr;

	debug("Reserve kernel memory 0x%x-0x%x\n", kern_addr, kern_end - 1);
	memblock_reserve(kern_addr, kern_size);
}

void setup_memory(struct multiboot_info *mbi)
{
	init_phys_mem_map(mbi);
	reserve_kernel_memory();
	remap_lower_memory();
	setup_page_alloc();
}
