#include <kernel/kmap.h>
#include <arch/memory.h>

#include <stddef.h>

#include <kernel/debug.h>

static pte_t *kernel_map_page_frame = kernel_page_tables + LOWMEM_PAGE_FRAMES;
static pte_t *kernel_map_top_unused = kernel_page_tables + LOWMEM_PAGE_FRAMES;

void *kmap(unsigned long paddr, unsigned long flags)
{
	if (paddr < KMAP_OFFSET - PAGE_OFFSET)
		return virt_addr(paddr);

	pte_t *const pe = kernel_map_top_unused++;
	*pe = create_table_entry(paddr, flags | PTE_PRESENT);

	return (void *)(PAGE_OFFSET +
				((pe - kernel_page_tables) << PAGE_SHIFT));
}

void kunmap(const void *ptr)
{
	const uintptr_t vaddr = (uintptr_t)ptr;
	if (vaddr < KMAP_OFFSET)
		return;

	pte_t *pe = kernel_page_tables + ((vaddr - PAGE_OFFSET) >> PAGE_SHIFT);
	*pe = create_table_entry(0, 0);

	if (pe + 1 != kernel_map_top_unused)
		return;

	while (!(page_flags(*pe) & PTE_PRESENT))
		if (--pe < kernel_map_page_frame)
			break;

	kernel_map_top_unused = pe + 1;
}
