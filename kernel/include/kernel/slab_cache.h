#ifndef __KERNEL_SLAB_CACHE_H__
#define __KERNEL_SLAB_CACHE_H__

#include <kernel/page_alloc.h>
#include <kernel/list.h>

struct slab_cache;
struct slab {
	struct list_head free_list;
	struct list_head slab_list;
	struct slab_cache *cache;
	unsigned long size;
};

struct slab_cache {
	struct list_head partial;
	struct list_head full;

	unsigned long object_size;
	unsigned object_alignment;
};

struct slab_place {
	struct list_head link;
	unsigned long size;
};

void slab_cache_create(struct slab_cache *cache, unsigned long size,
			unsigned align);
void slab_cache_destroy(struct slab_cache *cache);

void *slab_cache_alloc(struct slab_cache *cache, unsigned flags);
void slab_cache_free(struct slab_cache *cache, void *ptr);

#endif /*__KERNEL_SLAB_CACHE_H__*/
