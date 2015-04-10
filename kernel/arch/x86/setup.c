#include <arch/memory.h>
#include <arch/multiboot.h>
#include <arch/early_console.h>

void setup_arch(void *bootstrap)
{
	struct multiboot_info *mbi = bootstrap;

	init_vga_console();
	init_serial_console();
	setup_memory(mbi);
}
