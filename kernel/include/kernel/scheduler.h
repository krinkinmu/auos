#ifndef __KERNEL_SCHEDULER_H__
#define __KERNEL_SCHEDULER_H__

struct task;

void schedule_task(struct task *task);
void schedule(void);

#endif /*__KERNEL_SCHEDULER_H__*/
