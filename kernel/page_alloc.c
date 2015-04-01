#include <kernel/page_alloc.h>
#include <kernel/kernel.h>

void free_pages(struct page *pages, unsigned order)
{
	struct zone *zone = page_zone(pages);
	struct page_list_data *node = zone_node(zone);
	unsigned long idx = page_to_pfn(node, pages);

	while (order < MAX_ORDER - 1) {
		unsigned long bidx = buddy_index(idx, order);

		if (bidx >= zone->pages)
			break;

		struct page *buddy = pfn_to_page(node, bidx);

		if ((int)order != buddy->order)
			break;

		list_remove(&buddy->chain);
		++order;

		if (bidx < idx) {
			pages = buddy;
			idx = bidx;
		}
	}
	pages->order = (int)order;
	list_insert_before(&zone->free_area[order], &pages->chain);
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

	if (corder >= MAX_ORDER)
		return 0;

	struct page *page = container_of(zone->free_area[corder].next,
					struct page, chain);

	list_remove(&page->chain);
	while (corder > order) {
		unsigned long idx = page_to_pfn(node, page);
		unsigned long bidx = buddy_index(idx, --corder);
		struct page *buddy = pfn_to_page(node, bidx);

		buddy->order = corder;
		list_insert_before(&zone->free_area[corder], &buddy->chain);
	}
	page->order = -1;

	return page;
}
