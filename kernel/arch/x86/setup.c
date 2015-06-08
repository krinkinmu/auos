#include <kernel/debug.h>
#include <kernel/list.h>
#include <kernel/task.h>
#include <kernel/acpi.h>
#include <kernel/irq.h>
#include <kernel/utility.h>

#include <arch/task_state.h>
#include <arch/memory.h>
#include <arch/multiboot.h>
#include <arch/early_console.h>
#include <arch/gdt.h>
#include <arch/idt.h>
#include <arch/apic.h>
#include <arch/i8259a.h>


static struct gdt_entry gdt[GDT_SIZE];
static struct gdt_ptr gdt_ptr;

static void setup_gdt(void)
{
	init_gdt_segment(&gdt[KERNEL_CODE_ENTRY], 0xFFFFFULL, 0x0ULL,
		KERNEL_CODE_TYPE);
	init_gdt_segment(&gdt[KERNEL_DATA_ENTRY], 0xFFFFFULL, 0x0ULL,
		KERNEL_DATA_TYPE);
	init_gdt_segment(&gdt[USER_CODE_ENTRY], 0xFFFFFULL, 0x0ULL,
		USER_CODE_TYPE);
	init_gdt_segment(&gdt[USER_DATA_ENTRY], 0xFFFFFULL, 0x0ULL,
		USER_DATA_TYPE);

	init_gdt_ptr(&gdt_ptr, gdt, GDT_SIZE);
	set_gdt(&gdt_ptr);
	reload_registers();
}

static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idt_ptr;

struct arch_irq_context {
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t edx;
	uint32_t ecx;
	uint32_t ebx;
	uint32_t eax;
	uint32_t irq;
	uint32_t err;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
} __attribute__((packed));

void raw_irq_handler(struct arch_irq_context *ctx) {
	debug("%d %x %x %x %x\n",
		ctx->irq, ctx->err, ctx->eip, ctx->cs, ctx->eflags);
	while (1);
}

static void setup_idt(void)
{
	extern char raw_irq_handler_entries[];
	const unsigned long entries = (unsigned long)raw_irq_handler_entries;
	
	for (unsigned i = 0; i != 256; ++i)
		init_isr_gate(idt + i, entries + i * 16, IDT_KERNEL);

	init_idt_ptr(&idt_ptr, idt, IDT_SIZE);
	set_idt(&idt_ptr);
}

static inline struct task_state *task_state(struct task *task)
{
	return (struct task_state *)(task + 1);
}

static inline void *task_stack(struct task *task)
{
	return (char *)task + KERNEL_STACK_SIZE;
}

static void setup_swapper(void)
{
	struct task *task = current_task();
	struct task_state *state = task_state(task);

	debug("Swapper task %x\n", (unsigned)task);

	memset(state, 0, sizeof(*state));
	state->prev_tss = TSS2_SELECTOR;
	state->cr3 = (uint32_t)phys_addr(swapper_page_dir);
	init_gdt_segment(&gdt[TSS1_ENTRY], sizeof(*state), (uint32_t)state,
				FREE_TSS_TYPE);
	load_tr(TSS1_SELECTOR);
}

void task_init(struct task *task, void (*entry)(void))
{
	struct task_state *state = task_state(task);

	memset(state, 0, sizeof(*state));
	state->esp = (uint32_t)task_stack(task);
	state->ss = KERNEL_DATA_SELECTOR;
	state->ds = KERNEL_DATA_SELECTOR;
	state->es = KERNEL_DATA_SELECTOR;
	state->fs = KERNEL_DATA_SELECTOR;
	state->gs = KERNEL_DATA_SELECTOR;
	state->cs = KERNEL_CODE_SELECTOR;
	state->eip = (uint32_t)entry;
	state->cr3 = (uint32_t)phys_addr(swapper_page_dir);
}

/*
static void print_task_state(struct task_state *state)
{
	#define PRINT_STATE(arg) debug(#arg " %x\n", state->arg)
	PRINT_STATE(prev_tss);
	PRINT_STATE(ring0_esp);
	PRINT_STATE(ring0_ss);
	PRINT_STATE(ring1_esp);
	PRINT_STATE(ring1_ss);
	PRINT_STATE(ring2_esp);
	PRINT_STATE(ring2_ss);
	PRINT_STATE(cr3);
	PRINT_STATE(eip);
	PRINT_STATE(eflags);
	PRINT_STATE(eax);
	PRINT_STATE(ecx);
	PRINT_STATE(edx);
	PRINT_STATE(ebx);
	PRINT_STATE(esp);
	PRINT_STATE(ebp);
	PRINT_STATE(esi);
	PRINT_STATE(edi);
	PRINT_STATE(es);
	PRINT_STATE(cs);
	PRINT_STATE(ss);
	PRINT_STATE(ds);
	PRINT_STATE(fs);
	PRINT_STATE(gs);
	PRINT_STATE(lds);
	PRINT_STATE(trap);
	PRINT_STATE(iopb);
	#undef PRINT_STATE
}
*/
void task_switch(struct task *next)
{
	struct task *prev = current_task();
	uint16_t tss = task_state(prev)->prev_tss;
	struct task_state *state = task_state(next);

/*
	debug("Next TSS selector %x\n", tss);
	debug("Current task state:\n");
	print_task_state(task_state(prev));
	debug("Next task state:\n");
	print_task_state(task_state(next));
*/

	init_gdt_segment(&gdt[GDT_ENTRY(tss)], sizeof(*state), (uint32_t)state,
		FREE_TSS_TYPE);

	if (tss == TSS1_SELECTOR) {
		state->prev_tss = TSS2_SELECTOR;
		__asm__ ("ljmp %0, $0" : : "i"(TSS1_SELECTOR));
	} else {
		state->prev_tss = TSS1_SELECTOR;
		__asm__ ("ljmp %0, $0" : : "i"(TSS2_SELECTOR));
	}
}

void setup_arch(void *bootstrap)
{
	struct multiboot_info *mbi = bootstrap;

	init_vga_console();
	init_serial_console();
	setup_gdt();
	setup_idt();
	setup_i8259a();
	setup_memory(mbi);
	setup_swapper();
	setup_acpi();
	setup_apic();
	setup_local_apic();
}
