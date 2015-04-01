#ifndef __PAGE_ALLOC_H__
#define __PAGE_ALLOC_H__

#include <kernel/list.h>

#define MAX_ORDER       11
#define MAX_ORDER_PAGES (1 << (MAX_ORDER - 1))
#define ZONE_TYPES      2

struct zone;
struct page_list_data;

struct page {
	struct list_head chain;
	struct zone *zone;
	int order;
};

enum zone_type {
	ZONE_NORMAL,
	ZONE_HIGH,
};

struct zone {
	struct page_list_data *node;
	struct list_head free_area[MAX_ORDER];
	unsigned long pages;
};

struct page_list_data {
	struct zone zones[ZONE_TYPES];
	struct page *pages;
	unsigned long start_pfn;
	unsigned long end_pfn;
};

extern struct page_list_data nodes[];

static inline struct zone *page_zone(struct page *page)
{
	return page->zone;
}

static inline struct page_list_data *zone_node(struct zone *zone)
{
	return zone->node;
}

static inline unsigned long buddy_index(unsigned long idx, unsigned order)
{
	return idx ^ (1 << order);
}

static inline unsigned long page_to_pfn(const struct page_list_data *node,
                        const struct page *page)
{
	return page - node->pages;
}

static inline struct page *pfn_to_page(struct page_list_data *node,
                        unsigned long idx)
{
	return node->pages + idx;
}

struct page *alloc_pages(unsigned order, unsigned node, unsigned zone);
void free_pages(struct page *pages, unsigned order);

#endif /*__PAGE_ALLOC_H__*/
