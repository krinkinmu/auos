#ifndef __KERNEL_TASK_H__
#define __KERNEL_TASK_H__

#include <kernel/kernel.h>
#include <kernel/list.h>

#include <arch/task_state.h>
#include <arch/memory.h>

typedef int tid_t;

struct task {
	struct list_head link;
	struct task_state *state;
	tid_t tid;
};

static inline struct task_state *task_state(struct task *task)
{
	return task->state;
}

static inline struct task *current_task(void)
{
	unsigned long sp = ((unsigned long)&sp) & ~(KERNEL_STACK_ALIGN - 1);

	return (struct task *)sp;
}

struct task *init_task(void);

#endif /*__KERNEL_TASK_H__*/
