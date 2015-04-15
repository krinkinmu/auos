#include <kernel/list.h>
#include <kernel/task.h>

#include <arch/init_task.h>
#include <arch/descriptor_table.h>

static struct task *init;

struct task *init_task(void)
{
	return init;
}

void init_task_init(void)
{
	const struct descriptor_ptr gdt_ptr = get_gdt();
	gdt_entry_t *gdt = (gdt_entry_t *)gdt_ptr.base;
	struct task *task = current_task();
	struct task_state *state = (struct task_state *)(task + 1);

	list_init_head(&task->link);
	task->state = (struct task_state *)(init + 1);
	task->tid = 1;

	state->prev_tss = TSS2_SELECTOR;
	state->ring0_esp = (uint32_t)((char *)task + PAGE_SIZE);
	state->ring0_ss = KERNEL_DATA_SELECTOR;
	state->cr3 = (uint32_t)phys_addr(swapper_page_dir);

	init_entry(&gdt[TSS1_ENTRY], sizeof(*state), (uint32_t)state,
			FREE_TSS_TYPE);
	init = task;
	load_tr(TSS1_SELECTOR);
}
