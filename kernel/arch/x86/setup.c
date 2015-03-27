#include <arch/x86/memory.h>
#include "multiboot.h"

void init_io(void);

void setup_arch(void *bootstrap)
{
	struct multiboot_info *mbi = bootstrap;
	init_io();
	setup_memory(mbi);
}
