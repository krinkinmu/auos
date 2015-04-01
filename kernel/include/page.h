#ifndef __PAGE_H__
#define __PAGE_H__

#include <list.h>

#define MAX_ORDER       11
#define MAX_ORDER_PAGES (1 << (MAX_ORDER - 1))
#define ZONE_TYPES      2

#define PAGE_FREE       1UL

struct zone;
struct page {
	struct list_head chain;
	struct zone *zone;
	int order;
};

enum zone_type {
	ZONE_NORMAL,
	ZONE_HIGH,
	_ZONE_TYPES
};

struct page_list_data;
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

struct page *alloc_pages(unsigned order, unsigned node, unsigned zone);
void free_pages(struct page *pages, unsigned order);

#endif /*__PAGE_H__*/
