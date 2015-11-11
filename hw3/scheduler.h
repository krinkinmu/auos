#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "list.h"

#define SCHEDULER_TIME_SLICE 10

struct task_state;

enum state {
	READY,
	BLOCKED,
	FINISHED
};

struct task {
	struct list_head queue;
	struct task_state *task_state;
	long remain;
	enum state state;
};

typedef void (*task_fptr_t)(void *);

struct task *current;

void task_start(struct task *this, task_fptr_t fptr, void *data);
void task_new(struct task *task, void *stack, unsigned size,
			task_fptr_t fptr, void *data);
void schedule(void);
void scheduler_setup(void);

#endif /*__SCHEDULER_H__*/
