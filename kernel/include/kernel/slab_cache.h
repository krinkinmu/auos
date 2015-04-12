#ifndef __KERNEL_SLAB_CACHE_H__
#define __KERNEL_SLAB_CACHE_H__

#include <kernel/page_alloc.h>
#include <kernel/list.h>

#include <stddef.h>

struct slab_cache;
struct slab {
	struct list_head free_list;
	struct list_head slab_list;
	struct slab_cache *cache;
	size_t size;
};

struct slab_cache {
	struct list_head partial;
	struct list_head full;

	size_t object_size;
	size_t object_alignment;
};

struct slab_place {
	struct list_head link;
	size_t size;
};

void slab_cache_create(struct slab_cache *cache, size_t size, size_t align);
void slab_cache_destroy(struct slab_cache *cache);

void *slab_cache_alloc(struct slab_cache *cache, unsigned flags);
void slab_cache_free(struct slab_cache *cache, void *ptr);

#endif /*__KERNEL_SLAB_CACHE_H__*/
