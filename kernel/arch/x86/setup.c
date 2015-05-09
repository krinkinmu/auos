#include <kernel/debug.h>
#include <kernel/list.h>
#include <kernel/task.h>
#include <kernel/acpi.h>
#include <kernel/irq.h>

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

void arch_raw_irq_handler(struct arch_irq_context *ctx) {
	debug("%x %x %x %x %x\n",
		ctx->irq, ctx->err, ctx->eip, ctx->cs, ctx->eflags);
	while (1);
}

static void setup_idt(void)
{
	extern char raw_irq_handler[];
	
	for (unsigned i = 0; i != 256; ++i) {
		const unsigned long isr =
			(unsigned long)(raw_irq_handler + i * 16);

		init_isr_gate(idt + i, isr, IDT_KERNEL);
	}

	init_idt_ptr(&idt_ptr, idt, IDT_SIZE);
	set_idt(&idt_ptr);
}

static struct task *init;

static void setup_init(void)
{
	struct task *task = current_task();
	struct task_state *state = (struct task_state *)(task + 1);

	list_init_head(&task->link);
	task->state = (struct task_state *)(init + 1);
	task->tid = 1;

	state->prev_tss = TSS2_SELECTOR;
	state->ring0_esp = (uint32_t)((char *)task + PAGE_SIZE);
	state->ring0_ss = KERNEL_DATA_SELECTOR;
	state->cr3 = (uint32_t)phys_addr(swapper_page_dir);

	init_gdt_segment(&gdt[TSS1_ENTRY], sizeof(*state), (uint32_t)state,
				FREE_TSS_TYPE);
	init = task;
	load_tr(TSS1_SELECTOR);
}

struct task *init_task(void)
{
	return init;
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
	setup_acpi();
	setup_init();
	setup_apic();
	setup_local_apic();
	__asm__ ("int $0x22");
}
