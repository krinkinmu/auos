#include <kernel/kernel.h>
#include <kernel/debug.h>
#include <kernel/utility.h>

#include "memory.h"
#include "memblock.h"

#define MEMBLOCK_REGIONS 128

static struct memblock_region all_regions[MEMBLOCK_REGIONS];
static struct memblock_region reserved_regions[MEMBLOCK_REGIONS];

static struct memblock memblock = {
	.memory.regions    = all_regions,
	.memory.size       = 0,
	.memory.capacity   = MEMBLOCK_REGIONS,

	.reserved.regions  = reserved_regions,
	.reserved.size     = 0,
	.reserved.capacity = MEMBLOCK_REGIONS,
};

static void memblock_regions_merge(struct memblock_region *dst,
				const struct memblock_region *src)
{
	unsigned long long addr = dst->addr;
	unsigned long long end = addr + dst->size;
	unsigned long long send = src->addr + src->size;

	if (src->addr < addr)
		addr = src->addr;

	if (send > end)
		end = send;
 
	dst->addr = addr;
	dst->size = end - addr;
}

static int memblock_regions_overlap(const struct memblock_region *lhs,
				const struct memblock_region *rhs)
{
	unsigned long long laddr = lhs->addr;
	unsigned long long lend = laddr + lhs->size;

	unsigned long long raddr = rhs->addr;
	unsigned long long rend = raddr + rhs->size;

	return (laddr <= raddr && raddr <= lend) ||
		(laddr <= rend && rend <= lend);
}

static void memblock_region_remove(struct memblock_type *type,
				unsigned i)
{
	for (; i + 1 < type->size; ++i)
		type->regions[i] = type->regions[i + 1];
	--type->size;
}

static int memblock_region_insert(struct memblock_type *type,
				struct memblock_region *reg, unsigned i)
{
	if (type->size == type->capacity)
		return -1;

	for (unsigned j = type->size; j > i; --j)
		type->regions[j] = type->regions[j - 1];
	type->regions[i] = *reg;
	++type->size;
	return 0;
}

static int memblock_add_range(struct memblock_type *type,
				unsigned long long addr,
				unsigned long long size)
{
	struct memblock_region reg = { addr, size };
	unsigned i;

	if (!size)
		return 0;

	struct memblock_region *dst;

	for (i = 0; i != type->size; ++i) {
		dst = &type->regions[i];
		unsigned long long rend = dst->addr + dst->size;

		if (rend >= addr)
			break;
	}

	if (i != type->size && memblock_regions_overlap(dst, &reg)) {
		memblock_regions_merge(dst, &reg);
		if (i + 1 != type->size &&
				memblock_regions_overlap(dst, dst + 1)) {
			memblock_regions_merge(dst, dst + 1);
			memblock_region_remove(type, i + 1);
		}
		return 0;
	}

	return memblock_region_insert(type, &reg, i);
}

/*
 This is hard one. I'll try to explain it in details. Function takes four
 arguments: size is size of requested memory block, align is block
 alignment, [from and to) is physical memory range in which we lookup
 memory block. Function returns physical address if allocation is
 successful and zero otherwise. Also I suppose that align is power of two.
*/
static unsigned long memblock_find_in_range(unsigned long size,
				unsigned align, unsigned long long from,
				unsigned long long to)
{
	struct memblock_type *mem = &memblock.memory;
	struct memblock_type *res = &memblock.reserved;

	/*
	 j is an index of current region in reserved memory blocks array.
	*/
	unsigned j = 0;

	/* i iterates over all accessible memory regions. */
	for (unsigned i = 0; i != mem->size; ++i) {
		/*
		 If end of the current region is less or equal to from, then
		 it is before the requested memory range, so go to the next.
		 */
		if (mem->regions[i].addr + mem->regions[i].size <= from)
			continue;

		/*
		 If start of the current region is greater or equal than to,
		 then it is after the requested memory range, and so we can
		 stop here and exit.
		*/
		if (mem->regions[i].addr >= to)
			break;

		/*
		 addr iterates over address in the current memory region,
		 and end holds end of the current memory region.
		*/
		unsigned long long addr = mem->regions[i].addr;
		unsigned long long end = addr + mem->regions[i].size;

		addr = max(addr, from);
		end = min(end, to);

		while (addr < end) {
			/* Align addr on requested border. */
			addr = ALIGN_UP(addr, (unsigned long long)align);

			/* Has the region got enough space? */
			if (addr + size > end)
				break;

			/* [raddr, rend) is current reserved region. */
			unsigned long long raddr = 0;
			unsigned long long rend = 0;

			/* Iterate over reserved regions. */
			for (; j != res->size; ++j) {
				raddr = res->regions[j].addr;
				rend = raddr + res->regions[j].size;

				/* This one can disturb us, so stop here. */
				if (rend > addr)
					break;
			}

			/* Check that [addr, addr + size) isn't reserved.  */
			if (j == res->size || raddr >= addr + size)
				return addr;

			addr = rend;
		}
	}

	return 0;
}

int memblock_add(unsigned long long addr, unsigned long long size)
{
	return memblock_add_range(&memblock.memory, addr, size);
}

int memblock_reserve(unsigned long long addr, unsigned long long size)
{
	return memblock_add_range(&memblock.reserved, addr, size);
}

unsigned long memblock_alloc_range(unsigned long size, unsigned align,
				unsigned long long from, unsigned long long to)
{
	unsigned long addr = memblock_find_in_range(size, align, from, to);
	if (addr && !memblock_reserve(addr, size)) {
		debug("Allocated region 0x%x-0x%x\n", addr, addr + size - 1);
		memset(virt_addr(addr), 0, size);
	}
	return addr;
}

int memblock_is_free(unsigned long addr, unsigned long size)
{
	unsigned long long end = (unsigned long long)addr + size;

	return memblock_find_in_range(size, 1, addr, end) != 0;
}

static void memblock_list_regions(const struct memblock_type *type)
{
	for (unsigned i = 0; i != type->size; ++i) {
		unsigned long addr = type->regions[i].addr;
		unsigned long end = addr + type->regions[i].size;

		debug("region 0x%x-0x%x\n", addr, end);
	}
}

void memblock_list_reserved(void)
{
	struct memblock_type *res = &memblock.reserved;

	memblock_list_regions(res);
}

void memblock_list_memory(void)
{
	struct memblock_type *mem = &memblock.memory;

	memblock_list_regions(mem);
}
