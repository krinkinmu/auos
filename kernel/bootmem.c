#include <kernel/bootmem.h>
#include <kernel/kernel.h>

#define BOOT_MEM_RANGES 128

struct boot_mem_range {
        unsigned long begin;
        unsigned long end;
};

struct boot_mem_type {
        struct boot_mem_range *ranges;
        unsigned size;
        unsigned capacity;
};

struct boot_mem {
        struct boot_mem_type memory;
        struct boot_mem_type reserved;
};

static struct boot_mem_range memory_ranges[BOOT_MEM_RANGES];
static struct boot_mem_range reserved_ranges[BOOT_MEM_RANGES];

static struct boot_mem boot_mem = {
	.memory = { memory_ranges, 0, BOOT_MEM_RANGES },
	.reserved = { reserved_ranges, 0, BOOT_MEM_RANGES }
};


static void boot_mem_merge(struct boot_mem_range *dst,
			const struct boot_mem_range *src)
{
	const unsigned long begin = minu(src->begin, dst->begin);
	const unsigned long end = maxu(src->end, dst->end);

	dst->begin = begin;
	dst->end = end;
}

static int boot_mem_overlap(const struct boot_mem_range *l,
			const struct boot_mem_range *r)
{
	return (l->begin <= r->begin && r->begin <= l->end) ||
		(l->begin <= r->end && r->end <= l->end);
}

static void boot_mem_remove(struct boot_mem_type *type, unsigned i)
{
	for (; i + 1 < type->size; ++i)
		type->ranges[i] = type->ranges[i + 1];
	--type->size;
}

static int boot_mem_insert(struct boot_mem_type *type,
			const struct boot_mem_range *range, unsigned i)
{
	if (type->size == type->capacity)
		return -1;

	for (unsigned long j = type->size; j > i; --j)
		type->ranges[j] = type->ranges[j - 1];
	type->ranges[i] = *range;
	++type->size;
	return 0;
}

static int boot_mem_add_range(struct boot_mem_type *type, unsigned long begin,
			unsigned long end)
{
	if (end <= begin)
		return 0;

	const struct boot_mem_range range = { begin, end };
	unsigned i;

	for (i = 0; i != type->size; ++i) {
		const unsigned long end = type->ranges[i].end;

		if (end >= begin)
			break;
	}

	struct boot_mem_range *dst = type->ranges + i;
	if (i != type->size && boot_mem_overlap(dst, &range)) {
		boot_mem_merge(dst, &range);
		if (i + 1 != type->size && boot_mem_overlap(dst, dst + 1)) {
			boot_mem_merge(dst, dst + 1);
			boot_mem_remove(type, i + 1);
		}
		return 0;
	}

	return boot_mem_insert(type, &range, i);
}

static unsigned long boot_mem_find(unsigned long size, unsigned align,
			unsigned long from, unsigned long to)
{
	if (from > to || size > to - from)
		return to;

	if (!size)
		return from;

	const struct boot_mem_type *mem = &boot_mem.memory;
	const struct boot_mem_type *res = &boot_mem.reserved;
	unsigned j = 0;

	for (unsigned i = 0; i != mem->size; ++i) {
		if (mem->ranges[i].end <= from)
			continue;

		if (mem->ranges[i].begin >= to)
			break;

		const unsigned long end = minu(mem->ranges[i].end, to);
		unsigned long begin = maxu(mem->ranges[i].begin, from);

		while (begin < end) {
			begin = align_up(begin, align);

			if (begin + size > end)
				break;

			unsigned long rbegin;
			unsigned long rend;

			for (; j != res->size; ++j) {
				rbegin = res->ranges[j].begin;
				rend = res->ranges[j].end;

				if (rend > begin)
					break;
			}

			if (j == res->size || rbegin >= begin + size)
				return begin;

			begin = rend;
		}
	}

	return to;
}

int boot_mem_add(unsigned long begin, unsigned long end)
{
	return boot_mem_add_range(&boot_mem.memory, begin, end);
}

int boot_mem_reserve(unsigned long begin, unsigned long end)
{
	return boot_mem_add_range(&boot_mem.reserved, begin, end);
}

unsigned long boot_mem_alloc(unsigned long size, unsigned align,
			unsigned long from, unsigned long to)
{
	const unsigned long addr = boot_mem_find(size, align, from, to);

	if (addr != to && !boot_mem_reserve(addr, addr + size))
		return addr;
	return to;
}

int boot_mem_is_free(unsigned long begin, unsigned long end)
{
	return boot_mem_find(end - begin, 1, begin, end) != 0;
}
