#include <kernel/slab_cache.h>
#include <kernel/debug.h>
#include <arch/memory.h>
#include "multiboot.h"

void setup_arch(void *bootstrap)
{
	void init_io(void);

	struct multiboot_info *mbi = bootstrap;
	init_io();
	setup_memory(mbi);

	struct slab_cache cache;
	slab_cache_create(&cache, sizeof(unsigned long), sizeof(unsigned long));

	unsigned long *ulptr1 = slab_cache_alloc(&cache, ALLOC_NORMAL);
	debug("Returned ptr 0x%x\n", (unsigned long)ulptr1);
	unsigned long *ulptr2 = slab_cache_alloc(&cache, ALLOC_NORMAL);
	debug("Returned ptr 0x%x\n", (unsigned long)ulptr2);

	slab_cache_free(&cache, ulptr2);
	slab_cache_free(&cache, ulptr1);

	slab_cache_destroy(&cache);
}
