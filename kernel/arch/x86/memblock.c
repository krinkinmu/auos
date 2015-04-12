#include <kernel/utility.h>
#include <kernel/kernel.h>
#include <kernel/debug.h>

#include <arch/memory.h>
#include <arch/memblock.h>

#define MEMBLOCK_REGIONS 128

static struct memblock_region all_regions[MEMBLOCK_REGIONS];
static struct memblock_region reserved_regions[MEMBLOCK_REGIONS];

static struct memblock memblock = {
	.memory = { all_regions, 0, MEMBLOCK_REGIONS },
	.reserved = { reserved_regions, 0, MEMBLOCK_REGIONS },
};

static void memblock_regions_merge(struct memblock_region *dst,
			const struct memblock_region *src)
{
	uint64_t addr = dst->addr;
	uint64_t end = addr + dst->size;
	uint64_t send = src->addr + src->size;

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
	uint64_t laddr = lhs->addr;
	uint64_t lend = laddr + lhs->size;

	uint64_t raddr = rhs->addr;
	uint64_t rend = raddr + rhs->size;

	return (laddr <= raddr && raddr <= lend) ||
		(laddr <= rend && rend <= lend);
}

static void memblock_region_remove(struct memblock_type *type, size_t i)
{
	for (; i + 1 < type->size; ++i)
		type->regions[i] = type->regions[i + 1];
	--type->size;
}

static int memblock_region_insert(struct memblock_type *type,
			struct memblock_region *reg, size_t i)
{
	if (type->size == type->capacity)
		return -1;

	for (size_t j = type->size; j > i; --j)
		type->regions[j] = type->regions[j - 1];
	type->regions[i] = *reg;
	++type->size;
	return 0;
}

static int memblock_add_range(struct memblock_type *type, uint64_t addr,
			uint64_t size)
{
	struct memblock_region reg = { addr, size };
	size_t i;

	if (!size)
		return 0;

	struct memblock_region *dst;

	for (i = 0; i != type->size; ++i) {
		dst = &type->regions[i];
		uint64_t rend = dst->addr + dst->size;

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
static uint64_t memblock_find_in_range(size_t size, size_t align,
			uint64_t from, uint64_t to)
{
	struct memblock_type *mem = &memblock.memory;
	struct memblock_type *res = &memblock.reserved;

	/*
	 j is an index of current region in reserved memory blocks array.
	*/
	size_t j = 0;

	/* i iterates over all accessible memory regions. */
	for (size_t i = 0; i != mem->size; ++i) {
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
		uint64_t addr = mem->regions[i].addr;
		uint64_t end = addr + mem->regions[i].size;

		addr = maxu(addr, from);
		end = minu(end, to);

		while (addr < end) {
			/* Align addr on requested border. */
			addr = align_up(addr, align);

			/* Has the region got enough space? */
			if (addr + size > end)
				break;

			/* [raddr, rend) is current reserved region. */
			uint64_t raddr = 0;
			uint64_t rend = 0;

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

int memblock_add(uint64_t addr, uint64_t size)
{
	return memblock_add_range(&memblock.memory, addr, size);
}

int memblock_reserve(uint64_t addr, uint64_t size)
{
	return memblock_add_range(&memblock.reserved, addr, size);
}

uint64_t memblock_alloc_range(size_t size, size_t align, uint64_t from,
			uint64_t to)
{
	const uint64_t addr = memblock_find_in_range(size, align, from, to);

	if (addr && !memblock_reserve(addr, size)) {
		debug("Allocated region 0x%x-0x%x\n", (unsigned int)addr,
					(unsigned int)(addr + size - 1));
		memset(virt_addr(addr), 0, size);
	}
	return addr;
}

int memblock_is_free(uint64_t addr, uint64_t size)
{
	const uint64_t end = addr + size;

	return memblock_find_in_range(size, 1, addr, end) != 0;
}
