#ifndef __KERNEL_PAGE_ALLOC_H__
#define __KERNEL_PAGE_ALLOC_H__

#include <kernel/list.h>
#include <kernel/kernel.h>
#include <arch/memory.h>

#include <stddef.h>
#include <stdint.h>

#define MAX_ORDER       11
#define MAX_ORDER_PAGES (1 << (MAX_ORDER - 1))
#define ZONE_TYPES      2

#define ALLOC_HIGH      BITUL(1)
#define ALLOC_NORMAL    BITUL(2)

struct zone;
struct page_list_data;
struct slab;

struct page {
	struct list_head chain;
	struct zone *zone;
	struct slab *slab;
	int order;
};

enum zone_type {
	ZONE_NORMAL,
	ZONE_HIGH,
};

struct zone {
	struct page_list_data *node;
	struct list_head free_area[MAX_ORDER];
	size_t pages;
};

struct page_list_data {
	struct zone zones[ZONE_TYPES];
	struct page *pages;
	size_t start_pfn;
	size_t end_pfn;
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

static inline size_t buddy_index(size_t idx, unsigned order)
{
	return idx ^ (1 << order);
}

static inline size_t page_to_pfn(const struct page *page)
{
	return page - nodes[0].pages;
}

static inline struct page *pfn_to_page(size_t idx)
{
	return nodes[0].pages + idx;
}

static inline uintptr_t page_physical_address(const struct page *page)
{
	return (uintptr_t)page_to_pfn(page) << PAGE_SHIFT;
}

static inline void *page_virtual_address(const struct page *page)
{
	return virt_addr(page_physical_address(page));
}

struct page *alloc_pages(unsigned order, unsigned flags);
void free_pages(struct page *pages, unsigned order);

#endif /*__KERNEL_PAGE_ALLOC_H__*/
