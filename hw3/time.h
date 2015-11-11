#ifndef __TIME_H__
#define __TIME_H__

#define TIMER_FREQ       1193182
#define TICKS_PER_SECOND 100
#define TIMER_FDIV       (TIMER_FREQ / TICKS_PER_SECOND)
#define PIT_IRQ          0

unsigned long long get_ticks(void);
void time_setup(void);

#endif /*__TIME_H__*/
