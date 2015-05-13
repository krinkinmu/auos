#include <kernel/slab_cache.h>
#include <kernel/kernel.h>
#include <kernel/debug.h>
#include <kernel/utility.h>

#include <limits.h>

#define SLAB_ORDER 1

typedef unsigned long bitmap_t;

struct slab {
	struct list_head link;
	struct slab_cache *cache;
	unsigned free;
	bitmap_t bitmap[];
};

static unsigned slab_object_size(unsigned size, unsigned alignment)
{
	if (alignment >= size)
		return alignment;
	return alignment * ((size + alignment - 1) / alignment);
}

static unsigned slab_object_count(unsigned size, unsigned space)
{
	const unsigned ssize = sizeof(struct slab);
	const unsigned bsize = sizeof(bitmap_t);

	return (CHAR_BIT * (space - ssize - bsize) + 1)
		/ (CHAR_BIT * size + 1);
}

static void slab_init(struct slab_cache *cache, struct slab *slab)
{
	memset(slab, 0, cache->object_offset);
	slab->cache = cache;
	slab->free = cache->object_count;
}

static void slab_cache_init(struct slab_cache *cache, size_t size,
			size_t align)
{
	const unsigned space = PAGE_SIZE << SLAB_ORDER;
	const unsigned object_size = slab_object_size(size, align);
	const unsigned object_count = slab_object_count(object_size, space);
	const unsigned object_offset = space - object_size * object_count;

	list_init_head(&cache->partial);
	list_init_head(&cache->full);
	cache->object_size = object_size;
	cache->object_count = object_count;
	cache->object_offset = object_offset;
}

static void slab_cache_fini(struct slab_cache *cache)
{
	assert(list_empty(&cache->full), "Slab cache is still not empty\n");

	struct list_head *head = &cache->partial;
	struct list_head *curr = head->next;

	while (curr != head) {
		struct slab *slab = CONTAINER_OF(curr, struct slab, link);
		struct page *page = pfn_to_page(phys_addr(slab) >> PAGE_SHIFT);

		assert(slab->free == cache->object_count,
			"Slab cache is still not empty\n");
		curr = curr->next;
		free_pages(page, SLAB_ORDER);
	}
}

static void slab_cache_grow(struct slab_cache *cache, unsigned flags)
{
	struct page *pages = alloc_pages(SLAB_ORDER, flags);

	if (!pages)
		return;

	struct slab *slab = (struct slab *)page_virtual_address(pages);

	slab_init(cache, slab);
	list_insert_before(&cache->partial, &slab->link);
	for (unsigned i = 0; i != BITU(SLAB_ORDER); ++i)
		pages[i].slab = slab;
}

static void *slab_alloc(struct slab_cache *cache, struct slab *slab)
{
	unsigned i = 0;

	while (~slab->bitmap[i] == 0)
		++i;

	bitmap_t bmap = slab->bitmap[i];
	unsigned j = 0;

	while (bmap % 2) {
		bmap >>= 1;
		++j;
	}

	unsigned off = i * sizeof(bitmap_t) * CHAR_BIT + j;

	--slab->free;
	slab->bitmap[i] |= ((bitmap_t)1 << j);
	return (char *)slab + cache->object_offset + off * cache->object_size;
}

static void slab_free(struct slab_cache *cache, struct slab *slab, char *ptr)
{
	const unsigned offset = cache->object_offset;
	const unsigned size = cache->object_size;
	const unsigned wsize = sizeof(bitmap_t) * 8;
	const unsigned bit = (ptr - ((char *)slab + offset)) / size;
	const unsigned word = bit / wsize;
	const unsigned shift = bit % wsize;

	slab->bitmap[word] &= ~((bitmap_t)1 << shift);
	++slab->free;
}

void slab_cache_create(struct slab_cache *cache, size_t size, size_t align)
{
	slab_cache_init(cache, size, align);
}

void slab_cache_destroy(struct slab_cache *cache)
{
	slab_cache_fini(cache);
}

void *slab_cache_alloc(struct slab_cache *cache, unsigned flags)
{
	if (list_empty(&cache->partial))
		slab_cache_grow(cache, flags);

	if (list_empty(&cache->partial))
		return 0;

	struct slab *slab =
		CONTAINER_OF(cache->partial.next, struct slab, link);
	void *ptr = slab_alloc(cache, slab);

	if (!slab->free) {
		list_remove(&slab->link);
		list_insert_after(&cache->full, &slab->link);
	}
	return ptr;
}

void slab_cache_free(struct slab_cache *cache, void *ptr)
{
	struct page *page = pfn_to_page(phys_addr(ptr) >> PAGE_SHIFT);
	struct slab *slab = page->slab;
	int full = (slab->free == 0);

	slab_free(cache, slab, ptr);
	if (full) {
		list_remove(&slab->link);
		list_insert_before(&cache->partial, &slab->link);
	}
}
