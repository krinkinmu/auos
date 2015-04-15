#include <kernel/debug.h>
#include <kernel/task.h>

#include <arch/memory.h>
#include <arch/multiboot.h>
#include <arch/early_console.h>
#include <arch/init_task.h>

void setup_arch(void *bootstrap)
{
	struct multiboot_info *mbi = bootstrap;

	init_vga_console();
	init_serial_console();
	setup_memory(mbi);
	init_task_init();

	struct task *current = current_task();
	debug("current task id = %d\n", current->tid);
}
