#include <kernel/page_alloc.h>
#include <kernel/kernel.h>
#include <kernel/debug.h>

static inline size_t buddy_pfn(size_t pfn, unsigned order)
{
	return pfn ^ (1 << order);
}

static void free_pages_node(struct page *pages, int order, struct zone *zone)
{
	size_t pfn = page_to_pfn(pages);

	while (order < MAX_ORDER - 1) {
		const size_t bpfn = buddy_pfn(pfn, order);

		if (bpfn >= zone->end_pfn || bpfn < zone->start_pfn)
			break;

		struct page *buddy = pfn_to_page(bpfn);

		if (order != buddy->order)
			break;

		list_remove(&buddy->chain);
		++order;

		if (bpfn < pfn) {
			pages = buddy;
			pfn = bpfn;
		}
	}
	pages->order = order;
	list_insert_before(&zone->free_area[order], &pages->chain);
}

void free_pages(struct page *pages, unsigned order)
{
	assert((pages->order < 0) && ((unsigned)(-pages->order - 1) == order),
			"Freeing pages with wrong order\n");

	struct zone *zone = page_zone(pages);

	free_pages_node(pages, (int)order, zone);
}

static struct page *alloc_pages_node(int order, struct zone *zone)
{
	int corder = order;

	while (corder < MAX_ORDER) {
		if (!list_empty(&zone->free_area[corder]))
			break;
		++corder;
	}

	if (corder >= MAX_ORDER)
		return 0;

	struct page *page =
		CONTAINER_OF(zone->free_area[corder].next, struct page, chain);

	list_remove(&page->chain);
	while (corder > order) {
		const size_t pfn = page_to_pfn(page);
		const size_t bpfn = buddy_pfn(pfn, --corder);
		struct page *buddy = pfn_to_page(bpfn);

		buddy->order = corder;
		list_insert_before(&zone->free_area[corder], &buddy->chain);
	}
	page->order = -order - 1;

	return page;
}

struct page *alloc_pages(unsigned order, unsigned flags)
{
	assert(order < MAX_ORDER, "Too large allocation size\n");
	assert(flags & ALLOC_NORMAL,
		"Only ZONE_NORMAL allocations are supported\n");

	(void)flags;

	struct page_list_data *node = &nodes[0];
	struct zone *zone = &node->zones[ZONE_NORMAL];

	return alloc_pages_node((int)order, zone);
}
