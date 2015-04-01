#include <arch/x86/memory.h>
#include <debug.h>
#include <page.h>

#include "memblock.h"

static struct page_list_data nodes[1];

static void init_zone(struct page_list_data *node, struct zone *zone,
			unsigned long start_pfn, unsigned long end_pfn)
{
	zone->node = node;
	zone->pages = end_pfn - start_pfn;

	for (unsigned i = 0; i != MAX_ORDER; ++i)
		init_list_head(&zone->free_area[i]);

	for (unsigned long pfn = start_pfn; pfn != end_pfn; ++pfn) {
		struct page *page = node->pages + (pfn - start_pfn);
		init_list_head(&page->chain);
		page->zone = zone;
	}
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
	for (unsigned i = 0; i != ZONE_TYPES; ++i)
		init_zone(node, &node->zones[i], start_pfn, max_pfns[i]);
}

static struct zone *page_zone(struct page *page)
{
	return page->zone;
}

static struct page_list_data *zone_node(struct zone *zone)
{
	return zone->node;
}

static unsigned long buddy_index(unsigned long idx, unsigned order)
{
	return idx ^ (1 << order);
}

void free_pages(struct page *pages, unsigned order)
{
	struct zone *zone = page_zone(pages);
	struct page_list_data *node = zone_node(zone);
	unsigned long idx = node->pages - pages;

	while (order < MAX_ORDER - 1) {
		unsigned long bidx = buddy_index(idx, order);
		if (bidx >= zone->pages)
			break;

		struct page *buddy = &node->pages[bidx];
		if ((int)order != buddy->order)
			break;

		remove_from_list(&buddy->chain);
		++order;

		if (bidx < idx) {
			pages = buddy;
			idx = bidx;
		}
	}
	pages->order = (int)order;
	insert_before(&zone->free_area[order], &pages->chain);
}

struct page *alloc_pages(unsigned order, unsigned nid, unsigned type)
{
	struct page_list_data *node = &nodes[nid];
	struct zone *zone = &node->zones[type];
	unsigned corder = order;

	while (corder < MAX_ORDER) {
		if (!list_empty(&zone->free_area[corder]))
			break;
		++corder;
	}

	if (corder == MAX_ORDER)
		return 0;

	struct page *page = (void *)(&zone->free_area[corder].next);
	remove_from_list(&page->chain);

	while (corder > order) {
		unsigned long idx = node->pages - page;
		unsigned long bidx = buddy_index(idx, --corder);
		struct page *buddy = node->pages + bidx;
		buddy->order = corder;
		insert_before(&zone->free_area[corder], &buddy->chain);
	}
	page->order = -1;

	return page;
}

void init_page_alloc(void)
{
	unsigned long max_zone_pfns[ZONE_TYPES];
	max_zone_pfns[ZONE_NORMAL] = low_mem_page_frames;
	max_zone_pfns[ZONE_HIGH] = page_frames;
	init_node(&nodes[0], max_zone_pfns);
}
