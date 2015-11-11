#include "scheduler.h"
#include "idt.h"
#include "gdt.h"
#include "pic.h"
#include "stdio.h"

#define STACK_SIZE 4096

static char stack2[STACK_SIZE];
static struct task task1, task2;
static char *message;

#define barrier() __asm__ volatile ("" ::: "memory")

static void task1_function(void *data)
{
	char **msg = (char **)data;
	char buffer[256];

	while (1) {
		unsigned count;

		while (*msg) {
			barrier();
			schedule();
		}

		count = serial_read(buffer, sizeof(buffer) - 1);
		buffer[count] = 0;
		if (count)
			*msg = buffer;
	}
}

static void task2_function(void *data)
{
	char **msg = (char **)data;

	while (1) {
		char *ptr;

		while (!*msg) {
			barrier();
			schedule();
		}

		ptr = *msg;
		myputs(ptr);
		*msg = 0;
	}
}

void main(void)
{
	clrscr();
	idt_setup();
	gdt_setup();
	exceptions_setup();
	irq_setup();
	pic_setup();
	time_setup();
	serial_setup();
	scheduler_setup();

	task_new(&task2, stack2, STACK_SIZE, task2_function, NULL);
	task_start(&task1, &task1_function, NULL);

	while (1);
}
