#include <arch/x86/memory.h>
#include "multiboot.h"
#include "io.h"

void setup_arch(void *bootstrap)
{
	struct multiboot_info *mbi = bootstrap;

	init_io();
	puts("Hello, World!");
	setup_memory(mbi);
}

void main(void *bootstrap)
{
	setup_arch(bootstrap);
	while (1);
}
