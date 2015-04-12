#ifndef __X86_ARCH_MEMBLOCK_H__
#define __X86_ARCH_MEMBLOCK_H__

#include <stddef.h>
#include <stdint.h>

struct memblock_region {
	uint64_t addr;
	uint64_t size;
};

struct memblock_type {
	struct memblock_region *regions;
	size_t size;
	size_t capacity;
};

struct memblock {
	struct memblock_type memory;
	struct memblock_type reserved;
};

int memblock_add(uint64_t addr, uint64_t size);
int memblock_reserve(uint64_t addr, uint64_t size);
uint64_t memblock_alloc_range(size_t size, size_t align, uint64_t from,
			uint64_t to);
int memblock_is_free(uint64_t addr, uint64_t size);

#endif /*__X86_ARCH_MEMBLOCK_H__*/
