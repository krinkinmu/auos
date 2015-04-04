#include <kernel/page_alloc.h>
#include <kernel/kernel.h>
#include <kernel/debug.h>

static void __free_pages_node(struct page *pages, int order, struct zone *zone)
{
	unsigned long idx = page_to_pfn(pages);

	while (order < MAX_ORDER - 1) {
		const unsigned long bidx = buddy_index(idx, order);

		if (bidx >= zone->pages)
			break;

		struct page *const buddy = pfn_to_page(bidx);

		if (order != buddy->order)
			break;

		list_remove(&buddy->chain);
		++order;

		if (bidx < idx) {
			pages = buddy;
			idx = bidx;
		}
	}
	pages->order = order;
	list_insert_before(&zone->free_area[order], &pages->chain);
}

void free_pages(struct page *pages, unsigned order)
{
	assert((pages->order < 0) && ((unsigned)(-pages->order - 1) == order),
			"Freeing pages with wrong order\n");

	struct zone *const zone = page_zone(pages);

	__free_pages_node(pages, (int)order, zone);
}

static struct page *__alloc_pages_node(int order, struct zone *zone)
{
	int corder = order;

	while (corder < MAX_ORDER) {
		if (!list_empty(&zone->free_area[corder]))
			break;
		++corder;
	}

	if (corder >= MAX_ORDER)
		return 0;

	struct page *const page = container_of(zone->free_area[corder].next,
					struct page, chain);

	list_remove(&page->chain);
	while (corder > order) {
		const unsigned long idx = page_to_pfn(page);
		const unsigned long bidx = buddy_index(idx, --corder);
		struct page *const buddy = pfn_to_page(bidx);

		buddy->order = corder;
		list_insert_before(&zone->free_area[corder], &buddy->chain);
	}
	page->order = -order;

	return page;
}

struct page *alloc_pages(unsigned order, unsigned flags)
{
	assert(order < MAX_ORDER, "Too large allocation size\n");
	assert(flags & ALLOC_NORMAL,
		"Only ZONE_NORMAL allocations are supported\n");

	(void)flags;

	struct page_list_data *const node = &nodes[0];
	struct zone *const zone = &node->zones[ZONE_NORMAL];

	return __alloc_pages_node((int)order, zone);
}
