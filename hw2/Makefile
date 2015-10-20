CC ?= gcc
LD ?= ld

CFLAGS := -m32 -ffreestanding -O3
LFLAGS := -s -nostdlib -nostdinc

SRC := main.c stdio.c pit.c pic.c ioport.c gdt.c idt.c
OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.d)

all: multiboot

multiboot: multiboot.o handler.o $(OBJ) multiboot.ld
	ld $(LFLAGS) -T multiboot.ld -o $@ multiboot.o handler.o $(OBJ)
	strip -R .eh_frame -R .comment -R .shstrtab $@

multiboot.o: multiboot.S
	$(CC) -m32 -c $^ -o $@

handler.o: handler.S
	$(CC) -m32 -c $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(DEP)

.PHONY: clean
clean:
	rm -f multiboot multiboot.o $(OBJ) $(DEP)
