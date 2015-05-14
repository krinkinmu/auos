#ifndef __KERNEL_TASK_H__
#define __KERNEL_TASK_H__

#include <kernel/list.h>
#include <arch/memory.h>

typedef int tid_t;

struct task {
	struct list_head link;
	tid_t tid;
};

static inline struct task *current_task(void)
{
	unsigned long sp = ((unsigned long)&sp) & ~(KERNEL_STACK_ALIGN - 1);

	return (struct task *)sp;
}

struct task *alloc_task(void);
void free_task(struct task *task);
void task_init(struct task *task, void (*entry)(void));
void task_switch(struct task *task);

#endif /*__KERNEL_TASK_H__*/
