#include <kernel/scheduler.h>
#include <kernel/kernel.h>
#include <kernel/debug.h>
#include <kernel/task.h>
#include <kernel/list.h>

static struct list_head active = { &active, &active };

void schedule_task(struct task *task)
{
	list_insert_before(&active, &task->link);
}

void schedule(void)
{
	struct task *current = current_task();

	if (list_empty(&active))
		return;

	struct task *next = CONTAINER_OF(active.next, struct task, link);

	list_remove(&next->link);
	list_insert_before(&active, &current->link);
	task_switch(next);
}
