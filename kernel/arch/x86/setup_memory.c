#include <kernel/bootmem.h>
#include <kernel/kernel.h>
#include <kernel/utility.h>
#include <kernel/debug.h>

#include <arch/asm/cpu.h>
#include <arch/memory.h>
#include <arch/multiboot.h>

static size_t page_frame_count;
static size_t lowmem_page_frame_count;

static void init_phys_mem_map(struct multiboot_info *mbi)
{
	const unsigned long phys_max = (unsigned long)PHYS_MEM_MAX;

	unsigned long phys_mem_size = 0;
	unsigned long mmap_entry = mbi->mmap_addr;
	unsigned long mmap_end = mmap_entry + mbi->mmap_length;

	debug("Physical memory map:\n");
	while (mmap_entry < mmap_end) {
		const struct multiboot_mmap_entry *entry = (void *)mmap_entry;

		mmap_entry += entry->size + sizeof(entry->size);
		if (entry->addr > phys_max)
			continue;

		const unsigned long begin = entry->addr;
		const unsigned long end = minu(begin + entry->len, phys_max);
		const unsigned type = entry->type;

		if (type == MULTIBOOT_AVAILABLE) {
			if (end > phys_mem_size)
				phys_mem_size = end;
			boot_mem_add(begin, end);
		} else {
			boot_mem_reserve(begin, end);
		}

		debug("memory region: 0x%x-0x%x type=%u\n",
			begin,
			end - 1,
			type);
	}

	page_frame_count = phys_mem_size >> PAGE_SHIFT;
        lowmem_page_frame_count = minu(page_frame_count, LOWMEM_PAGE_FRAMES);

	debug("Page frames at all: 0x%x\n", page_frame_count);
	debug("Low mem page frames at all: 0x%x\n", lowmem_page_frame_count);
}

static void remap_lower_memory(void)
{
	memcpy(swapper_page_dir + LOWMEM_PAGE_DIR,
		initial_page_dir + LOWMEM_PAGE_DIR,
		KERNEL_PAGE_DIRS * sizeof(pde_t));

	load_cr3(phys_addr(swapper_page_dir));
}

static void reserve_kernel_memory(void)
{
	const unsigned long kern_addr = phys_addr(__kernel_begin);
	const unsigned long kern_end = phys_addr(__kernel_end);

	debug("Reserve kernel memory 0x%x-0x%x\n", kern_addr, kern_end - 1);
	boot_mem_reserve(kern_addr, kern_end);
}

pte_t kernel_page_tables[KERNEL_PAGE_FRAMES] ALIGN(PAGE_SIZE);

size_t page_frames(void)
{
	return page_frame_count;
}

size_t lowmem_page_frames(void)
{
	return lowmem_page_frame_count;
}

void setup_memory(struct multiboot_info *mbi)
{
	init_phys_mem_map(mbi);
	reserve_kernel_memory();
	remap_lower_memory();
	setup_page_alloc();
}
