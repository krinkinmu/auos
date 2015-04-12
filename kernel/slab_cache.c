#include <kernel/slab_cache.h>
#include <kernel/kernel.h>
#include <kernel/debug.h>

void slab_cache_create(struct slab_cache *cache, size_t size, size_t align)
{
	list_init_head(&cache->partial);
	list_init_head(&cache->full);

	if (size < sizeof(struct slab_place))
		size = sizeof(struct slab_place);

	cache->object_size = align_up(size, align);
	cache->object_alignment = align;
}

void slab_cache_destroy(struct slab_cache *cache)
{
	assert(list_empty(&cache->full),
		"Slab cache still contains entries\n");

	struct list_head *head = &cache->partial;
	struct list_head *curr = head->next;

	while (curr != head) {
		struct slab *slab = container_of(curr, struct slab, free_list);
		assert(slab->size, "Slab cache still contains entries\n");
		curr = curr->next;
	}
}

static void slab_init(struct slab_cache *cache, struct slab *slab,
			char *begin, char *end)
{
	const size_t align = cache->object_alignment;
	const size_t size = cache->object_size;

	begin = (char *)align_up((uintptr_t)begin, align);
	end = (char *)align_down((uintptr_t)end, align);

	assert(begin + size <= end, "Slab size too small\n");

	list_init_head(&slab->free_list);
	slab->cache = cache;
	slab->size = 0;

	struct slab_place *const place = (void *)begin;

	place->size = end - begin;
	list_insert_before(&slab->free_list, &place->link);
	list_insert_before(&cache->partial, &slab->slab_list);
}

static void slab_cache_grow(struct slab_cache *cache, unsigned order,
			unsigned flags)
{
	struct page *pages = alloc_pages(order, flags);

	if (!pages)
		return;

	char *ptr = page_virtual_address(pages);
	struct slab *slab = (struct slab *)ptr;

	slab_init(cache, slab, ptr + sizeof(*slab),
				ptr + (1 << (order + PAGE_SHIFT)));

	for (unsigned i = 0; i != 1u << order; ++i)
		(pages + i)->slab = slab;
}

static inline int slab_is_full(const struct slab *slab)
{
	return list_empty(&slab->free_list);
}

static inline int slab_cache_is_full(const struct slab_cache *cache)
{
	return list_empty(&cache->partial);
}

static void *slab_alloc_in_slab(struct slab_cache *cache, struct slab *slab)
{
	struct slab_place *place = container_of(slab->free_list.next,
						struct slab_place, link);
	void *ptr = (char *)place + (place->size - cache->object_size);

	if (place->size == cache->object_size)
		list_remove(&place->link);
	else
		place->size -= cache->object_size;
	++slab->size;
	return ptr;
}

void *slab_cache_alloc(struct slab_cache *cache, unsigned flags)
{
	if (slab_cache_is_full(cache))
		slab_cache_grow(cache, 0, flags);

	if (slab_cache_is_full(cache))
		return 0;

	struct slab *slab = container_of(cache->partial.next,
						struct slab, slab_list);
	void *ptr = slab_alloc_in_slab(cache, slab);

	if (slab_is_full(slab)) {
		list_remove(&slab->slab_list);
		list_insert_after(&cache->full, &slab->slab_list);
	}
	return ptr;
}

static void slab_free_in_slab(struct slab_cache *cache, struct slab *slab,
			void *ptr)
{
	struct slab_place *place = ptr;

	place->size = cache->object_size;
	list_insert_before(&slab->free_list, &place->link);
	--slab->size;
}

void slab_cache_free(struct slab_cache *cache, void *ptr)
{
	struct page *const page = pfn_to_page(phys_addr(ptr) >> PAGE_SHIFT);
	struct slab *const slab = page->slab;
	const int full = slab_is_full(slab);

	slab_free_in_slab(cache, slab, ptr);
	if (full) {
		list_remove(&slab->slab_list);
		list_insert_before(&cache->partial, &slab->slab_list);
	}
}
