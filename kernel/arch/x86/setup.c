#include <kernel/slab_cache.h>
#include <kernel/debug.h>

#include <arch/memory.h>
#include <arch/io.h>

#include "multiboot.h"

void setup_arch(void *bootstrap)
{
	struct multiboot_info *mbi = bootstrap;

	init_io();
	setup_memory(mbi);
}
