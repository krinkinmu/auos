#ifndef __KERNEL_SLAB_CACHE_H__
#define __KERNEL_SLAB_CACHE_H__

#include <kernel/page_alloc.h>
#include <kernel/list.h>

#include <stddef.h>

struct slab;
struct slab_cache {
	struct list_head partial;
	struct list_head full;
	unsigned object_size;
	unsigned object_count;
	unsigned object_offset;
};

void slab_cache_create(struct slab_cache *cache, size_t size, size_t align);
void slab_cache_destroy(struct slab_cache *cache);

void *slab_cache_alloc(struct slab_cache *cache, unsigned flags);
void slab_cache_free(struct slab_cache *cache, void *ptr);

#endif /*__KERNEL_SLAB_CACHE_H__*/
