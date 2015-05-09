ARCH_ASM_SOURCES := \
	raw_irq_handler.S

ARCH_C_SOURCES   := \
	setup.c \
	vga_console.c \
	serial_console.c \
	setup_memory.c \
	setup_page_alloc.c \
	gdt.c \
	idt.c \
	acpi.c \
	ioport.c \
	kmap.c \
	apic.c \
	i8259a.c
