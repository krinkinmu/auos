CC ?= gcc
LD ?= ld

CFLAGS := -m32 -ffreestanding -O3
LFLAGS := -s -nostdlib -nostdinc

SRC := \
	main.c \
	stdio.c \
	pic.c \
	ioport.c \
	gdt.c \
	idt.c \
	irq.c \
	time.c \
	exceptions.c \
	list.c \
	scheduler.c \
	serial.c
OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.d)

ASM := multiboot.S switch.S interrupts.S
AOBJ:= $(ASM:.S=.o)

all: multiboot

multiboot: $(AOBJ) $(OBJ) multiboot.ld
	ld $(LFLAGS) -T multiboot.ld -o $@ $(AOBJ) $(OBJ)
	strip -R .eh_frame -R .comment -R .shstrtab $@

%.o: %.S
	$(CC) -m32 -c $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

interrupts.S: gen_interrupts.py
	python $^ > $@

-include $(DEP)

.PHONY: clean
clean:
	rm -f multiboot interrupts.S $(AOBJ) $(OBJ) $(DEP)
