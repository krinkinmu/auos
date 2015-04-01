#include <kernel/page_alloc.h>
#include <kernel/kernel.h>
#include <kernel/debug.h>

#include "memory.h"
#include "memblock.h"

struct page_list_data nodes[1];

static void init_zone(struct page_list_data *node, struct zone *zone,
			unsigned long start_pfn, unsigned long end_pfn)
{
	zone->node = node;
	zone->pages = end_pfn - start_pfn;

	for (unsigned i = 0; i != MAX_ORDER; ++i)
		list_init_head(&zone->free_area[i]);

	for (unsigned long pfn = start_pfn; pfn != end_pfn; ++pfn) {
		struct page *page = pfn_to_page(node, pfn - start_pfn);

		list_init_head(&page->chain);
		page->zone = zone;
		page->order = -1;
	}

	unsigned long free = 0;
	for (unsigned long pfn = start_pfn; pfn != end_pfn; ++pfn) {
		if (memblock_is_free(pfn << PAGE_SHIFT, PAGE_SIZE)) {
			free_pages(pfn_to_page(node, pfn - start_pfn), 0);
			++free;
		}
	}

	debug("Zone [0x%x-0x%x) is initialized\n", start_pfn, end_pfn);
	debug("there are 0x%x free pages\n", free);
}

static void init_node(struct page_list_data *node, unsigned long *max_pfns)
{
	unsigned long start_pfn = 0;
	unsigned long end_pfn = max_pfns[ZONE_TYPES - 1];

	node->start_pfn = start_pfn;
	node->end_pfn = end_pfn;

	unsigned long size = (end_pfn - start_pfn) * sizeof(struct page);
	unsigned long paddr = memblock_alloc_range(size, PAGE_SIZE, PAGE_SIZE,
					low_mem_page_frames << PAGE_SHIFT);
	if (!paddr)
		panic("Cannot allocate struct page array of size %u\n", size);

	node->pages = virt_addr(paddr);
	for (unsigned i = 0; i != ZONE_TYPES; ++i) {
		init_zone(node, &node->zones[i], start_pfn, max_pfns[i]);
		start_pfn = max_pfns[i];
	}
}

void setup_page_alloc(void)
{
	unsigned long max_zone_pfns[ZONE_TYPES];

	max_zone_pfns[ZONE_NORMAL] = low_mem_page_frames;
	max_zone_pfns[ZONE_HIGH] = page_frames;
	init_node(&nodes[0], max_zone_pfns);
	debug("Page allocator is initialized\n");
}
