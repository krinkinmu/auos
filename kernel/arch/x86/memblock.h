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
unsigned long memblock_alloc_range(unsigned long size, unsigned long align,
				unsigned long from, unsigned long to);
void memblock_free_range(unsigned long addr, unsigned long size);

#endif /*__MEMBLOCK_H__*/
