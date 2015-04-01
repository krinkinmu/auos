#ifndef __MEMBLOCK_H__
#define __MEMBLOCK_H__

struct memblock_region {
	unsigned long long addr;
	unsigned long long size;
};

struct memblock_type {
	unsigned long size;
	unsigned long capacity;
	struct memblock_region *regions;
};

struct memblock {
	struct memblock_type memory;
	struct memblock_type reserved;
};

int memblock_add(unsigned long long addr, unsigned long long size);
int memblock_reserve(unsigned long long addr, unsigned long long size);
unsigned long memblock_alloc_range(unsigned long size, unsigned align,
				unsigned long long from,
				unsigned long long to);
int memblock_is_free(unsigned long addr, unsigned long size);
void memblock_list_reserved(void);
void memblock_list_memory(void);

#endif /*__MEMBLOCK_H__*/
