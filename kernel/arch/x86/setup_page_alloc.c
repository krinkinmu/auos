#include <kernel/page_alloc.h>
#include <kernel/kernel.h>
#include <kernel/debug.h>
#include <kernel/bootmem.h>
#include <arch/memory.h>

struct page_list_data nodes[1];

static void init_zone(struct page_list_data *node, struct zone *zone,
			size_t start_pfn, size_t end_pfn)
{
	zone->node = node;
	zone->start_pfn = start_pfn;
	zone->end_pfn = end_pfn;

	for (size_t i = 0; i != MAX_ORDER; ++i)
		list_init_head(&zone->free_area[i]);

	for (size_t pfn = start_pfn; pfn != end_pfn; ++pfn) {
		struct page *page = pfn_to_page(pfn - node->start_pfn);

		list_init_head(&page->chain);
		page->zone = zone;
		page->order = -1;
	}

	size_t free = 0;
	for (size_t pfn = start_pfn; pfn != end_pfn; ++pfn) {
		if (boot_mem_is_free(pfn << PAGE_SHIFT, PAGE_SIZE)) {
			struct page *page = pfn_to_page(pfn - node->start_pfn);

			free_pages(page, 0);
			++free;
		}
	}

	debug("Zone [0x%x-0x%x) is initialized\n", start_pfn, end_pfn);
	debug("there are 0x%x free pages\n", free);
}

static struct page *alloc_page_array(unsigned long pages)
{
	static const unsigned long begin = PAGE_SIZE;
	static const unsigned long end = LOWMEM_PAGE_FRAMES << PAGE_SHIFT;
	static const unsigned align = PAGE_SIZE;

	const unsigned long size = pages * sizeof(struct page);
	const unsigned long paddr = boot_mem_alloc(size, align, begin, end);

	assert(paddr != end, "alloc_page_array: allocation failed\n");
	return virt_addr(paddr);
}

static void init_node(struct page_list_data *node, const size_t *max_pfns)
{
	size_t start_pfn = 0;
	size_t end_pfn = max_pfns[ZONE_TYPES - 1];

	node->start_pfn = start_pfn;
	node->end_pfn = end_pfn;
	node->pages = alloc_page_array(end_pfn - start_pfn);

	for (size_t i = 0; i != ZONE_TYPES; ++i) {
		init_zone(node, &node->zones[i], start_pfn, max_pfns[i]);
		start_pfn = max_pfns[i];
	}
}

void setup_page_alloc(void)
{
	size_t max_zone_pfns[ZONE_TYPES];

	max_zone_pfns[ZONE_NORMAL] = lowmem_page_frames();
	max_zone_pfns[ZONE_HIGH] = page_frames();
	init_node(&nodes[0], max_zone_pfns);
	debug("Page allocator is initialized\n");
}
