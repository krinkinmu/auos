#include "scheduler.h"
#include "idt.h"

struct task_state {
	unsigned long edi;
	unsigned long esi;
	unsigned long ebx;
	unsigned long ebp;
};

struct task_start_frame {
	struct task_state state;
	void (*entry)(struct task *, task_fptr_t, void *data);
	unsigned long dummy;
	struct task *task;
	task_fptr_t fptr;
	void *data;
};

static struct list_head tasks;

extern void task_state_switch(struct task_state **old, struct task_state *new);

void schedule(void)
{
	const unsigned long flags = irqsave();
	struct task *next, *this;

	if (!list_empty(&tasks)) {
		this = current;

		if (this->state == READY)
			list_add_tail(&tasks, &this->queue);

		next = list_entry(tasks.next, struct task, queue);
		list_del(tasks.next);

		task_state_switch(&this->task_state, next->task_state);
		current = this;
		current->remain = SCHEDULER_TIME_SLICE;
	}
	irqrestore(flags);
}

void task_start(struct task *this, task_fptr_t fptr, void *data)
{
	enable_interrupts();
	current = this;
	fptr(data);
	this->state = FINISHED;
	// let make it simple, so far...
	while (1) schedule();
}

void task_new(struct task *task, void *stack, unsigned size,
			task_fptr_t fptr, void *data)
{
	struct task_start_frame *frame = (struct task_start_frame *)
				((char *)stack + size - sizeof(*frame));
	unsigned long flags;

	frame->state.edi = 0;
	frame->state.esi = 0;
	frame->state.ebx = 0;
	frame->state.ebp = 0;
	frame->dummy = 0;
	
	frame->entry = task_start;
	frame->task = task;
	frame->fptr = fptr;
	frame->data = data;

	task->task_state = &frame->state;
	task->state = READY;

	flags = irqsave();
	list_add_tail(&tasks, &task->queue);
	irqrestore(flags);
}

void scheduler_setup(void)
{ list_init(&tasks); }
