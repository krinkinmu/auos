#include "memory.h"
#include "multiboot.h"

void setup_arch(void *bootstrap)
{
	void init_io(void);

	struct multiboot_info *mbi = bootstrap;
	init_io();
	setup_memory(mbi);
}
