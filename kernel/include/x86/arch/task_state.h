#ifndef __X86_ARCH_TASK_STATE_H__
#define __X86_ARCH_TASK_STATE_H__

#include <stdint.h>

struct task_state {
	uint32_t prev_tss;
	uint32_t ring0_esp;
	uint32_t ring0_ss;
	uint32_t ring1_esp;
	uint32_t ring1_ss;
	uint32_t ring2_esp;
	uint32_t ring2_ss;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t lds;
	uint16_t trap;
	uint16_t iopb;
} __attribute__((packed));

static inline void load_tr(uint16_t sel)
{
	__asm__ ("ltr %0" : : "a"(sel));
}

#endif /*__X86_ARCH_TASK_STATE_H__*/
